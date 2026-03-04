#include "Scene.h"

#include <algorithm>
#include <cassert>

using namespace dae;

void Scene::Add(GameObject* object)
{
    assert(object != nullptr && "Cannot add a null GameObject to the scene.");
    m_objects.emplace_back(object);
}

void Scene::Remove(const GameObject& object)
{
    std::erase_if(m_objects, [&object](const std::unique_ptr<GameObject>& ptr) { return ptr.get() == &object; });
}

void Scene::RemoveAll()
{
    m_objects.clear();
}

void Scene::Update(float deltaTime)
{
    for(const auto& object : m_objects)
    {
        object->Update(deltaTime);
    }
}

void Scene::Render()
{
    for(const auto* renderComponents : m_renderComponents)
    {
        renderComponents->Render();
    }
}

void Scene::DrawUI()
{
    for(const auto* uiComponent : m_uiComponents)
    {
        uiComponent->DrawUI();
    }
}

void Scene::Load()
{
    SceneManager::GetInstance().LoadScene(this);
}

void Scene::RegisterRenderComponent(RenderComponent* renderComponent)
{
    m_renderComponents.push_back(renderComponent);
}

void Scene::UnregisterRenderComponent(RenderComponent* component)
{
    std::erase(m_renderComponents, component);
}

void Scene::RegisterUIComponent(UIComponent* component)
{
    m_uiComponents.push_back(component);
}

void Scene::UnregisterUIComponent(UIComponent* component)
{
    std::erase(m_uiComponents, component);
}