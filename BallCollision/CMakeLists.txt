set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/BallCollision/bin)

set(source_files
        src/ball.cpp
        src/quad_tree.cpp
        src/vector_operations.cpp)

add_executable(main main.cpp ${source_files})
add_executable(test test.cpp ${source_files})
add_executable(test_two_balls test_two_balls.cpp ${source_files})

include_directories(lib)
target_link_libraries(main -lsfml-graphics -lsfml-window -lsfml-system)
target_link_libraries(test -lsfml-graphics -lsfml-window -lsfml-system)
target_link_libraries(test_two_balls -lsfml-graphics -lsfml-window -lsfml-system)
