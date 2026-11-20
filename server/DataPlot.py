import matplotlib.pyplot as plt
import numpy as np

timestamp, servo, scale, thrust, pt1, pt2 = np.loadtxt(
    'data19.txt', delimiter=',', unpack=True
)

# Individual windows

# Window 1
plt.figure(1, figsize=(12, 7))
plt.plot(timestamp, thrust, label='Thrust', color='blue')
plt.plot(timestamp, scale, label='Scale', color='orange')

plt.xlabel("Time", fontsize=12)
plt.ylabel("Magnitude", fontsize=12)
plt.legend(loc='best')
plt.grid(True, linestyle='--', alpha=0.6)

# Window 2
plt.figure(2, figsize=(12, 7))
plt.plot(timestamp, pt1, label='Pressure Transducer 1', color='blue')
plt.plot(timestamp, pt2, label='Pressure Transducer 2', color='red')

plt.xlabel("Time", fontsize=12)
plt.ylabel("Magnitude", fontsize=12)
plt.legend(loc='best')
plt.grid(True, linestyle='--', alpha=0.6)

# Plot with large range (left) and small range (right)

# Setup figure and primary left Y-axis
fig, ax1 = plt.subplots(figsize=(10, 6))

# Large range data
color_scale = 'tab:blue'
ax1.set_xlabel('Time', fontsize=12)
ax1.set_ylabel('Magnitude', color=color_scale, fontsize=12)
line1 = ax1.plot(timestamp, scale, color=color_scale, label='Scale')
ax1.tick_params(axis='y', labelcolor=color_scale)

# Create secondary Y-axis
ax2 = ax1.twinx()

# Small range data
color_servo = 'tab:red'
ax2.set_ylabel('Servo Angle', color=color_servo, fontsize=12)
line2 = ax2.plot(timestamp, servo, color=color_servo, label='Servo Angle')
ax2.tick_params(axis='y', labelcolor=color_servo)

# Merge legends
lines = line1 + line2
labels = [line.get_label() for line in lines]
ax1.legend(lines, labels, loc='upper left')

ax1.grid(True, linestyle='--', alpha=0.5)

plt.show()