//
// Created by lily-laptop on 04/03/2026.
//

#include "CacheComponent.h"

#include <print>

#include "GameObject.h"
#include "imgui.h"

dae::CacheComponent::CacheComponent(GameObject* pOwner, int bufferSize)
    : Component(pOwner)
    , m_pUIComponent1(pOwner->AddComponent<UIComponent>(
          [this](GameObject*) mutable
          {
              static int sampleCount = 0;
              if(ImGui::Begin("Exercise 1"))
              {
                  ImGui::InputInt("samples", &sampleCount);
                  if(ImGui::Button("Thrash the Cache"))
                      RunExercise1(sampleCount);
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
    m_buffer.reserve(bufferSize);
}

void dae::CacheComponent::Update(float) {}

void dae::CacheComponent::RunExercise1(int sampleCount)
{
    std::println("Running exercise 1 with {} samples", sampleCount);
}

void dae::CacheComponent::RunExercise2(int sampleCount)
{
    std::println("Running exercise 2 with {} samples", sampleCount);
}