cmake_minimum_required(VERSION 3.0)
project(AudioSwitch VERSION 0.2.0)

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# For systems where the Visual C++ Redistributable
# for Visual Studio 2015 is not installed
# (missing MSVCP120D.dll)
add_definitions("/MT")

add_executable(audioswitch WIN32 main.cpp controller.cpp controller.h policy_config.h)

install(TARGETS audioswitch DESTINATION bin)

set(CPACK_GENERATOR WIX)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_NAME "AudioSwitch")
set(CPACK_PACKAGE_VENDOR "Hans Kirchner")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "AudioSwitch")
set(CPACK_WIX_UPGRADE_GUID "30C45A60-C5D8-44d3-8E11-7394A401B813")
include(CPack)
