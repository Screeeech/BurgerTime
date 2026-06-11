#ifndef BURGERTIME_ENTITY_HPP
#define BURGERTIME_ENTITY_HPP
#include <glm/vec2.hpp>
#include <memory>

#include "Component.hpp"


namespace gla
{
class Texture2D;
class Animation;
}
namespace bt
{
class Stage;

class Entity final : public gla::Component
{
public:
    enum class Type : std::uint8_t
    {
        Pepper,
        Salt,
        HotDog,
        Pickle,
        Egg,
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
    static void CreatePlayer(gla::GameObject* parent, int entityIndex, glm::vec2 startPosition, Type playerType);
    static void CreateEnemy(gla::GameObject* parent, int entityIndex, glm::vec2 startPosition, Type entityType);
    static void DefineAnimationsPlayer(gla::Animation& animation, int rowIndex);
    static void DefineAnimationsEnemy(gla::Animation& animation, int rowIndex);
};

}  // namespace bt

#endif  // BURGERTIME_ENTITY_HPP
