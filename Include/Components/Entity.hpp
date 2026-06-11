#ifndef BURGERTIME_ENTITY_HPP
#define BURGERTIME_ENTITY_HPP
#include <glm/vec2.hpp>
#include <memory>

#include "Component.hpp"


namespace gla
{
class Texture2D;
class Animation;
}  // namespace gla
namespace bt
{
class Stage;

class Entity final : public gla::Component
{
public:
    static constexpr glm::vec2 playerVelocity{ 0.65f, 0.45f };
    static constexpr glm::vec2 enemyWalkSpeed{ 0.4f, 0.3f };

    enum class Type : std::uint8_t
    {
        HotDog,
        Pickle,
        Egg,
        Pepper,
        Salt,
    };

    explicit Entity(gla::GameObject* pOwner, int entityIndex, Type entityType);

    static auto GetScoreForEnemyType(Type type) -> int;

    Type const entityType;
    int const entityIndex;

protected:
    void OnActivate() override;
    void OnDeactivate() override;

public:
    // Enemy and Player creation helper functions
    static void CreatePlayer(gla::GameObject* parent, int entityIndex, Type playerType);
    static void CreateEnemy(gla::GameObject* enemyObject, int entityIndex, Type entityType, glm::vec2 initialWalkingDirection = {});
    static void DefineAnimationsPlayer(gla::Animation& animation, int rowIndex);
    static void DefineAnimationsEnemy(gla::Animation& animation, int rowIndex);
};

}  // namespace bt

#endif  // BURGERTIME_ENTITY_HPP
