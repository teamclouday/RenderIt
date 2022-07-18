# one example
function(buildExample EXAMPLE_NAME)
    set(EXAMPLE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME})
    message(STATUS "Generating project file for ${EXAMPLE_NAME}")
    file(GLOB_RECURSE SRC ${EXAMPLE_FOLDER}/src/*.cpp ${EXAMPLE_FOLDER}/src/*.hpp)
    add_executable(${EXAMPLE_NAME} ${SRC})
    set_target_properties(${EXAMPLE_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_FOLDER}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXAMPLE_FOLDER}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXAMPLE_FOLDER}/bin
    )
    target_include_directories(${EXAMPLE_NAME} PRIVATE
        ${RENDERIT_HEADERS}
        ${EXAMPLE_FOLDER}/src
    )
    target_link_libraries(${EXAMPLE_NAME} PRIVATE
        ${RENDERIT_LIBS}
    )
    if(EXISTS ${EXAMPLE_FOLDER}/shaders)
        add_custom_target(${EXAMPLE_NAME}_copy_shaders
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXAMPLE_FOLDER}/shaders ${EXAMPLE_FOLDER}/bin/shaders
        )
        add_dependencies(${EXAMPLE_NAME} ${EXAMPLE_NAME}_copy_shaders)
    endif()
    if(EXISTS ${EXAMPLE_FOLDER}/assets)
        add_custom_target(${EXAMPLE_NAME}_copy_assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXAMPLE_FOLDER}/assets ${EXAMPLE_FOLDER}/bin/assets
        )
        add_dependencies(${EXAMPLE_NAME} ${EXAMPLE_NAME}_copy_assets)
    endif()
endfunction(buildExample)

# all example
function(buildExamples)
    foreach(EXAMPLE ${EXAMPLES})
        buildExample(${EXAMPLE})
    endforeach(EXAMPLE)
endfunction(buildExamples)

set(EXAMPLES
    HelloWorld
    SimpleModel
    Animation
    Lights
    Mandelbulb
    SimpleShapes
    ParallaxMapping
    MSAA
    Transparency
    PBR
    ToneMapping
    Shadow
)

buildExamples()
