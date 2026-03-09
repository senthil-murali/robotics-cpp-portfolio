# Robotics C++ Portfolio

Hi, I'm Senthilkumar Murali. I'm a robotics engineer with 3+ years of experience with industrial robotics systems and hands-on experience programming FANUC industrial robots.

This repository is a collection of projects in Modern C++ and ROS2, built to bridge my industrial robotics background with software engineering.

Currently focused on modern C++ and ROS2, with each project building toward production-level robotics development.

## Projects

| Project | Key Concepts | Description |
|---------|-------------|-------------|
| [sensor-data-processor](./sensor-data-processor) | `unique_ptr`, `std::move`, lambdas, `std::function`, RAII | Sensor data pipeline with filtering, transforming, and ownership transfer |
| [robot-connection-manager](./robot-connection-manager) | `const` correctness, raw pointer access, `find_if`, `chrono`, erase-remove idiom | Factory robot connection manager with ownership, search, and status tracking |
| [obstacle_detector](./obstacle_detector) | Lifecycle node, component node, `LifecyclePublisher`, `shared_ptr`, QoS, ROS2 parameters | ROS2 lifecycle component node — detects obstacles from LaserScan and publishes alerts |

## Tech Stack

- **Languages:** C++17
- **Build Systems:** CMake, CMake Presets, Ninja, colcon (ament_cmake)
- **Tooling:** clangd, VS Code, CodeLLDB, Git
- **ROS2:** Humble (Ubuntu 22.04)
- **Hardware:** Jetson Orin Nano Super
- **OS:** Ubuntu 22.04

## Currently Learning

Modern C++ patterns, ROS2 component architecture, and NVIDIA simulation tools. New projects added regularly.