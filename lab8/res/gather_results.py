from glob import glob
import re
from pathlib import Path
import pandas as pd
import numpy as np


files = glob("/home/jakub/Documents/HPC/labs/lab8/res/*.err")

res = {}

for filename in files:
    with open(filename) as file:
        path = Path(filename).name
        for l in file:
            num = re.findall("\d+[.]\d+(?:e-\d+)?", l)
            if len(num) > 0:
                ds = re.findall("\d+", path)
                if len(ds) == 1:
                    num_nodes = 0
                    g_size = int(ds[0])
                else:
                    num_nodes = int(ds[0])
                    g_size = int(ds[1])
                time = float(num[0])
                # print(f"{num_nodes} {g_size} {time}")
                res[(num_nodes, g_size)] = time
                break


speedups = {}

graph_sizes = [16,128,512, 1024]


num_nodes = 1
while num_nodes <= 16:
    for s in graph_sizes:
        speedups[(num_nodes, s)] = res[(0, s)] / res[(num_nodes, s)]

df = pd.DataFrame([(k1, k2, v) for (k1, k2), v in res.items()])
print(df)


ax = plt.axes(projection='3d')
ax.plot_trisurf(x, y, z,
                cmap='viridis', edgecolor='none');
