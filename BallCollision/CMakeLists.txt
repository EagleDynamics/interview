set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/BallCollision/bin)

set(source_files
        src/ball.cpp
        src/quad_tree.cpp
        src/vector_operations.cpp
        src/simulation.cpp
        )

add_executable(main main.cpp ${source_files})

include_directories(lib)
IF (WIN32)
    target_link_libraries(main sfml-graphics sfml-window sfml-system)
ELSE()
    target_link_libraries(main -lsfml-graphics -lsfml-window -lsfml-system)
ENDIF()
