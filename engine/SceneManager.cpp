#include "SceneManager.h"

#include "Scene.h"

void dae::SceneManager::Update(float deltaTime)
{
    if(not m_currentScene)
        return;

    m_currentScene->Update(deltaTime);
}

void dae::SceneManager::Render()
{
    if(not m_currentScene)
        return;

    m_currentScene->Render();
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