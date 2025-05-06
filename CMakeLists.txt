cmake_minimum_required(VERSION 3.5)

project(PaintApp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB SOURCES 
    "src/*.cpp" 
    "src/shapes/*.cpp"
)

file(GLOB HEADERS 
    "include/*.h" 
    "include/shapes/*.h"
)

set(RESOURCES 
    "../resources/resources.qrc"
)

add_executable(PaintApp ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(PaintApp PRIVATE include)

target_link_libraries(PaintApp Qt6::Widgets)