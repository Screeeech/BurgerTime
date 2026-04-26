#include "../Include/Renderer.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <implot.h>

#include <iostream>
#include <stdexcept>

#include "../Include/SceneManager.hpp"
#include "../Include/Texture2D.hpp"

void dae::Renderer::Init(SDL_Window* window)
{
    m_window = window;

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

#if defined(__EMSCRIPTEN__)
    m_renderer = SDL_CreateRenderer(window, nullptr);
#else
    m_renderer = SDL_CreateRenderer(window, nullptr);
#endif

    if(m_renderer == nullptr)
    {
        std::cout << "Failed to create the renderer: " << SDL_GetError() << "\n";
        throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
#if __EMSCRIPTEN__
    io.IniFilename = NULL;
#endif

    ImGui_ImplSDL3_InitForSDLRenderer(window, m_renderer);
    ImGui_ImplSDLRenderer3_Init(m_renderer);
}

void dae::Renderer::Render() const
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    SceneManager::Get().DrawUI();

    ImGui::Render();

    const auto [r, g, b, a] = GetBackgroundColor();
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    SDL_RenderClear(m_renderer);

    SceneManager::Get().Render();

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);
    SDL_RenderPresent(m_renderer);
}

void dae::Renderer::Destroy()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    if(m_renderer != nullptr)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

void dae::Renderer::RenderTexture(const Texture2D& texture, float x, float y, SDL_FRect srcRect) const
{
    SDL_FRect dst{};
    dst.x = x;
    dst.y = y;
    const SDL_FRect* pSrcRect{};
    if(srcRect.w >= 0 and srcRect.h >= 0)
    {
        pSrcRect = &srcRect;
        dst.w = srcRect.w;
        dst.h = srcRect.h;
    }
    else
    {
        SDL_GetTextureSize(texture.GetSDLTexture(), &dst.w, &dst.h);
    }

    SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), pSrcRect, &dst);
}

void dae::Renderer::RenderTextureScale(const Texture2D& texture, float x, float y, float scaleX, float scaleY,
                                       SDL_FRect srcRect) const
{
    SDL_FRect dst{};
    dst.x = x;
    dst.y = y;

    const SDL_FRect* pSrcRect{};
    if(srcRect.w >= 0 and srcRect.h >= 0)
    {
        pSrcRect = &srcRect;
        dst.w = srcRect.w;
        dst.h = srcRect.w;
    }
    else
    {
        SDL_GetTextureSize(texture.GetSDLTexture(), &dst.w, &dst.h);
    }

    dst.w *= scaleX;
    dst.h *= scaleY;

    SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), pSrcRect, &dst);
}

void dae::Renderer::RenderTextureScaleFlipped(const Texture2D& texture, float x, float y, float scaleX, float scaleY, bool flipX,
                                              bool flipY, SDL_FRect srcRect) const
{
    SDL_FRect dst{};
    dst.x = x;
    dst.y = y;
    dst.w = srcRect.w * scaleX;
    dst.h = srcRect.w * scaleY;

    const SDL_FRect* pSrcRect{};
    if(srcRect.w >= 0 and srcRect.h >= 0)
        pSrcRect = &srcRect;

    constexpr SDL_FPoint origin = { 0.f, 0.f };

    uint8_t flipBits{};
    if(flipX)
        flipBits |= SDL_FLIP_HORIZONTAL;
    if(flipY)
        flipBits |= SDL_FLIP_VERTICAL;
    const auto flipMode{ static_cast<SDL_FlipMode>(flipBits) };

    SDL_RenderTextureRotated(GetSDLRenderer(), texture.GetSDLTexture(), pSrcRect, &dst, 0, &origin, flipMode);
}

SDL_Renderer* dae::Renderer::GetSDLRenderer() const
{
    return m_renderer;
}
