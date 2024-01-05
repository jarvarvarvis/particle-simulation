cmake_minimum_required(VERSION 3.28.1+)

project(particle-simulation C)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

set(INCLUDE_DIRS ${INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS})
include_directories(${INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set(LIBS ${LIBS}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        m glfw
        pthread)
target_link_libraries(${PROJECT_NAME} ${LIBS})
