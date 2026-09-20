plt.figure(2, figsize=(12,7))
plt.plot(timestamp, pt1, label='pt1', color='blue')
plt.plot(timestamp, pt2, label='pt2', color='red')
plt.xlabel("Time", fontsize=12)
plt.ylabel("Magnitue", fontsize=12)

plt.grid(True, linestyle='--', alpha=0.6)