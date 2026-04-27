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

add_executable(${GAME_TARGET} ${SOURCE_FILES})

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
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/web" "$<TARGET_FILE_DIR:${GAME_TARGET}>/"
  )
else ()
  # Copy resources folder to output directory after build
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Resources" "$<TARGET_FILE_DIR:${GAME_TARGET}>/Resources"
  )
endif ()