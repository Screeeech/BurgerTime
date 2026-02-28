#include "Scene.h"

#include <algorithm>
#include <cassert>

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
    assert(object != nullptr && "Cannot add a null GameObject to the scene.");
    m_objects.emplace_back(std::move(object));
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
    for(auto& object : m_objects)
    {
        object->Update(deltaTime);
    }
}

void Scene::Render()
{
    for(const auto& renderComponents : m_pRenderComponents)
    {
        renderComponents->Render();
    }
}

void Scene::Load()
{
    SceneManager::GetInstance().LoadScene(this);
}

void Scene::RegisterRenderComponent(RenderComponent* renderComponent)
{
    m_pRenderComponents.push_back(renderComponent);
}

void Scene::UnregisterRenderComponent(RenderComponent* component)
{
    std::erase(m_pRenderComponents, component);
}