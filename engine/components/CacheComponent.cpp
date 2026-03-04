//
// Created by lily-laptop on 04/03/2026.
//

#include "CacheComponent.h"

#include <algorithm>
#include <chrono>
#include <print>

#include "GameObject.h"
#include "imgui.h"
#include "implot.h"

dae::CacheComponent::CacheComponent(GameObject* pOwner, int bufferSize)
    : Component(pOwner)
    , m_pUIComponent1(pOwner->AddComponent<UIComponent>(
          [this](GameObject*) mutable
          {
              static int sampleCount = 1;
              static bool plotExists{};

              if(ImGui::Begin("Exercise 1"))
              {
                  ImGui::InputInt("samples", &sampleCount);
                  if(ImGui::Button("Thrash the Cache"))
                  {
                      plotExists = true;
                      RunExercise1(sampleCount);
                  }
              }

              if(plotExists)
              {
                  ImPlot::BeginPlot("Durations");

                  ImPlot::PlotLine<uint32_t>("", m_durations1.data(), static_cast<int>(m_durations1.size()), 10, 0);

                  ImPlot::EndPlot();
              }

              ImGui::End();
          }))
    , m_pUIComponent2(pOwner->AddComponent<UIComponent>(
          [this](GameObject*) mutable
          {
              static int sampleCount = 0;
              if(ImGui::Begin("Exercise 2"))
              {
                  ImGui::InputInt("samples", &sampleCount);
                  if(ImGui::Button("Thrash the Cache"))
                      RunExercise2(sampleCount);
              }
              ImGui::End();
          }))
{
    m_buffer.resize(bufferSize);
    std::ranges::fill(m_buffer, 1);
}

void dae::CacheComponent::Update(float) {}

void dae::CacheComponent::RunExercise1(int sampleCount)
{
    std::println("Running exercise 1 with {} samples", sampleCount);
    m_durations1.resize(10);
    std::fill_n(m_durations1.begin(), 10, 0);

    for(int sampleIndex{}; sampleIndex < sampleCount; ++sampleIndex)
    {
        int durationIndex{};
        for(int stepsize = 1; stepsize < 1024; stepsize *= 2)
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(size_t i{}; i < m_buffer.size(); i += stepsize)
            {
                m_buffer[i] *= stepsize + 1;
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
            m_durations1.at(durationIndex) += duration;
            ++durationIndex;
        }
    }

    // Divide to get average
    std::ranges::transform(m_durations1, m_durations1.begin(), [sampleCount](const auto& duration){ return duration / sampleCount; });
}

void dae::CacheComponent::RunExercise2(int sampleCount)
{
    std::println("Running exercise 2 with {} samples", sampleCount);
}