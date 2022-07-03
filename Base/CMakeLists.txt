file(GLOB_RECURSE RENDERIT_SRC *.cpp *.hpp)

add_library(RenderItLib STATIC ${RENDERIT_SRC})

target_include_directories(RenderItLib PRIVATE
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

target_link_libraries(RenderItLib PRIVATE
    libglew_static
    glfw
    assimp
    ImGui
    OpenGL::GL
    Threads::Threads
)

if(WIN32)
    target_link_libraries(RenderItLib PRIVATE
        comdlg32
    )
endif()
