#pragma once
#include <glm/glm.hpp>

namespace dae
{
class GameObject;

class Transform final
{
public:
    explicit Transform(float x, float y, float z = 0, GameObject* parent = nullptr);
    explicit Transform(const Transform& transform, GameObject* owner);

    // Position
    void SetLocalPosition(float x, float y, float z = 0);
    void SetLocalPosition(const glm::vec3& position);
    void ChangeLocalPosition(float x, float y, float z = 0);
    void ChangeLocalPosition(const glm::vec3& delta);
    [[nodiscard]] glm::vec3 GetLocalPosition() const;
    [[nodiscard]] glm::vec3 GetWorldPosition();

    // Rotation
    // TODO: Add once I need it :P

    // Scale
    void SetScale(float x, float y, float z = 1);
    [[nodiscard]] glm::vec2 GetLocalScale() const;
    [[nodiscard]] glm::vec2 GetWorldScale();


    // World
    [[nodiscard]] glm::mat4 GetWorldMatrix();
    void UpdateWorldMatrix();
    void ApplyInverseTransform(const Transform& parentTransform);

    bool isDirty{ true };

private:
    GameObject* m_pOwner{};

    glm::mat4 m_worldMatrix{ 1.f };

    glm::vec3 m_localRotation{ 0.f };
    glm::vec3 m_localScale{ 1.f };
    glm::vec3 m_localPosition{ 0.f };

    glm::vec3 m_worldRotation{ 0.f };
    glm::vec3 m_worldScale{ 1.f };
    glm::vec3 m_worldPosition{ 0.f };
};
}  // namespace dae
