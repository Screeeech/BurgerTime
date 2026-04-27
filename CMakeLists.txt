cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BurgerTime)

add_subdirectory(Galena)

set(GAME_TARGET ${PROJECT_NAME})

set(SOURCE_FILES
    "Source/Main.cpp"
    "Source/Commands/MoveCommand.cpp"
    "Source/Components/CacheComponent.cpp"
    "Source/Components/HealthComponent.cpp"
    "Source/Components/PlayerController.cpp"
    "Source/Components/ScoreComponent.cpp"
    "Source/AchievementManager.cpp"
)

add_executable(${GAME_TARGET} ${SOURCE_FILES})

if (EMSCRIPTEN)
  target_link_libraries(${GAME_TARGET} PRIVATE GalenaWeb)
else ()
  target_link_libraries(${GAME_TARGET} PRIVATE Galena)
endif ()

target_include_directories(${GAME_TARGET} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
)

target_compile_features(${GAME_TARGET} PRIVATE cxx_std_23)

if (MSVC)
  # compile at warning level 4 + treat warnings as errors
  target_compile_options(${GAME_TARGET} PRIVATE /W4 /WX)
else ()
  #  target_compile_options(${GAME_TARGET} PRIVATE -Wall -Wextra)
  target_compile_options(${GAME_TARGET} PRIVATE -Wall -Wextra -Werror)
endif ()

# ==================== Sanitizers ====================
option(USE_ASAN "Enable AddressSanitizer" OFF)
if (USE_ASAN)
  message(STATUS "AddressSanitizer is enabled")
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif ()

# ==================== PLATFORM SPECIFIC ====================

if (WIN32)
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Resources" "$<TARGET_FILE_DIR:${GAME_TARGET}>/Resources"
  )
elseif (EMSCRIPTEN)
  set(EMSCRIPTEN_COMPILE_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -g")
  set(EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH --preload-file \"${CMAKE_SOURCE_DIR}/Resources@/\"")

  # Use the Emscripten toolchain
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/EmscriptenToolchain.cmake)

  # Additional settings for Emscripten build
  set_target_properties(${GAME_TARGET} PROPERTIES
      COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
  )

  # Have emscripten generate a html page too.
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Web"
      "$<TARGET_FILE_DIR:${GAME_TARGET}>/"
  )
else ()
  # Copy resources folder to output directory after build
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
      "$<TARGET_FILE_DIR:${GAME_TARGET}>/Resources"
  )
endif ()