import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
from tqdm import tqdm

filename = './cluster45-22.log'

miss = []
hit = []
hit_rate = []

with open(filename) as file:
    for line in tqdm(file):
        try:
            if 'm ' in line:
                miss.append(int(line.split(' ')[1]))
            elif 'h ' in line:
                pass
                hit.append(int(line.split(' ')[1]))
            elif 'Hit-rate: ' in line:
                pass
                hit_rate.append(float(line.split(' ')[1]))
            else:
                print(line)
        except:
            print(line)

print("# of miss:", len(miss))
print("# of hit:", len(hit))

print("Save hit-rate plot...")
hit_rate = np.array(hit_rate)
zeros = np.zeros(len(hit_rate))
plt.figure(figsize=(12, 7))
plt.plot(hit_rate[hit_rate > zeros], '-^')
plt.xlabel("Windows (minute)")
plt.ylabel("Hit rate")
plt.title('Hit rate change by time (Cluster 3)')
plt.grid()
plt.savefig('cluster-45-hit-rate.png')
plt.close()

print("Save hit latency plot...")
hit = np.array(hit)
hit.sort()
x = np.arange(len(hit))
fig, ax = plt.subplots(figsize=(20, 10))
ax.plot(x, hit, label='hit', color='blue', lw=5)
ax.fill_between(x, 0, hit, alpha=.3, color='blue')
plt.xlabel('requests')
plt.ylabel('latency')
plt.legend()
plt.title('Sorted Request Latency (Hit)')
plt.grid()
plt.savefig('cluster-45-hit.png')
plt.close(fig)

print("Save miss latency plot...")
miss = np.array(miss)
miss.sort()
x = np.arange(len(miss))
fig, ax = plt.subplots(figsize=(20, 10))
ax.plot(x, miss, label='miss', color='orange', lw=5)
ax.fill_between(x, 0, miss, alpha=.3, color='orange')
plt.xlabel('requests')
plt.ylabel('latency')
plt.legend()
plt.title('Sorted Request Latency (Miss)')
plt.grid()
plt.savefig('cluster-45-miss.png')
plt.close(fig)

print("Save mean&max comparison plot...")
hit_latency = [np.mean(hit), np.max(hit)]
miss_latency = [np.mean(miss), np.max(miss)]
df = pd.DataFrame({'hit': hit_latency, 'miss': miss_latency}, index=['mean', 'max'])
df.plot.bar(rot=0, figsize=(8,8))

print("Save percentail comparison plot...")
percent = [10, 25, 50, 75, 99, 99.9]
df = pd.DataFrame({'hit': np.percentile(hit, percent), 'miss': np.percentile(miss, percent)}, index=percent)
df.plot.bar(rot=0, figsize=(20,10))
