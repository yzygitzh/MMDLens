SET(PARSER_TEST_SRC_LIST parser.cpp parser_test.cpp)
SET(RENDER_TEST_SRC_LIST render_test.cpp parser.cpp renderer.cpp controller.cpp)

SET(BOOST_LINK_OPT "-lboost_filesystem -lboost_system -lboost_program_options")
SET(FIP_LINK_OPT "-lfreeimageplus")
SET(CMAKE_EXE_LINKER_FLAGS "${BOOST_LINK_OPT} ${FIP_LINK_OPT}")

# ADD_DEFINITIONS(-DMODEL_PARSER_DEBUG)

ADD_EXECUTABLE(mmd_parser_test ${PARSER_TEST_SRC_LIST})

ADD_EXECUTABLE(mmd_render_test ${RENDER_TEST_SRC_LIST})
TARGET_LINK_LIBRARIES(mmd_render_test glad glfw)

ADD_CUSTOM_COMMAND(
    OUTPUT COPY_GLSL
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/*.glsl" "${CMAKE_CURRENT_BINARY_DIR}/"
)
ADD_CUSTOM_TARGET(COPY_GLSL_TASK ALL DEPENDS COPY_GLSL)
