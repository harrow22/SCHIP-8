add_executable(${PROJECT_NAME} Emulator.cpp
        interpreter/Interpreter.cpp
        interpreter/Interpreter.h
        memory/Memory.cpp
        memory/Memory.h
        display/Display.cpp
        display/Display.h
        keyboard/Keyboard.cpp
        keyboard/Keyboard.h)
target_link_libraries(${PROJECT_NAME}
        PRIVATE ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME}
        PRIVATE ${SDL2_INCLUDE_DIR})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/lib $<TARGET_FILE_DIR:${PROJECT_NAME}>)