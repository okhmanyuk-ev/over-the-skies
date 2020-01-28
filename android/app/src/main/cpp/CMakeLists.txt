cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -frtti -fexceptions -Wno-error=format-security")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

set(BASE_DIR "../../../../../")

file(GLOB MAIN_SRC
    ${BASE_DIR}src/*.h
    ${BASE_DIR}src/*.cpp)

file(GLOB CORE_SRC
    ${BASE_DIR}sky/src/Core/*.h
    ${BASE_DIR}sky/src/Core/*.cpp)

file(GLOB CONSOLE_SRC
    ${BASE_DIR}sky/src/Console/*.cpp
    ${BASE_DIR}sky/src/Console/*.h)

file(GLOB COMMON_SRC
    ${BASE_DIR}sky/src/Common/*.cpp
    ${BASE_DIR}sky/src/Common/*.h)

file(GLOB PLATFORM_SRC
    ${BASE_DIR}sky/src/Platform/*.cpp
    ${BASE_DIR}sky/src/Platform/*.h)

file(GLOB GRAPHICS_SRC
    ${BASE_DIR}sky/src/Graphics/*.cpp
    ${BASE_DIR}sky/src/Graphics/*.h)

file(GLOB RENDERER_SRC
    ${BASE_DIR}sky/src/Renderer/*.cpp
    ${BASE_DIR}sky/src/Renderer/*.h
    ${BASE_DIR}sky/src/Renderer/D3D11/*.cpp
    ${BASE_DIR}sky/src/Renderer/D3D11/*.h
    ${BASE_DIR}sky/src/Renderer/OpenGL/*.cpp
    ${BASE_DIR}sky/src/Renderer/OpenGL/*.h)

file(GLOB AUDIO_SRC
    ${BASE_DIR}sky/src/Audio/*.cpp
    ${BASE_DIR}sky/src/Audio/*.h)

file(GLOB SCENE_SRC
    ${BASE_DIR}sky/src/Scene/*.cpp
    ${BASE_DIR}sky/src/Scene/*.h)

file(GLOB SHARED_SRC
    ${BASE_DIR}sky/src/Shared/*.cpp
    ${BASE_DIR}sky/src/Shared/*.h)

file(GLOB NETWORK_SRC
    ${BASE_DIR}sky/src/Network/*.cpp
    ${BASE_DIR}sky/src/Network/*.h)

file(GLOB IMGUI_SRC
    ${BASE_DIR}sky/lib/imgui/*.cpp
    ${BASE_DIR}sky/lib/imgui/*.h)

file(GLOB TINYUTF8_SRC
    ${BASE_DIR}sky/lib/tinyutf8/*.cpp
    ${BASE_DIR}sky/lib/tinyutf8/*.h
    ${BASE_DIR}sky/lib/tinyutf8/*.hpp)

file(GLOB FMT_SRC
    ${BASE_DIR}sky/lib/fmt/src/*.cc)

include_directories(${BASE_DIR}sky/src)
include_directories(${BASE_DIR}sky/lib/imgui)
include_directories(${BASE_DIR}sky/lib/glm)
include_directories(${BASE_DIR}sky/lib/stb)
include_directories(${BASE_DIR}sky/lib/fmod/include)
include_directories(${BASE_DIR}sky/lib/rectpack2D/include)
include_directories(${BASE_DIR}sky/lib/nlohmann_json/include)
include_directories(${BASE_DIR}sky/lib/tinyutf8)
include_directories(${BASE_DIR}sky/lib/fmt/include)

add_library(main SHARED
    ${MAIN_SRC}
    ${CORE_SRC}
    ${CONSOLE_SRC}
    ${COMMON_SRC}
    ${PLATFORM_SRC}
    ${GRAPHICS_SRC}
    ${RENDERER_SRC}
    ${AUDIO_SRC}
    ${SCENE_SRC}
    ${NETWORK_SRC}
    ${SHARED_SRC}
    ${IMGUI_SRC}
    ${TINYUTF8_SRC}
    ${FMT_SRC}
)

target_compile_definitions(main PRIVATE -DGLM_ENABLE_EXPERIMENTAL)

add_library(fmod SHARED IMPORTED)

set_target_properties(fmod PROPERTIES IMPORTED_LOCATION
    ../../../../../../sky/lib/fmod/android/${ANDROID_ABI}/libfmod.so)

add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ../../../../../../sky/lib/fmod/android/${ANDROID_ABI}/libfmod.so
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfmod.so)

target_link_libraries(main
    android
    native_app_glue
    EGL
    fmod
    GLESv3
    log
    m
)