file(GLOB UTIL_SRC
    ../src/cpp/utils.hpp
    ../src/cpp/utils.cpp
    )

# Simple macro to reduce repetitive code
macro(add_unit_test TEST_FILE)
    get_filename_component(TEST_NAME_WITH_EXTENSION ${TEST_FILE} NAME)
    string(FIND ${TEST_NAME_WITH_EXTENSION} "." EXTENSION_BEGIN)
    string(SUBSTRING ${TEST_NAME_WITH_EXTENSION} 0 ${EXTENSION_BEGIN} TEST_NAME)

    add_executable(${TEST_NAME} ${UTIL_SRC} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} ncurses)

    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endmacro()

# ctest test items
add_unit_test(testUtils.cpp)