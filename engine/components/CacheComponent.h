//
// Created by lily-laptop on 04/03/2026.
//

#ifndef ENGINE_CACHECOMPONENT_H
#define ENGINE_CACHECOMPONENT_H
#include <cstdint>
#include <vector>

#include "Component.h"

namespace dae
{
class UIComponent;

class CacheComponent : public Component
{
public:
    explicit CacheComponent(GameObject* pOwner, int bufferSize = 10000);

    void Update(float deltaTime) override;

private:
    struct Transform
    {
        float matrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    };

    class GameObject3D
    {
    public:
        Transform transform;
        int ID{};
    };

    void RunExercise1(int sampleCount);
    void RunExercise2(int sampleCount);

    UIComponent* m_pUIComponent1{};
    UIComponent* m_pUIComponent2{};

    std::vector<int> m_buffer1;
    std::vector<uint32_t> m_durations1;

    std::vector<GameObject3D> m_buffer2;
    std::vector<uint32_t> m_durations2;
};

}  // namespace dae
#endif  // ENGINE_CACHECOMPONENT_H
