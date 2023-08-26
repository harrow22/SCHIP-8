# glad github repo: https://github.com/Dav1dde/glad.git
# using v2.0.4 and gl 4.6
add_library(
        glad
        src/glad.c
        include/glad/glad.h
        include/KHR/khrplatform.h
)
target_include_directories(glad PUBLIC ${glad_SOURCE_DIR}/include)