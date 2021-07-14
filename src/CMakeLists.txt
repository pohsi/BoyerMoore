set(TARGET_NAME BoyerMoore)


set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    

add_executable(${TARGET_NAME} ${TARGET_PLATFORM} ${SOURCES})


if((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    target_compile_options(${TARGET_NAME} PRIVATE "/Zc:__cplusplus")
endif()