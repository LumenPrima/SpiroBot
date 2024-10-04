cmake_minimum_required(VERSION 3.5)

project(SpiroBot LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Qt packages
find_package(Qt6 COMPONENTS Widgets Core Gui Svg REQUIRED)
message(STATUS "Qt version: ${Qt6_VERSION}")
get_target_property(QtCore_INCLUDE_DIRS Qt6::Core INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "Qt6 Core include dirs: ${QtCore_INCLUDE_DIRS}")

# Add your source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/drawingarea.cpp
    src/gcodegenerator.cpp
    src/gcodeexportdialog.cpp
    include/mainwindow.h
    include/drawingarea.h
    include/gcodegenerator.h
    include/gcodeexportdialog.h
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link against Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Svg)