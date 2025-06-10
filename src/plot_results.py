import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load data
df = pd.read_csv('timings.csv')

# Calculate mean and standard deviation
stats = df.groupby('n').agg({
    'jarvis_time': ['mean', 'std'],
    'graham_time': ['mean', 'std'],
    'chan_time': ['mean', 'std']
}).reset_index()

# Flatten column names
stats.columns = ['n', 
                'jarvis_mean', 'jarvis_std',
                'graham_mean', 'graham_std',
                'chan_mean', 'chan_std']

# Create plot
plt.figure(figsize=(10, 6))

# Plot with error bars
plt.errorbar(stats['n'], stats['jarvis_mean'], yerr=stats['jarvis_std'], 
             fmt='o-', capsize=5, label='Jarvis')
plt.errorbar(stats['n'], stats['graham_mean'], yerr=stats['graham_std'], 
             fmt='s-', capsize=5, label='Graham')
plt.errorbar(stats['n'], stats['chan_mean'], yerr=stats['chan_std'], 
             fmt='^-', capsize=5, label='Chan')

# Formatting
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Number of Points (log scale)')
plt.ylabel('Execution Time (μs, log scale)')
plt.title('Convex Hull Algorithm Performance')
plt.legend()
plt.grid(True, which="both", ls="--")
plt.savefig('performance_comparison.png', dpi=300)
plt.show()

# Additional plot for large n
plt.figure(figsize=(10, 6))
large_n = stats[stats['n'] > 1000]  # Focus on larger datasets
for algo in ['jarvis', 'graham', 'chan']:
    plt.plot(large_n['n'], large_n[f'{algo}_mean'], 'o-', 
             label=algo.capitalize())
    
plt.xlabel('Number of Points')
plt.ylabel('Time (μs)')
plt.title('Performance on Large Datasets')
plt.legend()
plt.grid(True)
plt.savefig('large_n_performance.png', dpi=300)
plt.show()
