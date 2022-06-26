file(GLOB BASE_SRC *.cpp *.hpp)

add_library(Base ${BASE_SRC})

target_include_directories(Base PRIVATE
    ${CMAKE_SOURCE_DIR}/External/glew-cmake/include
    ${CMAKE_SOURCE_DIR}/External/glfw/include
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
    ${CMAKE_SOURCE_DIR}/External/glm
    ${CMAKE_SOURCE_DIR}/External/assimp/include
)

target_link_libraries(Base PRIVATE
    libglew_static
    glfw
    assimp
    ImGui
    OpenGL::GL
    Threads::Threads
)
