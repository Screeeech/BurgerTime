#include "../Include/Scene.hpp"

#include <algorithm>
#include <cassert>

#include "../Include/Components/UIComponent.hpp"
#include "../Include/Renderable.hpp"

using namespace dae;

void Scene::RemoveGameObject(GameObject* pObject)
{
    assert(pObject != nullptr && "pObject is nullptr");
    m_pRootObject->RemoveChild(pObject);
}

void Scene::Update(float deltaTime)
{
    m_pRootObject->Update(deltaTime);
}

void Scene::Render()
{
    for(auto* renderComponents : m_renderComponents)
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

void Scene::RegisterRenderComponent(Renderable* renderable)
{
    m_renderComponents.push_back(renderable);
    SortCachedRenderComponents();
}

void Scene::UnregisterRenderComponent(Renderable* component)
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
    std::ranges::sort(m_renderComponents,
                      [](Renderable* pComp1, Renderable* pComp2) { return pComp1->GetZIndex() < pComp2->GetZIndex(); });
}

GameObject* Scene::GetRoot()
{
    return m_pRootObject.get();
}

Scene::Scene()
    : m_pRootObject{new GameObject(0, 0, 0, "Scene root")}
{}