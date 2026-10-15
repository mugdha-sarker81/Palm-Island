cmake_minimum_required(VERSION 3.16)

project(PalmIsland VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

set(RAYLIB_VERSION "5.5")
set(RAYLIB_ARCHIVE "${CMAKE_BINARY_DIR}/_deps/raylib-${RAYLIB_VERSION}.zip")
set(RAYLIB_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/raylib-${RAYLIB_VERSION}")

if(NOT EXISTS "${RAYLIB_SOURCE_DIR}/CMakeLists.txt")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/_deps")

    if(NOT EXISTS "${RAYLIB_ARCHIVE}")
        set(RAYLIB_URL "https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.zip")

        if(WIN32)
            string(REPLACE "'" "''" RAYLIB_ARCHIVE_PS "${RAYLIB_ARCHIVE}")
            execute_process(
                COMMAND powershell.exe -NoProfile -ExecutionPolicy Bypass -Command
                    "$ProgressPreference='SilentlyContinue'; Invoke-WebRequest -Uri '${RAYLIB_URL}' -OutFile '${RAYLIB_ARCHIVE_PS}'"
                RESULT_VARIABLE RAYLIB_DOWNLOAD_RESULT
            )
        else()
            file(DOWNLOAD "${RAYLIB_URL}" "${RAYLIB_ARCHIVE}" STATUS RAYLIB_DOWNLOAD_STATUS)
            list(GET RAYLIB_DOWNLOAD_STATUS 0 RAYLIB_DOWNLOAD_RESULT)
        endif()

        if(NOT RAYLIB_DOWNLOAD_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to download Raylib ${RAYLIB_VERSION}.")
        endif()
    endif()

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar xf "${RAYLIB_ARCHIVE}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/_deps"
        RESULT_VARIABLE RAYLIB_EXTRACT_RESULT
    )
    if(NOT RAYLIB_EXTRACT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to extract Raylib ${RAYLIB_VERSION}.")
    endif()
endif()

add_subdirectory("${RAYLIB_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/raylib-build")

add_executable(PalmIsland
    src/main.c
    src/input.c
    src/interaction.c
    src/player.c
    src/room.c
    src/camera.c
    src/render.c
    src/thornback.c
)
if(NOT MSVC)
    target_compile_options(PalmIsland PRIVATE -Wall -Wextra)
endif()
target_link_libraries(PalmIsland PRIVATE raylib)

option(PALM_BUILD_TESTS "Build focused Palm Island gameplay tests" OFF)
if(PALM_BUILD_TESTS)
    enable_testing()
    add_executable(PalmIslandMovementTests
        tests/movement_collision_test.c
        src/interaction.c
        src/player.c
        src/room.c
    )
    target_include_directories(PalmIslandMovementTests PRIVATE src)
    target_link_libraries(PalmIslandMovementTests PRIVATE raylib)
    if(NOT MSVC)
        target_compile_options(PalmIslandMovementTests PRIVATE -Wall -Wextra)
    endif()
    add_test(NAME movement_collision COMMAND PalmIslandMovementTests)

    add_executable(PalmIslandCameraTests
        tests/camera_follow_test.c
        src/camera.c
        src/room.c
    )
    target_include_directories(PalmIslandCameraTests PRIVATE src)
    target_link_libraries(PalmIslandCameraTests PRIVATE raylib)
    if(NOT MSVC)
        target_compile_options(PalmIslandCameraTests PRIVATE -Wall -Wextra)
    endif()
    add_test(NAME camera_follow COMMAND PalmIslandCameraTests)

    add_executable(PalmIslandRenderTests
        tests/render_depth_test.c
        src/render.c
        src/player.c
        src/room.c
        src/thornback.c
    )
    target_include_directories(PalmIslandRenderTests PRIVATE src)
    target_link_libraries(PalmIslandRenderTests PRIVATE raylib)
    if(NOT MSVC)
        target_compile_options(PalmIslandRenderTests PRIVATE -Wall -Wextra)
    endif()
    add_test(NAME render_depth COMMAND PalmIslandRenderTests)

    add_executable(PalmIslandThornbackTests
        tests/thornback_trial_test.c
        src/thornback.c
    )
    target_include_directories(PalmIslandThornbackTests PRIVATE src)
    target_link_libraries(PalmIslandThornbackTests PRIVATE raylib)
    if(NOT MSVC)
        target_compile_options(PalmIslandThornbackTests PRIVATE -Wall -Wextra)
    endif()
    add_test(NAME thornback_trial COMMAND PalmIslandThornbackTests)
endif()
