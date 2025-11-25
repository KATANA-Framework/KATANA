# CMake generated Testfile for
# Source directory: /workspace/test
# Build directory: /workspace/.docker_build/profile/test
#
# This file includes the relevant testing commands required for
# testing this directory and lists subdirectories to be tested as well.
add_test([=[unit_tests]=] "/workspace/.docker_build/profile/test/unit_tests")
set_tests_properties([=[unit_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "/workspace/test/CMakeLists.txt;43;add_test;/workspace/test/CMakeLists.txt;0;")
add_test([=[integration_tests]=] "/workspace/.docker_build/profile/test/integration_tests")
set_tests_properties([=[integration_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "/workspace/test/CMakeLists.txt;44;add_test;/workspace/test/CMakeLists.txt;0;")
