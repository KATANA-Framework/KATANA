from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class KatanaConan(ConanFile):
    name = "katana"
    version = "0.1.0"
    license = "MIT"
    author = "VisageDvachevsky"
    url = "https://github.com/KATANA-Framework/KATANA"
    description = (
        "High-performance async C++ framework with reactor-per-core architecture "
        "(includes codegen compute/validation examples)"
    )
    topics = ("async", "reactor", "networking", "http", "performance")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports_sources = "CMakeLists.txt", "katana/*", "examples/*", "test/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["katana_core"]
        self.cpp_info.system_libs = ["pthread"]
