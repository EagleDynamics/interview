set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/BallCollision/bin)

set(source_files
        src/Ball.cpp
        src/QuadTree.cpp)

add_executable(interview main.cpp ${source_files})

include_directories(lib)
target_link_libraries(interview -lsfml-graphics -lsfml-window -lsfml-system)