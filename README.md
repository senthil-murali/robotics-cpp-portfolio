# Robotics C++ Portfolio

Hi, I'm Senthilkumar Murali — a robotics engineer with 3+ years programming FANUC industrial robots. This repo is where I bridge that hands-on experience with modern C++ and ROS2 software engineering.

Each project builds toward production-level robotics development, with new ones added regularly.

## Projects

| Project | Key Concepts | Description |
|---------|-------------|-------------|
| [sensor-data-processor](./sensor-data-processor) | `unique_ptr`, `std::move`, lambdas, `std::function`, RAII | Sensor data pipeline with filtering, transforming, and ownership transfer |
| [robot-connection-manager](./robot-connection-manager) | `const` correctness, raw pointer access, `find_if`, `chrono`, erase-remove idiom | Factory robot connection manager with ownership, search, and status tracking |
| [obstacle_detector](./obstacle_detector) | Lifecycle node, component node, `LifecyclePublisher`, `shared_ptr`, QoS, ROS2 parameters | ROS2 lifecycle component node — detects obstacles from LaserScan and publishes alerts |
| [waypoint_follower](./waypoint_follower) | Action client, templates, `std::vector`, `std::bind`, type aliases, quaternions, header/source split | Nav2 action client — navigates TurtleBot3 through sequential waypoints in Gazebo |
## Tech Stack

- **Languages:** C++17
- **Build Systems:** CMake, CMake Presets, Ninja, colcon (ament_cmake)
- **Tooling:** clangd, VS Code, CodeLLDB, Git
- **ROS2:** Humble (Ubuntu 22.04)
- **Hardware:** Jetson Orin Nano Super
- **OS:** Ubuntu 22.04

## Currently Learning

Modern C++ patterns, ROS2 component architecture, and NVIDIA simulation tools. New projects added regularly.