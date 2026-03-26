#ifndef ENGINE_RENDERABLE_H
#define ENGINE_RENDERABLE_H
#include "Component.h"
#include "SceneManager.h"

namespace dae
{

class Renderable : public Component
{
public:
    explicit Renderable(GameObject* pOwner, int zIndex)
        : Component(pOwner)
        , m_zIndex(zIndex)
    {
        // TODO: Change to Initialize and to parented scene instead of active scene
        SceneManager::Get().GetActiveScene()->RegisterRenderComponent(this);
    }

    ~Renderable() override
    {
        SceneManager::Get().UnregisterRenderComponent(this);
    }

    virtual void Render() = 0;

    void SetZIndex(int zIndex)
    {
        m_zIndex = zIndex;
        SceneManager::Get().SortCachedRenderComponents();
    }

    int GetZIndex() const
    {
        return m_zIndex;
    }

private:
    int m_zIndex;
};
}  // namespace dae
#endif  // ENGINE_RENDERABLE_H
