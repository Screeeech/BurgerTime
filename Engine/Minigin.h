#pragma once
#include <chrono>
#include <filesystem>
#include <functional>
#include <string>

namespace dae
{
class Minigin final
{
public:
    explicit Minigin(const std::filesystem::path& dataPath);
    ~Minigin() noexcept;
    void Run(const std::function<void()>& load);
    void RunOneFrame();

    Minigin(const Minigin& other) = delete;
    Minigin(Minigin&& other) = delete;
    Minigin& operator=(const Minigin& other) = delete;
    Minigin& operator=(Minigin&& other) = delete;

private:
    bool m_quit{};
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
};
}  // namespace dae
