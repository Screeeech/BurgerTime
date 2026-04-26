cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BurgerTime)

add_subdirectory(Engine)

set(GAME_TARGET ${PROJECT_NAME})

set(SOURCE_FILES
    "Source/Main.cpp"
    "Source/Commands/MoveCommand.cpp"
    "Source/Components/CacheComponent.cpp"
    "Source/Components/HealthComponent.cpp"
    "Source/Components/PlayerController.cpp"
    "Source/Components/ScoreComponent.cpp"
    "Source/AchievementManager.cpp"
    "Source/EventManager.cpp"
)

add_executable(${GAME_TARGET} ${TARGET_PARAM} ${SOURCE_FILES})

target_include_directories("${GAME_TARGET}" PRIVATE
    "${CMAKE_SOURCE_DIR}/Include/"
)

target_link_libraries(${GAME_TARGET} PRIVATE BurgerTimeEngine)

target_compile_features(${GAME_TARGET} PRIVATE cxx_std_23)

if (MSVC)
  # compile at warning level 4 + treat warnings as errors
  target_compile_options(${GAME_TARGET} PRIVATE /W4 /WX)
else ()
  #  target_compile_options(${GAME_TARGET} PRIVATE -Wall -Wextra)
  target_compile_options(${GAME_TARGET} PRIVATE -Wall -Wextra -Werror)
endif ()


# ==================== LIBRARIES ====================

# Add steam if available
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/steam")
find_package(Steamworks QUIET)

if (Steamworks_FOUND)
  target_link_libraries("${GAME_TARGET}" PUBLIC Steamworks::Steamworks)
  target_compile_definitions(${GAME_TARGET} PRIVATE USE_STEAMWORKS)

  if (WIN32)
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "${Steamworks_RUNTIME_DLL}" "$<TARGET_FILE_DIR:${GAME_TARGET}>")
  else ()
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${Steamworks_LIBRARIES}" "$<TARGET_FILE_DIR:${GAME_TARGET}>"
    )
  endif ()

  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${GAME_TARGET}>/steam_appid.txt"
      COMMENT "Generating steam_appid.txt"
  )
endif ()


# ==================== Sanitizers ====================
option(USE_ASAN "Enable AddressSanitizer" OFF)
if (USE_ASAN)
  message(STATUS "AddressSanitizer is enabled")
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif ()

# Copy resources folder to output directory after build
add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Resources" "$<TARGET_FILE_DIR:${GAME_TARGET}>/Resources"
)

if (WIN32)
  # Set game as the default startup project
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${GAME_TARGET})
  # Set working directory to output folder
  set_target_properties(${GAME_TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>")
elseif (EMSCRIPTEN)
  set(EMSCRIPTEN_COMPILE_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -g")
  set(EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH --preload-file \"${CMAKE_SOURCE_DIR}/Resources@/\"")

  # Use the Emscripten toolchain
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/EmscriptenToolchain.cmake)

  # Additional settings for Emscripten build
  set_target_properties(${ENGINE_TARGET} PROPERTIES
      COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
  )
  # Have emscripten generate a html page too.
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()