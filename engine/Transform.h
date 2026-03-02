#pragma once
#include <glm/glm.hpp>

namespace dae
{
class GameObject;

class Transform final
	{
	public:
	    Transform(float x, float y, float z = 0, GameObject* parent = nullptr);

		[[nodiscard]] const glm::vec3& GetPosition() const { return m_localPosition; }
		void SetLocalPosition(float x, float y, float z = 0);
		void SetLocalPosition(const glm::vec3& position);
	    void ChangeLocalPosition(float x, float y, float z = 0);
	    void ChangeLocalPosition(const glm::vec3& delta);
	    [[nodiscard]] glm::vec3 GetWorldPosition();


	    void UpdateWorldMatrix(const Transform& parentTransform = Transform(0, 0));
	    void ApplyInverseTransform(const Transform& parentTransform);

        bool isDirty{};
	private:
	    GameObject* m_pParent{};

	    glm::mat4 m_worldMatrix;
	    glm::vec3 m_rotation;

	    glm::vec3 m_scale;
		glm::vec3 m_localPosition;
		glm::vec3 m_worldPosition;
	};
}
