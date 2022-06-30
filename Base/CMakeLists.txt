file(GLOB RENDERIT_SRC *.cpp *.hpp)

add_library(RenderItLib ${RENDERIT_SRC})

target_include_directories(RenderItLib PRIVATE
    ${CMAKE_SOURCE_DIR}/External/glew-cmake/include
    ${CMAKE_SOURCE_DIR}/External/glfw/include
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
    ${CMAKE_SOURCE_DIR}/External/glm
    ${CMAKE_SOURCE_DIR}/External/assimp/include
    ${CMAKE_SOURCE_DIR}/External/termcolor/include
    ${CMAKE_SOURCE_DIR}/External/stb
)

target_link_libraries(RenderItLib PRIVATE
    libglew_static
    glfw
    assimp
    ImGui
    OpenGL::GL
    Threads::Threads
)
