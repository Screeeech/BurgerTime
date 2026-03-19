#include "Scene.h"

#include <algorithm>
#include <cassert>

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject>&& object)
{
    assert(object != nullptr && "Cannot add a null GameObject to the scene.");
    m_objects.emplace_back(std::move(object));
}

void Scene::Remove(GameObject* pGameObject)
{
    std::erase_if(m_objects, [&pGameObject](const std::unique_ptr<GameObject>& ptr) { return ptr.get() == pGameObject; });
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
    SceneManager::Get().LoadScene(this);
}

void Scene::RegisterRenderComponent(RenderComponent* renderComponent)
{
    m_renderComponents.push_back(renderComponent);
    SortCachedRenderComponents();
}

void Scene::UnregisterRenderComponent(RenderComponent* component)
{
    if(not m_renderComponents.empty())
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

void Scene::SortCachedRenderComponents()
{
    std::ranges::sort(m_renderComponents, [](RenderComponent* pComp1, RenderComponent* pComp2)
    {
        return pComp1->GetZIndex() < pComp2->GetZIndex();
    } );
}