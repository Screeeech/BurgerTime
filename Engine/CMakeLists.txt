cmake_minimum_required(VERSION 3.26)

if (EMSCRIPTEN)
  message("Using Emscripten!")
endif ()

set(ENGINE_TARGET BurgerTimeEngine)

set(ENGINE_SOURCES
    "Source/Commands/CallbackCommand.cpp"
    "Source/Commands/GameObjectCommand.cpp"
    "Source/Components/Animation.cpp"
    "Source/Components/FpsComponent.cpp"
    "Source/Components/RotatorComponent.cpp"
    "Source/Components/Sprite.cpp"
    "Source/Components/TextComponent.cpp"
    "Source/Components/UIComponent.cpp"
    "Source/Font.cpp"
    "Source/GameObject.cpp"
    "Source/InputManager.cpp"
    "Source/Minigin.cpp"
    "Source/Renderer.cpp"
    "Source/ResourceManager.cpp"
    "Source/Scene.cpp"
    "Source/SceneManager.cpp"
    "Source/Texture2D.cpp"
    "Source/Transform.cpp"
)

add_library(${ENGINE_TARGET} STATIC ${ENGINE_SOURCES})

target_include_directories(${ENGINE_TARGET} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
)

# ==================== LIBRARIES ====================

include(FetchContent)

# Add ImGui and ImPlot
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.92.6
    GIT_SHALLOW true
    GIT_PROGRESS true
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SOURCES
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)
target_link_libraries(imgui PUBLIC
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot
    GIT_TAG v0.17
    GIT_SHALLOW true
    GIT_PROGRESS true
)
FetchContent_MakeAvailable(implot)

set(IMPLOT_SOURCES
    "${implot_SOURCE_DIR}/implot.cpp"
    "${implot_SOURCE_DIR}/implot_demo.cpp"
    "${implot_SOURCE_DIR}/implot_items.cpp"
)
add_library(implot STATIC ${implot_SOURCE_DIR} ${IMPLOT_SOURCES})
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)

# add glm
find_package(glm CONFIG QUIET)
if (NOT glm_FOUND)
  FetchContent_Declare(
      glm
      URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_MakeAvailable(glm)
endif ()

# Add SDL3
find_package(SDL3 3.4 CONFIG QUIET)
if (NOT SDL3_FOUND)
  if (WIN32)
    FetchContent_Declare(
        SDL3
        URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
        DOWNLOAD_NO_PROGRESS ON
        DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}")
    find_package(SDL3 CONFIG REQUIRED)
  else ()
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(SDL3)
  endif ()
endif ()

# Add SDL3_ttf
find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
if (NOT SDL3_ttf_FOUND)
  if (WIN32)
    FetchContent_Declare(
        SDL3_ttf
        URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
        DOWNLOAD_NO_PROGRESS ON
        DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
    find_package(SDL3_ttf CONFIG REQUIRED)
  else ()
    FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    set(SDL3TTF_INSTALL OFF)
    if (EMSCRIPTEN)
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten")
      set(SDL3TTF_USE_SYSTEM_FREETYPE ON)
      set(SDL3TTF_VENDORED OFF)
      set(SDLTTF_PLUTOSVG OFF)
      add_compile_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
      add_link_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
    endif ()
    FetchContent_MakeAvailable(SDL3_ttf)
  endif ()
endif ()

if (WIN32)
  # Add VLD (Visual Leak Detector) for memory leak detection on Windows
  find_package(VLD CONFIG)
  target_include_directories(${ENGINE_TARGET} PUBLIC
      "${VLD_INCLUDE_DIR}"
  )
endif ()

target_include_directories(${ENGINE_TARGET} PUBLIC
    "${glm_SOURCE_DIR}"
    "${sdl3_SOURCE_DIR}/include"
    "${sdl3_image_SOURCE_DIR}/include"
    "${sdl3_ttf_SOURCE_DIR}/include"
    "${imgui_SOURCE_DIR}"
)

target_link_libraries(${ENGINE_TARGET} PUBLIC
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    imgui
    implot)

