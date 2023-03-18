import sys

file_name = sys.argv[1]

f = open(file_name, "r")
lines = f.readlines()

cells = 0
cycle = 0
reads = 0

for line in lines:
    if line[:5] == "cycle":
        cycle += int(line.split()[-1])
    elif line[:5] == "Cells":
        cells += 64 * int(line.split()[-1])
    reads += 1

area_scaling_factor_7nm = 7.8
time = cycle/(1024*1024*1024)
freq = 2
area = 5.4
cells_overhead = 3.72

print("throughput in {}GHz {:.3f} MCUPS/mm2".format(freq, cells*freq/time/(1024*1024)/area*area_scaling_factor_7nm/cells_overhead))
print("throughput in {}GHz {:.3f} Reads/mm2".format(freq, reads*freq/time/area*area_scaling_factor_7nm))
print("cycle", cycle)
print("cells", cells)
print("reads", reads)