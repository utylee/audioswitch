cmake_minimum_required(VERSION 3.0)
project(AudioSwitch LANGUAGES CXX VERSION 0.5)

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# For systems where the Visual C++ Redistributable
# for Visual Studio 2015 is not installed
# (missing MSVCP120D.dll) -> link statically
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_executable(audioswitch WIN32 main.cpp controller.cpp controller.h policy_config.h resources.rc)

install(TARGETS audioswitch DESTINATION bin)

set(CPACK_GENERATOR WIX)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_NAME "AudioSwitch")
set(CPACK_PACKAGE_VENDOR "Hans Kirchner")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "AudioSwitch")
set(CPACK_WIX_UPGRADE_GUID "30C45A60-C5D8-44d3-8E11-7394A401B813")
set(CPACK_PACKAGE_EXECUTABLES "audioswitch" "AudioSwitch")
set(CPACK_CREATE_DESKTOP_LINKS "audioswitch")
include(CPack)
