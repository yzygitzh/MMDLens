SET(PARSER_TEST_SRC_LIST parser.cpp parser_test.cpp)
SET(RENDER_TEST_SRC_LIST render_test.cpp glad.cpp)

SET(BOOST_LINK_OPT "-lboost_filesystem -lboost_system -lboost_program_options")
SET(CMAKE_EXE_LINKER_FLAGS "${BOOST_LINK_OPT}")

ADD_DEFINITIONS(-DMODEL_PARSER_DEBUG)

ADD_EXECUTABLE(mmd_parser_test ${PARSER_TEST_SRC_LIST})

ADD_EXECUTABLE(mmd_render_test ${RENDER_TEST_SRC_LIST})
TARGET_LINK_LIBRARIES(mmd_render_test glfw)
