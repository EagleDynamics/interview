cmake_minimum_required(VERSION 3.24)
project(main)

set(CMAKE_CXX_STANDARD 17)

IF (WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_DIR ${PROJECT_SOURCE_DIR}/SFML-2.5.1/lib/cmake/SFML)
    find_package(SFML COMPONENTS system window graphics audio network REQUIRED)
    include_directories(${PROJECT_SOURCE_DIR}/SFML-2.5.1/include)
ENDIF()

add_subdirectory(BallCollision)