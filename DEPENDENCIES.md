# Dependencies

## Build Requirements

- **C++ Compiler**: GCC 12+ or Clang 15+ with C++23 support
- **CMake**: 3.20 or higher
- **Linux**: Kernel 2.6.27+ (for epoll, eventfd)

## System Libraries

- **pthread**: POSIX threads
- **libstdc++**: Standard C++ library with C++23 support

## Optional Tools

- **clang-format**: Code formatting (version 14+)
- **clang-tidy**: Static analysis (version 14+)
- **Address Sanitizer**: Memory error detection
- **Thread Sanitizer**: Data race detection
- **UndefinedBehavior Sanitizer**: UB detection

## Installation

### Debian/Ubuntu
```bash
sudo apt-get install build-essential cmake g++-12 clang-15 clang-format clang-tidy
```

### Fedora/RHEL
```bash
sudo dnf install gcc-c++ cmake clang clang-tools-extra
```

### Arch Linux
```bash
sudo pacman -S base-devel cmake clang
```

## No External Dependencies

KATANA is designed to have **zero external dependencies** beyond the C++ standard library and Linux system calls. This ensures:
- Minimal build complexity
- Fast compilation times
- Easy deployment
- Maximum portability
