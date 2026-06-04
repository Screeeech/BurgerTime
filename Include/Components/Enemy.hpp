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
    explicit Enemy(gla::GameObject* pOwner, Stage* pStage, int playerIndex = 2);

    void LandOnPlatform();

    int const m_entityIndex;

protected:
    void FixedUpdate(float fixedDeltaTime) override;

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
    enemystates::EnemyStateMachine m_stateMachine;

    void OnDeath();
    void OnDrop(gla::Collider const& collider);

public:
    // Helper functions
    static void DefineAnimations(gla::Animation& animation, std::shared_ptr<gla::Texture2D> const& spriteSheetTexture);
};

}  // namespace bt

#endif  // BURGERTIME_ENEMY_HPP
