#ifndef BURGERTIME_CACHECOMPONENT_H
#define BURGERTIME_CACHECOMPONENT_H
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <vector>

#include "Component.hpp"

namespace gla
{
class UIComponent;
}

namespace bt
{

class CacheComponent : public gla::Component
{
public:
    explicit CacheComponent(gla::GameObject* pOwner, int bufferSize = 10000);
    ~CacheComponent() noexcept override = default;

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

    class GameObject3DAlt
    {
    public:
        Transform* transform = new Transform();
        int ID{};
    };

    template<class T>
    void ThrashThatCache(int sampleCount, std::vector<T>& buffer, std::vector<uint32_t>& durations)
    {
        durations.clear();
        durations.resize(10);

        for(int sampleIndex{}; sampleIndex < sampleCount; ++sampleIndex)
        {
            int durationIndex{};
            for(int stepsize = 1; stepsize < 1024; stepsize *= 2)
            {
                auto start = std::chrono::high_resolution_clock::now();
                for(size_t i{}; i < buffer.size(); i += stepsize)
                {
                    if constexpr(requires { T::ID; })
                        buffer[i].ID *= stepsize + 1;
                    else
                        buffer[i] *= stepsize + 1;
                }
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
                durations.at(durationIndex) += duration;
                ++durationIndex;
            }
        }

        // Divide to get average
        std::ranges::transform(durations, durations.begin(),
                               [sampleCount](const auto& duration) { return duration / sampleCount; });
    }

    void RunExercise1(int sampleCount);
    void RunExercise2(int sampleCount);
    void RunExercise3(int sampleCount);

    gla::UIComponent* m_pUIComponent1{};
    gla::UIComponent* m_pUIComponent2{};

    std::vector<int> m_buffer1;
    std::vector<uint32_t> m_durations1;

    std::vector<GameObject3D> m_buffer2;
    std::vector<GameObject3DAlt> m_buffer3;
    std::vector<uint32_t> m_durations2;
};

}  // namespace bt

#endif  // BURGERTIME_CACHECOMPONENT_H
