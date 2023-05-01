import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Polygon
from scipy.spatial import ConvexHull

from shapely.geometry import Point
from shapely.geometry.polygon import Polygon as plg

num_robots = 10
shape = 'cross'
name = shape + '_' + str(num_robots) + '_cy_data.dat'
# Read data from the input file
with open(name, 'r') as f:
    lines = f.readlines()

# Prepare data for plotting
timesteps = []

robot_list = []
for i in range(num_robots):
    robot_list.append('fb' + str(i))

robot_positions = {robot_id: [] for robot_id in robot_list}
average_positions = []
errors = []
inside_hull_ts = 0

for i, line in enumerate(lines):
    cur_polygon = []
    timesteps.append(i)
    tokens = line.split()
    tokens = tokens[1:]
    total_x, total_y = 0, 0

    for j in range(0, len(tokens), 2):
        robot_id = tokens[j]
        x, y, _ = map(float, tokens[j+1].split(','))
        robot_positions[robot_id].append((x, y))
        cur_polygon.append([x, y])
        total_x += x
        total_y += y

    # Check if CoM is within polygon
    form_polyg = plg(cur_polygon)
    if form_polyg.contains(Point(1.5, 0)) and inside_hull_ts == 0:
        inside_hull_ts = i

    # Calculate average position per time step
    avg_x, avg_y = total_x / len(robot_positions), total_y / len(robot_positions)
    average_positions.append((avg_x, avg_y))
    
    error = np.sqrt((avg_x - 1.5)**2 + (avg_y - 0)**2)
    errors.append(error)

# Get final positions of robots
final_positions = np.array([(pos[-1][0], pos[-1][1]) for pos in robot_positions.values()])

# Plot the positions
fig, ax = plt.subplots()

for robot_id, positions in robot_positions.items():
    x_positions = [pos[0] for pos in positions]
    y_positions = [pos[1] for pos in positions]
    ax.plot(x_positions, y_positions, label=robot_id)
    
    
    # Plot the final position as a circle
    final_x, final_y = x_positions[-1], y_positions[-1]
    ax.scatter(final_x, final_y, s=100, marker='o', edgecolors='k')

# Plot average positions
avg_x_positions = [pos[0] for pos in average_positions]
avg_y_positions = [pos[1] for pos in average_positions]
ax.plot(avg_x_positions, avg_y_positions, label='Average', color='pink', linestyle='--')
ax.scatter(avg_x_positions[-1], avg_y_positions[-1], s=150, marker='+', color='pink')
ax.scatter(1.5, 0, s=150, marker='+', color='black')

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_xlim([-2.5, 4.5])
ax.set_ylim([-3, 3])
ax.set_aspect('equal')
ax.legend()
ax.set_title('3 Agent Peanut Optimal Distribution')
plt.show()
# plt.savefig('3 Agent Peanut Optimal Distribution')

fig, ax = plt.subplots()
ax.plot(timesteps, errors, label='Error')
ax.scatter(inside_hull_ts, errors[inside_hull_ts], marker='*', s=200, color='red', label='Viable Solution')
ax.set_xlabel('Time Step')
ax.set_ylabel('Error')
ax.legend()
ax.set_title('10 Agent Box Optimal Distribution Error')
plt.show()

fig, ax = plt.subplots()
for robot_id, positions in robot_positions.items():
    # Plot the final position as a circle
    final_x, final_y = positions[-1]
    ax.scatter(final_x, final_y, s=100, marker='o', edgecolors='k', label=robot_id)

square = Rectangle((0.5, -1), 2, 2, linewidth=2, edgecolor='black', facecolor='none')
ax.add_patch(square)

# Calculate the convex hull
hull = ConvexHull(final_positions)
hull_polygon = Polygon(final_positions[hull.vertices], edgecolor='purple', facecolor='none')
ax.scatter(avg_x_positions[-1], avg_y_positions[-1], s=150, marker='+', color='purple')
ax.scatter(1.5, 0, s=150, marker='+', color='black')
ax.add_patch(hull_polygon)

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_xlim([-2.5, 4.5])
ax.set_ylim([-3, 3])
ax.set_aspect('equal')
ax.legend()
ax.set_title('10 Agent Box Optimal Distribution Polygon')
plt.show()