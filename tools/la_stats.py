"""Compute pulse statistics from a Saleae Logic 2 CSV export (edge list).
Usage: python3 -I la_stats.py <file.csv>
Columns: Time [s], D0 CM4 copy, D1 half/full, D2 RFFT, D3 transfer, D4 convert, ...
"""
import csv
import sys
import statistics as st

path = sys.argv[1]
with open(path, newline="") as f:
    r = csv.reader(f)
    hdr = next(r)
    rows = [(float(x[0]), [int(v) for v in x[1:]]) for x in r if x]

nch = len(hdr) - 1
rise = {c: [] for c in range(nch)}
fall = {c: [] for c in range(nch)}
prev = rows[0][1]
for t, v in rows[1:]:
    for c in range(nch):
        if v[c] != prev[c]:
            (rise if v[c] else fall)[c].append(t)
    prev = v

def highs(c):
    """Pair each rising edge with the next falling edge -> list of (rise, fall)."""
    out = []
    fi = 0
    fl = fall[c]
    for t0 in rise[c]:
        while fi < len(fl) and fl[fi] <= t0:
            fi += 1
        if fi < len(fl):
            out.append((t0, fl[fi]))
    return out

def us(x):
    return x * 1e6

def stats(name, vals):
    if not vals:
        print(f"{name:34s} no data")
        return
    print(f"{name:34s} n={len(vals):6d}  min={min(vals):10.2f}  mean={st.mean(vals):10.2f}  "
          f"median={st.median(vals):10.2f}  max={max(vals):10.2f}")

print("file:", path)
print("channels:", hdr[1:])
print("capture span: %.3f s" % (rows[-1][0] - rows[0][0]))

# Active window: where D0 pulses happen
for c in range(nch):
    print(f"ch{c} {hdr[c+1]!r}: rises={len(rise[c])} falls={len(fall[c])}")

# D1: toggles (high = half done, low = full done) -> each level = one half-buffer
d1_edges = sorted(rise[1] + fall[1])
half_periods = [us(b - a) for a, b in zip(d1_edges, d1_edges[1:])]
# keep only within active streaming (drop gaps > 10x median)
if half_periods:
    med = st.median(half_periods)
    hp = [p for p in half_periods if p < 10 * med]
    stats("D1 half-buffer time (us)", hp)
    m = st.median(hp)
    print(f"   -> fs = 1024 / {m:.2f} us = {1024 / (m * 1e-6) / 1e3:.2f} kHz")

for c, name in [(0, "D0 CM4 copy width (us)"), (3, "D3 CM7 transfer width (us)"),
                (4, "D4 convert width (us)"), (2, "D2 RFFT width (us)")]:
    if c < nch:
        stats(name, [us(b - a) for a, b in highs(c)])

# Latency: each D0 falling edge -> next D3 rising edge
d0f = fall[0]
d3r = rise[3]
lat = []
j = 0
for t in d0f:
    while j < len(d3r) and d3r[j] < t:
        j += 1
    if j < len(d3r):
        lat.append(us(d3r[j] - t))
if lat:
    med = st.median(lat)
    stats("Latency D0 fall -> D3 rise (us)", [x for x in lat if x < 20 * max(med, 1)])

# CM7 work per block: D3 rise -> next D2 fall
work = []
d2f = fall[2]
j = 0
for t in d3r:
    while j < len(d2f) and d2f[j] < t:
        j += 1
    if j < len(d2f):
        work.append(us(d2f[j] - t))
stats("CM7 work D3 rise -> D2 fall (us)", work)

print(f"D0 pulses (blocks copied by CM4): {len(highs(0))}   D3 pulses (blocks taken by CM7): {len(highs(3))}")
