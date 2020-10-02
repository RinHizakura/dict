import seaborn as sns
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats

sns.set()
df = pd.read_csv('out.csv',header=0)
print(df)

z_scores = stats.zscore(df["time(ns)"])
abs_z_scores = np.abs(z_scores)
filtered_entries = (abs_z_scores < 2)
df = df[filtered_entries]


# Grouped boxplot
sns.boxplot(x="ctz", y="time(ns)", hue="label", data=df, palette="Set1")
plt.show()

