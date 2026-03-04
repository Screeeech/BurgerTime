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
    void RunExercise1(int sampleCount);
    void RunExercise2(int sampleCount);

    UIComponent* m_pUIComponent1{};
    UIComponent* m_pUIComponent2{};

    std::vector<int> m_buffer;
    std::vector<uint32_t> m_durations1;
    std::vector<uint32_t> m_durations2;
};

}
#endif  // ENGINE_CACHECOMPONENT_H
