#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "GameObject.h"

dae::Transform::Transform(float x, float y, float z, GameObject* parent)
    : m_pOwner(parent)
    , m_localPosition(x, y, z)
    , m_worldPosition(x, y, z)
{
}

dae::Transform::Transform(const Transform& transform, GameObject* owner)
    : m_pOwner(owner)
    , m_localPosition(transform.m_localPosition)
{
}

void dae::Transform::SetLocalPosition(const float x, const float y, const float z)
{
    m_localPosition.x = x;
    m_localPosition.y = y;
    m_localPosition.z = z;

    m_pOwner->SetDirty();
}

void dae::Transform::SetLocalPosition(const glm::vec3& position)
{
    m_localPosition = position;

    m_pOwner->SetDirty();
}

void dae::Transform::ChangeLocalPosition(float x, float y, float z)
{
    m_localPosition.x += x;
    m_localPosition.y += y;
    m_localPosition.z += z;

    m_pOwner->SetDirty();
}

void dae::Transform::ChangeLocalPosition(const glm::vec3& delta)
{
    m_localPosition += delta;

    m_pOwner->SetDirty();
}

glm::vec3 dae::Transform::GetLocalPosition() const
{
    // Doesn't have to set dirty flag, because it doesn't affect children
    return m_localPosition;
}

glm::vec3 dae::Transform::GetWorldPosition()
{
    if(isDirty)
        UpdateWorldMatrix();

    m_worldPosition = glm::vec3(m_worldMatrix[3]);
    return m_worldPosition;
}

void dae::Transform::SetScale(float x, float y, float z)
{
    m_localScale.x = x;
    m_localScale.y = y;
    m_localScale.z = z;

    m_pOwner->SetDirty();
}

glm::mat4 dae::Transform::GetWorldMatrix()
{
    if(isDirty)
        UpdateWorldMatrix();

    return m_worldMatrix;
}

glm::vec2 dae::Transform::GetLocalScale() const
{
    return m_localScale;
}

glm::vec2 dae::Transform::GetWorldScale()
{
    if(isDirty)
        UpdateWorldMatrix();

    return m_worldScale;
}

void dae::Transform::UpdateWorldMatrix()
{
    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, m_localRotation.z, glm::vec3(0, 0, 1));  // roll
    rotationMatrix = glm::rotate(rotationMatrix, m_localRotation.x, glm::vec3(1, 0, 0));  // pitch
    rotationMatrix = glm::rotate(rotationMatrix, m_localRotation.y, glm::vec3(0, 1, 0));  // yaw

    const glm::mat4 localMatrix =
        glm::translate(glm::mat4(1.0f), m_localPosition) * rotationMatrix * glm::scale(glm::mat4(1.0f), m_localScale);

    m_worldMatrix = m_pOwner->GetParentWorldMatrix() * localMatrix;

    // Decompose the world matrix down to it's individual components
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::quat worldRotationQuat;
    glm::decompose(m_worldMatrix, m_worldScale, worldRotationQuat, m_localPosition, skew, perspective);
    m_worldRotation = glm::eulerAngles(worldRotationQuat);

    isDirty = false;
}

void dae::Transform::ApplyInverseTransform(const Transform& parentTransform)
{
    m_worldMatrix *= inverse(parentTransform.m_worldMatrix);
}
