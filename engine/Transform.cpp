#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

#include "GameObject.h"

dae::Transform::Transform(float x, float y, float z, GameObject* parent)
    : m_pParent(parent)
    , m_localPosition(x, y, z)
    , m_worldPosition(x, y, z)
{
}

void dae::Transform::SetLocalPosition(const float x, const float y, const float z)
{
    m_localPosition.x = x;
    m_localPosition.y = y;
    m_localPosition.z = z;

    m_pParent->SetDirty();
}

void dae::Transform::SetLocalPosition(const glm::vec3& position)
{
    m_localPosition = position;

    m_pParent->SetDirty();
}

void dae::Transform::ChangeLocalPosition(float x, float y, float z)
{
    m_localPosition.x += x;
    m_localPosition.y += y;
    m_localPosition.z += z;

    m_pParent->SetDirty();
}

void dae::Transform::ChangeLocalPosition(const glm::vec3& delta)
{
    m_localPosition += delta;

    m_pParent->SetDirty();
}

glm::vec3 dae::Transform::GetWorldPosition()
{
    if(isDirty)
        UpdateWorldMatrix();

    return m_worldPosition;
}

void dae::Transform::UpdateWorldMatrix(const Transform& parentTransform)
{
    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, m_rotation.z, glm::vec3(0, 0, 1));  // roll
    rotationMatrix = glm::rotate(rotationMatrix, m_rotation.x, glm::vec3(1, 0, 0));  // pitch
    rotationMatrix = glm::rotate(rotationMatrix, m_rotation.y, glm::vec3(0, 1, 0));  // yaw

    const glm::mat4 localMatrix =
        glm::translate(glm::mat4(1.0f), m_localPosition) * rotationMatrix * glm::scale(glm::mat4(1.0f), m_scale);

    m_worldMatrix = parentTransform.m_worldMatrix * localMatrix;

    isDirty = false;
}

void dae::Transform::ApplyInverseTransform(const Transform& parentTransform)
{
    m_worldMatrix *= inverse(parentTransform.m_worldMatrix);
}
