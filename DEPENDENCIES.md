# Dependencies

## Build Requirements

- **C++ Compiler**: GCC 12+ or Clang 16+ with C++23 support
- **CMake**: 3.20 or higher
- **Ninja**: build backend used by the CMake presets
- **Linux**: Kernel 2.6.27+ (for epoll, eventfd)

## System Libraries

- **pthread**: POSIX threads
- **libstdc++**: Standard C++ library with C++23 support

## Runtime / Optional Dependencies

- **libpq** (PostgreSQL client library): required for the SQL layer.
- **liburing**: required for the io_uring presets.

## Optional Tools

- **clang-format**: Code formatting (version 14+)
- **clang-tidy**: Static analysis (version 14+)
- **Address Sanitizer**: Memory error detection
- **Thread Sanitizer**: Data race detection
- **UndefinedBehavior Sanitizer**: UB detection

## Test Harness

The test suite uses a small gtest-compatible header in `test/gtest/`. It is not the real
googletest, so no googletest package needs to be installed.

## Benchmark-Only Dependencies

The following are needed only to run the cross-framework comparison benchmarks, not to
build or use KATANA itself:

- **drogon** / **libdrogon-dev**
- **cargo** / **rustc**
- **wrk**

## Installation

### Debian/Ubuntu
```bash
sudo apt-get install build-essential cmake ninja-build g++-12 clang-16 clang-format clang-tidy libpq-dev liburing-dev
```

### Fedora/RHEL
```bash
sudo dnf install gcc-c++ cmake ninja-build clang clang-tools-extra libpq-devel liburing-devel
```

### Arch Linux
```bash
sudo pacman -S base-devel cmake ninja clang postgresql-libs liburing
```
