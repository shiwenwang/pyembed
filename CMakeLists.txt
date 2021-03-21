cmake_minimum_required(VERSION 3.0.0)
project(EmbedPython VERSION 0.1.0)
set(PYTHON_MAJOR 3)
set(PYTHON_MINOR 9)

add_library(${PROJECT_NAME} 
    STATIC
        src/pyembed.cpp)

# need environment variable PYTHHOME
# if ($ENV{PYTHONHOME})
target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
        $ENV{PYTHONHOME}/include
)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        $ENV{PYTHONHOME}/libs/python${PYTHON_MAJOR}${PYTHON_MINOR}.lib
)   
# endif()        

add_executable(example 
    src/example.cpp
)

target_link_libraries(example
    PRIVATE
        ${PROJECT_NAME}
)


