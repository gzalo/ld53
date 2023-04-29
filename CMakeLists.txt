cmake_minimum_required(VERSION 3.17)
project(ld53)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

set(CMAKE_CXX_STANDARD 17)

add_executable(ld53 ld53.cpp graphics.cpp graphics.h Game.cpp Game.h collisions.cpp collisions.h)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USE_FLAGS "-s USE_SDL=2 -s LEGACY_GL_EMULATION")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    set_target_properties(ld53 PROPERTIES LINK_FLAGS "--embed-file res")
else()
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(ld53 SDL2::Main ${OPENGL_LIBRARY})
endif()
