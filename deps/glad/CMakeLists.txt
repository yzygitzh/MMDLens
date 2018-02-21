ADD_LIBRARY(glad STATIC
    src/glad.c
    include/KHR/khrplatform.h
    include/glad/glad.h
)

TARGET_INCLUDE_DIRECTORIES(glad PUBLIC "include/")
