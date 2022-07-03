cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(RenderIt)

option(RENDERIT_EXAMPLES "Build the Render examples" ON)

add_definitions(-DUNICODE)
add_definitions(-DGLEW_STATIC)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

option(BUILD_SHARED_LIBS "Build package with shared libraries." OFF)

option(glew-cmake_BUILD_SHARED "Build the shared glew library" OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/glew-cmake EXCLUDE_FROM_ALL)

option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw EXCLUDE_FROM_ALL)

option(ASSIMP_BUILD_ASSIMP_TOOLS
    "If the supplementary tools for Assimp are built in addition to the library."
    OFF
)
option(ASSIMP_BUILD_TESTS
    "If the test suite for Assimp is built in addition to the library."
    OFF
)
option(ASSIMP_INSTALL
    "Disable this if you want to use assimp as a submodule."
    OFF
)
OPTION(ASSIMP_BUILD_ZLIB
    "Build your own zlib"
    ON
)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/assimp EXCLUDE_FROM_ALL)

file(GLOB IMGUI_SRC
    ${CMAKE_SOURCE_DIR}/External/imgui/*.cpp
    ${CMAKE_SOURCE_DIR}/External/imgui/*.h
    ${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.h
    ${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_glfw.h
    ${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_glfw.cpp
)
add_library(ImGui ${IMGUI_SRC})
target_include_directories(ImGui PRIVATE
    ${CMAKE_SOURCE_DIR}/External/glew-cmake/include
    ${CMAKE_SOURCE_DIR}/External/glfw/include
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
)

add_subdirectory(Base)

set(RENDERIT_HEADERS
    ${CMAKE_SOURCE_DIR}/External/glew-cmake/include
    ${CMAKE_SOURCE_DIR}/External/glfw/include
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
    ${CMAKE_SOURCE_DIR}/External/glm
    ${CMAKE_SOURCE_DIR}/External/assimp/include
    ${CMAKE_SOURCE_DIR}/External/termcolor/include
    ${CMAKE_SOURCE_DIR}/External/tinygltf
    ${CMAKE_SOURCE_DIR}/Base
)

set(RENDERIT_LIBS
    libglew_static
    glfw
    assimp
    ImGui
    OpenGL::GL
    Threads::Threads
    RenderItLib
)

if(RENDERIT_EXAMPLES)
add_subdirectory(Examples)
endif(RENDERIT_EXAMPLES)
