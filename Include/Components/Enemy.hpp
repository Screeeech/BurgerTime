#ifndef BURGERTIME_ENEMY_HPP
#define BURGERTIME_ENEMY_HPP
#include <memory>

#include "Component.hpp"
#include "States/EnemyStates.hpp"

namespace gla
{
class Timer;
class CollisionRect;
class Collider;
class Texture2D;
}  // namespace gla
namespace bt
{

class Stage;
class Enemy final : public gla::Component
{
public:
    explicit Enemy(gla::GameObject* pOwner, Stage* pStage, int entityIndex = 2);

    void LandOnPlatform() const;

    int const m_entityIndex;

protected:
    void OnActivate() override;
    void OnDeactivate() override;

private:
    static constexpr glm::vec2 spriteFeetOffset{ 8.f, 15.f };

    MoveComponent* m_pMoveComponent;
    gla::Timer* m_pTimer;
    gla::Animation* m_pAnimation{};
    gla::CollisionRect* m_pPlayerHitBox;
    gla::CollisionRect* m_pHeadHurtBox;
    gla::CollisionRect* m_pFeetHurtBox{};
    enemystates::EnemyStateMachine* m_pStateMachine;

    void OnDeath() const;
    void OnDrop(gla::Collider const& collider);

public:
    // Helper functions
    static void DefineAnimationsHotDog(gla::Animation& animation, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture);
    static void DefineAnimationsEgg(gla::Animation& animation, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture);
};

}  // namespace bt

#endif  // BURGERTIME_ENEMY_HPP
