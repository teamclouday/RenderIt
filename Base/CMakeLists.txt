file(GLOB_RECURSE RENDERIT_SRC *.cpp *.hpp)

# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
# endif()

add_library(RenderItLib STATIC ${RENDERIT_SRC})

target_include_directories(RenderItLib PRIVATE
    ${CMAKE_SOURCE_DIR}/External/glew-cmake/include
    ${CMAKE_SOURCE_DIR}/External/glfw/include
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
    ${CMAKE_SOURCE_DIR}/External/glm
    ${CMAKE_SOURCE_DIR}/External/assimp/include
    ${CMAKE_SOURCE_DIR}/External/termcolor/include
    ${CMAKE_SOURCE_DIR}/External/stb
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
    OptickCore
)
target_link_directories(RenderItLib PRIVATE
    ${CMAKE_SOURCE_DIR}/External/optick/src
)
endif()
