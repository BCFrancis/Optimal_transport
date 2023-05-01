import matplotlib.pyplot as plt

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

for i, line in enumerate(lines):
    timesteps.append(i)
    tokens = line.split()
    tokens = tokens[1:]
    total_x, total_y = 0, 0

    for j in range(0, len(tokens), 2):
        robot_id = tokens[j]
        x, y, _ = map(float, tokens[j+1].split(','))
        robot_positions[robot_id].append((x, y))
        total_x += x
        total_y += y

    # Calculate average position per time step
    avg_x, avg_y = total_x / len(robot_positions), total_y / len(robot_positions)
    average_positions.append((avg_x, avg_y))

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
ax.scatter(avg_x_positions[-1], avg_y_positions[-1], s=150, marker='+', color='pink', edgecolors='k')
ax.scatter(1.5, 0, s=150, marker='+', color='black' , edgecolors='k')

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_xlim([-2.5, 4.5])
ax.set_ylim([-3, 3])
ax.set_aspect('equal')
ax.legend()
plot_title = str(num_robots) + " Agent " + str(shape) + " Optimal Distribution"
ax.set_title(plot_title)
#plt.show()
plt.savefig(plot_title + ".png")
