SET(PARSER_TEST_SRC_LIST parser.cpp parser_test.cpp)
SET(RENDER_TEST_SRC_LIST render_test.cpp glad.cpp parser.cpp renderer.cpp)

SET(BOOST_LINK_OPT "-lboost_filesystem -lboost_system -lboost_program_options")
SET(FIP_LINK_OPT "-lfreeimageplus")
SET(CMAKE_EXE_LINKER_FLAGS "${BOOST_LINK_OPT} ${FIP_LINK_OPT}")

# ADD_DEFINITIONS(-DMODEL_PARSER_DEBUG)

ADD_EXECUTABLE(mmd_parser_test ${PARSER_TEST_SRC_LIST})

ADD_EXECUTABLE(mmd_render_test ${RENDER_TEST_SRC_LIST})
TARGET_LINK_LIBRARIES(mmd_render_test glfw)

ADD_CUSTOM_COMMAND(
    TARGET mmd_render_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/*.glsl"
    $<TARGET_FILE_DIR:mmd_render_test>)
