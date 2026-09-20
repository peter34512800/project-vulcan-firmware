import matplotlib.pyplot as plt
import numpy as np

timestamp, servo, scale, thrust, pt1, pt2 = np.loadtxt('data14.txt', delimiter=',', unpack=True)

# individual windows
# window 1
plt.figure(1, figsize=(12,7))
plt.plot(timestamp, thrust, label='servo', color='blue')
plt.plot(timestamp, scale, label='scale', color='orange')

plt.xlabel("Time", fontsize=12)
plt.ylabel("Magnitue", fontsize=12)

plt.grid(True, linestyle='--', alpha=0.6)

# window 2
plt.figure(2, figsize=(12,7))
plt.plot(timestamp, pt1, label='pt1', color='blue')
plt.plot(timestamp, pt2, label='pt2', color='red')
plt.xlabel("Time", fontsize=12)
plt.ylabel("Magnitue", fontsize=12)

plt.grid(True, linestyle='--', alpha=0.6)

# plot with large range(left) and small range(right)
# setup figure and primary left Y-axis
fig, ax1 = plt.subplots(figsize=(10,6))

# large range data
color_scale = 'tab:blue'
ax1.set_xlabel('Time', fontsize=12)
ax1.set_ylabel('Magnitue', color=color_scale, fontsize=12)
line1 = ax1.plot(timestamp, scale, color=color_scale, label='scale')
ax1.tick_params(axis='y', labelcolor=color_scale)

# create secondary Y-axis
ax2 = ax1.twinx()

# small range data
color_servo = 'tab:red'
ax2.set_ylabel('Magnitue', color=color_servo, fontsize=12)
line2 = ax2.plot(timestamp, servo, color=color_servo, label='servo angle')
ax2.tick_params(axis='y', labelcolor=color_servo)

# merge legends
lines = line1 + line2
labels = [l.get_label() for l in lines]
ax1.legend(lines, labels, loc='upper left')

ax1.grid(True, linestyle='--', alpha=0.5)

plt.legend(loc='best')
plt.show()