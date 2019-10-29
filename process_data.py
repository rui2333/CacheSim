import csv
import pandas as pd
import matplotlib.pyplot as plt
data_set_raw = pd.read_csv('file.txt', sep = ", ", names = ['xor_runningzeroes_raw', 'xor_compress', 'orig_runningzeroes', 'orig_compress'])
data_set = pd.read_csv('file.txt', sep = ", ", names = ['xor_runningzeroes', 'xor_compress', 'orig_runningzeroes', 'orig_compress'])
#print(data_set.sample(50, random_state=0).head())
#data_set.plot.line(title = 'compress visulization')
#columns = data_set.columns.drop(['name'])
#data_set = data_set.sample(500, random_state=0)
rows = [data_set.sample(frac =.01), data_set_raw.sample(frac = .01)] 
columns = rows.columns
x_data = range(0, rows.shape[0])
fig, ax = plt.subplots()
for column in columns:
    ax.plot(x_data, rows[column])
ax.set_title('compress visualization')
ax.legend()
plt.show()
plt.savefig('result.png')
#plt.close()
