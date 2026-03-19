#include "SceneManager.h"

#include <print>

#include "Scene.h"

void dae::SceneManager::Update(float deltaTime)
{
    if(not m_currentScene)
        return;

    m_currentScene->Update(deltaTime);
}

void dae::SceneManager::Render() const
{
    if(not m_currentScene)
        return;

    m_currentScene->Render();
}

void dae::SceneManager::DrawUI() const
{
    if(not m_currentScene)
        return;

    m_currentScene->DrawUI();
}

void dae::SceneManager::LoadScene(Scene* scene)
{
    // Check if scene exists
    // If it does store a reference in m_currentScene
    for(auto& pScene : m_scenes)
    {
        if(auto* sceneRef = pScene.get(); sceneRef == scene)
            m_currentScene = sceneRef;
    }
}

dae::Scene* dae::SceneManager::GetActiveScene() const
{
    return m_currentScene;
}

dae::Scene& dae::SceneManager::CreateScene()
{
    m_scenes.emplace_back(new Scene());
    return *m_scenes.back();
}

void dae::SceneManager::RegisterRenderComponent(RenderComponent* component) const
{
    if(not m_currentScene)
        return;

    m_currentScene->RegisterRenderComponent(component);
}

void dae::SceneManager::UnregisterRenderComponent(RenderComponent* component) const
{
    if(not m_currentScene)
        return;

    m_currentScene->UnregisterRenderComponent(component);
}

void dae::SceneManager::RegisterUIComponent(UIComponent* component) const
{
    if(not m_currentScene)
        return;

    m_currentScene->RegisterUIComponent(component);
}

void dae::SceneManager::UnregisterUIComponent(UIComponent* component) const
{
    if(not m_currentScene)
        return;

    m_currentScene->UnregisterUIComponent(component);
}


void dae::SceneManager::Cleanup()
{
    m_scenes.clear();
    m_currentScene = nullptr;
}

void dae::SceneManager::SortCachedRenderComponents()
{
    if(not m_currentScene)
        return;

    m_currentScene->SortCachedRenderComponents();
}