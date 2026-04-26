#include "CacheComponent.hpp"

#include <print>

#include "Components/UIComponent.hpp"
#include "GameObject.hpp"
#include <imgui.h>
#include <implot.h>

dae::CacheComponent::CacheComponent(GameObject* pOwner, int bufferSize)
    : Component(pOwner)
    , m_pUIComponent1(pOwner->AddComponent<UIComponent>(
          [this](GameObject*) mutable
          {
              static int sampleCount = 30;

              if(ImGui::Begin("Exercise 1"))
              {
                  ImGui::InputInt("samples", &sampleCount);
                  if(ImGui::Button("Thrash the Cache"))
                      RunExercise1(sampleCount);

                  ImPlot::SetNextAxesToFit();
                  ImPlot::BeginPlot("Durations with integers");
                  ImPlot::PlotLine<uint32_t>("", m_durations1.data(), static_cast<int>(m_durations1.size()), 100, 0);
                  ImPlot::EndPlot();
              }

              ImGui::End();
          }))
    , m_pUIComponent2(pOwner->AddComponent<UIComponent>(
          [this](GameObject*) mutable
          {
              static int sampleCount = 30;

              if(ImGui::Begin("Exercise 2"))
              {
                  ImGui::InputInt("samples", &sampleCount);
                  if(ImGui::Button("Thrash the Cache with GameObject3D"))
                  {
                      RunExercise2(sampleCount);
                  }
                  if(ImGui::Button("Thrash the Cache with GameObject3DAlt"))
                  {
                      RunExercise3(sampleCount);
                  }

                  ImPlot::SetNextAxesToFit();
                  ImPlot::BeginPlot("Durations with GameObject3D(Alt)");
                  ImPlot::PlotLine<uint32_t>("", m_durations2.data(), static_cast<int>(m_durations2.size()), 100, 0);
                  ImPlot::EndPlot();
              }

              ImGui::End();
          }))
{
    m_buffer1.resize(bufferSize);
    std::ranges::fill(m_buffer1, 1);

    m_buffer2.resize(bufferSize);
    m_buffer3.resize(bufferSize);
}

void dae::CacheComponent::Update(float) {}

void dae::CacheComponent::RunExercise1(int sampleCount)
{
    std::println("Running exercise 1 with {} samples", sampleCount);
    ThrashThatCache(sampleCount, m_buffer1, m_durations1);
}

void dae::CacheComponent::RunExercise2(int sampleCount)
{
    std::println("Running exercise 2 with {} samples", sampleCount);
    ThrashThatCache(sampleCount, m_buffer2, m_durations2);
}

void dae::CacheComponent::RunExercise3(int sampleCount)
{
    std::println("Running exercise 3 with {} samples", sampleCount);
    ThrashThatCache(sampleCount, m_buffer3, m_durations2);
}
