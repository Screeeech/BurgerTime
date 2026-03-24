#include "Scene.h"

#include <algorithm>

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
    std::ranges::sort(m_renderComponents,
                      [](RenderComponent* pComp1, RenderComponent* pComp2) { return pComp1->GetZIndex() < pComp2->GetZIndex(); });
}

GameObject* Scene::GetRoot()
{
    return m_pRootObject.get();
}

Scene::Scene()
    : m_pRootObject{new GameObject(0, 0, 0, "Scene root")}
{}