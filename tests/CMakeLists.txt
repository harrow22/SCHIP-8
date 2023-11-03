include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(${PROJECT_NAME}_test TestDriver.cpp
        ../src/keyboard/Keyboard.cpp
        ../src/memory/Memory.cpp
        ../src/memory/Memory.test.cpp
        ../src/display/Display.cpp
        ../src/interpreter/Interpreter.cpp
        ../src/interpreter/Interpreter.test.cpp
)

target_link_libraries(${PROJECT_NAME}_test
        PRIVATE ${SDL2_LIBRARIES}
        PRIVATE GTest::gtest_main)
target_include_directories(${PROJECT_NAME}_test
        PRIVATE ${SDL2_INCLUDE_DIR})

if (NOT SDL2_FETCHED)
    add_custom_command(TARGET ${PROJECT_NAME}_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/lib $<TARGET_FILE_DIR:${PROJECT_NAME}_test>)
endif()

enable_testing()
gtest_discover_tests(${PROJECT_NAME}_test)