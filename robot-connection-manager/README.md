# Robot Connection Manager

A C++ class system that simulates managing connections to multiple factory robots, tracking connection status, faults, running programs, and heartbeat timestamps.

## Concepts Demonstrated

- **const members** — `name_` and `ip_address_` are const because a robot's identity never changes after creation. The compiler enforces this.
- **const methods** — `info()` and getters are marked const, telling the compiler they never modify class members. Required for use with const references.
- **const reference returns** — Getters return `const std::string&` to avoid copying and prevent modification of internal state.
- **const/non-const overloads** — `find_robot()` has two versions: const returns `const Robot*` for read-only access, non-const returns `Robot*` for mutable access. Compiler selects automatically.
- **Raw pointer non-owning access** — `find_robot()` returns `Robot*` instead of transferring the unique_ptr, allowing callers to use the robot without taking ownership. Returns nullptr if not found.
- **Erase-remove idiom** — `remove_if` moves keepers to the front, `erase` chops the leftover from the returned iterator to end.
- **std::find_if** — Searches linearly with a lambda predicate, returns iterator to first match or `end()` if not found. Clearer intent than a raw for loop.
- **std::chrono** — `steady_clock::time_point` tracks last heartbeat timestamp. `duration_cast` calculates seconds since last contact.
- **std::unique_ptr / RAII** — ConnectionManager owns all robots via unique_ptr. When the manager is destroyed, all robots are automatically cleaned up.
- **Lambda capture by reference** — `[&name]` captures search parameter by reference for use in STL algorithm predicates.

## Prerequisites

- CMake 3.16+, Ninja, g++ (C++17) — or just g++

## How to Build (CMake)
```bash
cmake --preset debug
cmake --build build/debug
```

## Quick Build (without CMake)
```bash
g++ -std=c++17 src/main.cpp -o robot-connection-manager
./robot-connection-manager
```

## What I Learned

Learned how const correctness protects class invariants at compile time, how raw pointers provide non-owning access to unique_ptr-managed objects, and how STL algorithms like find_if and remove_if express intent more clearly than manual loops.