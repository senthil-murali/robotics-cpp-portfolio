# Sensor Data Processor

A C++ pipeline that filters, transforms, and scales simulated robot sensor readings using modern C++ features.

## Concepts Demonstrated

- **Lambda expressions** — Used to create filter and transform functions based on thresholds.
- **Lambda capture** — Used `[threshold]` to capture an external variable for use inside the filter lambda.
- **std::function** — Stored lambdas in a vector to apply a series of processing steps in sequence. This makes the pipeline easy to modify — add, remove, or reorder steps without reworking the core logic.
- **std::unique_ptr / std::make_unique** — `unique_ptr` is the smart pointer type, `make_unique` safely creates one without using `new` directly.
- **std::move** — Used to transfer ownership of a `unique_ptr` into a vector, since unique_ptrs cannot be copied.
- **auto / type deduction** — Automatic type deduction to keep code concise.
- **RAII** — `unique_ptr` automatically frees memory when it goes out of scope — no manual delete needed.

## Prerequisites

- CMake 3.16+, Ninja, g++ (C++17) — or just g++

## How to Build (CMake)
```bash
cmake --preset debug
cmake --build build/debug
```

## Quick Build (without CMake)
```bash
g++ -std=c++17 src/main.cpp -o sensor-data-processor
./sensor-data-processor
```

## What I Learned

Learned how `std::function` enables flexible processing pipelines, how `unique_ptr` and `make_unique` manage memory safely, and how `std::move` transfers ownership between containers.