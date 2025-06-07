import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load CSV results
df = pd.read_csv('2105109.csv')  # Replace with your actual file

# Select graphs G1 to G10
target_graphs = [f'G{i}' for i in range(1, 11)]
df_subset = df[df['Name'].isin(target_graphs)]

# Extract values
x_labels = df_subset['Name'].tolist()
x = np.arange(len(x_labels))  # the label locations
width = 0.15  # the width of the bars

# Values for each algorithm
randomized = df_subset['Randomized-1']
greedy = df_subset['Greedy-1']
semi_greedy = df_subset[f'Semi-greedy-1 (α={0.7})']
local = df_subset['Local-1 Avg']
grasp = df_subset['GRASP-1 Best']

# Create the plot
fig, ax = plt.subplots(figsize=(12, 6))
ax.bar(x - 2*width, randomized, width, label='Randomized', color='deepskyblue')
ax.bar(x - width, greedy, width, label='Greedy', color='orangered')
ax.bar(x, semi_greedy, width, label='Semi-Greedy', color='lightgray')
ax.bar(x + width, grasp, width, label='GRASP', color='gold')
ax.bar(x + 2*width, local, width, label='Local Search', color='royalblue')

# Labels & Title
ax.set_xlabel('Graphs')
ax.set_ylabel('Cut Weight')
ax.set_title('Max Cut (Graph 1–10)')
ax.set_xticks(x)
ax.set_xticklabels(x_labels)
ax.legend()
plt.grid(axis='y', linestyle='--', alpha=0.6)

plt.tight_layout()
plt.savefig('maxcut_graphs_1_10.png')
plt.show()
