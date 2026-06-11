cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BurgerTime)

if (EMSCRIPTEN)
  message("Using emscripten!")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(RESOURCE_DIR "Resources")

add_subdirectory(Galena)

set(GAME_TARGET ${PROJECT_NAME})


set(SOURCE_FILES
    "Source/Main.cpp"
    "Source/GameScenes.cpp"
    "Source/Services/GameState.cpp"
    "Source/Commands/MoveCommand.cpp"
    "Source/Components/BurgerPart.cpp"
    "Source/Components/Stage.cpp"
    "Source/Components/MoveComponent.cpp"
    "Source/Components/Pepper.cpp"
    "Source/Components/PepperDisplay.cpp"
    "Source/Components/HealthDisplay.cpp"
    "Source/Components/Plate.cpp"
    "Source/Components/Score.cpp"
    "Source/Components/StartMenu.cpp"
    "Source/Components/HighScore.cpp"
    "Source/Components/Entity.cpp"
    "Source/Components/EnemyAI.cpp"
    "Source/AchievementManager.cpp"
    "Source/States/BurgerPartStates.cpp"
    "Source/States/PlayerStates.cpp"
    "Source/States/EnemyStates.cpp"
)

add_executable(${GAME_TARGET} ${SOURCE_FILES})

target_link_libraries(${GAME_TARGET} PRIVATE Galena)

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
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)
target_link_libraries(${GAME_TARGET} PRIVATE nlohmann_json::nlohmann_json)

# ==================== PLATFORM SPECIFIC ====================
if (WIN32)
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_DIR}" "$<TARGET_FILE_DIR:${GAME_TARGET}>/${RESOURCE_DIR}"
  )

  # Set Game as the default startup project
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${GAME_TARGET})
  # Set working directory to output folder
  set_target_properties(${GAME_TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${GAME_TARGET}>")

elseif (EMSCRIPTEN)
  target_link_options(${GAME_TARGET} PRIVATE
      -sEXCEPTION_DEBUG
      -sNO_DISABLE_EXCEPTION_CATCHING
      -sALLOW_MEMORY_GROWTH
      --preload-file "${CMAKE_SOURCE_DIR}/${RESOURCE_DIR}@/"
  )
  set_target_properties(${GAME_TARGET} PROPERTIES SUFFIX ".html")

  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Web"
      "$<TARGET_FILE_DIR:${GAME_TARGET}>/"
  )
else ()
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_DIR}"
      "$<TARGET_FILE_DIR:${GAME_TARGET}>/${RESOURCE_DIR}"
  )
endif ()