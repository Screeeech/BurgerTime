#include "Components/Entity.hpp"

#include "Commands/MoveCommand.hpp"
#include "Components/Animation.hpp"
#include "Components/BurgerPart.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/EnemyAI.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Pepper.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "Locator.hpp"
#include "Services/InputManager.hpp"
#include "Services/ResourceManager.hpp"
#include "Services/Sound.hpp"
#include "States/EnemyStates.hpp"
#include "States/PlayerStates.hpp"
#include "Utils.hpp"

namespace bt
{

Entity::Entity(gla::GameObject* pOwner, int entityIndex, Type entityType)
    : Component(pOwner)
    , entityType(entityType)
    , entityIndex(entityIndex)
{
}

auto Entity::GetScoreForEnemyType(Type type) -> int
{
    switch (type)
    {
        case Type::HotDog:
            return score::hotdog;
        case Type::Pickle:
            return score::pickle;
        case Type::Egg:
            return score::egg;
        default:
            return 0;
    }
}

void Entity::OnActivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.BindAction<MoveCommand>("moveUp"_h, entityIndex, m_pOwner, glm::vec2{ 0, -1 });
    inputManager.BindAction<MoveCommand>("moveLeft"_h, entityIndex, m_pOwner, glm::vec2{ -1, 0 });
    inputManager.BindAction<MoveCommand>("moveDown"_h, entityIndex, m_pOwner, glm::vec2{ 0, 1 });
    inputManager.BindAction<MoveCommand>("moveRight"_h, entityIndex, m_pOwner, glm::vec2{ 1, 0 });
}

void Entity::OnDeactivate()
{
    auto& inputManager = gla::Locator::Get<gla::InputManager>();
    inputManager.UnbindAction("moveUp"_h, entityIndex);
    inputManager.UnbindAction("moveLeft"_h, entityIndex);
    inputManager.UnbindAction("moveDown"_h, entityIndex);
    inputManager.UnbindAction("moveRight"_h, entityIndex);
}

void Entity::CreatePlayer(gla::GameObject* playerObject, int entityIndex, Type playerType)
{
    using namespace playerstates;

    playerObject->AddComponent<gla::Timer>();
    auto* playerEntity = playerObject->AddComponent<Entity>(entityIndex, playerType);
    auto* moveComponent(playerObject->AddComponent<MoveComponent>(game::playerVelocity.x, game::playerVelocity.y));
    auto* animation(playerObject->AddComponent<gla::Animation>(layers::player));

    if (playerType == Type::Pepper)
    {
        DefineAnimationsPlayer(*animation, 0);
    }
    else if (playerType == Type::Salt)
    {
        DefineAnimationsPlayer(*animation, 2);
    }
    else
    {
        assert(false and "Cannot create player out of an enemy type");
    }

    auto* pepperObject{ playerObject->GetParent()->CreateChild(0, 0, std::format("Pepper{}", entityIndex)) };
    pepperObject->AddComponent<Pepper>(*playerEntity);

    auto* hitbox = playerObject->AddComponent<gla::CollisionRect>(
        gla::Collider::Bits::Layer2,
        gla::Collider::Bits::Layer3 | gla::Collider::Bits::Layer8,
        [=](auto& collider, auto&) -> void
        {
            playerObject->GetComponent<PlayerStateMachine>()->TransitionTo<Dying>();
            collider.Disable();

            gla::Locator::Get<gla::Sound>().PlayAudio("death"_h);
        },
        glm::vec2{ 4.f, 4.f },
        glm::vec2{ 8.f, 8.f });

    playerObject->AddComponent<PlayerStateMachine>(Context{
        .animation = *animation,
        .pepperTimer = *playerObject->AddComponent<gla::Timer>(),
        .collider = *hitbox,
        .moveComponent = *moveComponent,
        .playerIndex = entityIndex,
    });

}

void Entity::CreateEnemy(gla::GameObject* enemyObject, int entityIndex, Type entityType, glm::vec2 initialWalkingDirection)
{
    using namespace enemystates;

    switch (entityType)
    {
        case Type::Pepper:
        case Type::Salt:
            assert(false and "Cannot create of Player type");
            break;
        case Type::HotDog:
            DefineAnimationsEnemy(*enemyObject->AddComponent<gla::Animation>(layers::enemies), 4);
            break;
        case Type::Pickle:
            DefineAnimationsEnemy(*enemyObject->AddComponent<gla::Animation>(layers::enemies), 6);
            break;
        case Type::Egg:
            DefineAnimationsEnemy(*enemyObject->AddComponent<gla::Animation>(layers::enemies), 8);
            break;
    }

    auto* enemyEntity = enemyObject->AddComponent<Entity>(entityIndex, entityType);

    auto* pMoveComponent = enemyObject->AddComponent<MoveComponent>(game::enemyVelocity.x, game::enemyVelocity.y);
    auto* pStunTimer = enemyObject->AddComponent<gla::Timer>();
    auto* pAIDelayTimer = enemyObject->AddComponent<gla::Timer>();
    auto* pAnimation = enemyObject->GetComponent<gla::Animation>();

    auto* pPlayerHitBox = enemyObject->AddComponent<gla::CollisionRect>(
        gla::Collider::Bits::Layer1,
        gla::Collider::Bits::Layer2,
        gla::EntityEvent("OnPepper"_h, entityIndex),
        glm::vec2{ 4.f, 4.f },
        glm::vec2{ 8.f, 8.f });

    auto* pHeadHurtBox = enemyObject->AddComponent<gla::CollisionRect>(
        gla::Collider::Bits::Layer5,
        0,
        [=](auto&, auto&) -> void { enemyObject->GetComponent<EnemyStateMachine>()->TransitionTo<Dying>(); },
        glm::vec2{ 4.f, 0.f },
        glm::vec2{ 8.f, 4.f });

    auto* pFeetHurtBox = enemyObject->AddComponent<gla::CollisionRect>(
        gla::Collider::Bits::Layer6,
        0,
        [=](auto& feetHurtBox, gla::Collider const& otherCollider) -> void
        {
            auto* collidingBurger = otherCollider.m_pOwner->GetComponent<BurgerPart>();
            assert(collidingBurger != nullptr and "Collider's parent needs to have a BurgerComponent");

            collidingBurger->AcquireEnemy(*enemyObject, *enemyEntity);

            // When falling we disable the feet hurt box so no 2nd burger parts can be triggered
            feetHurtBox.DisableCollisionLayers(gla::Collider::Bits::Layer6);

            enemyObject->GetComponent<EnemyStateMachine>()->TransitionTo<Falling>();
        },
        glm::vec2{ 4.f, 12.f },
        glm::vec2{ 8.f, 4.f });

    enemyObject->AddComponent<EnemyStateMachine>(Context{
        .animation = *pAnimation,
        .stunTimer = *pStunTimer,
        .aiDelayTimer = *pAIDelayTimer,
        .moveComponent = *pMoveComponent,
        .playerHitbox = *pPlayerHitBox,
        .headBurtBox = *pHeadHurtBox,
        .feetHurtBox = *pFeetHurtBox,
        .entityIndex = entityIndex,
        .type = entityType,
        .initialWalkingDirection = initialWalkingDirection,
    });
}

void Entity::DefineAnimationsPlayer(gla::Animation& animation, int rowIndex)
{
    auto const spriteSheetTexture = gla::Locator::Get<gla::ResourceManager>().LoadTexture("Textures/spritesheet.png");

    auto const size{ spriteSheetTexture->GetSize() };
    auto const cols{ static_cast<int>(size.x / 16.f) };
    auto const rows{ static_cast<int>(size.y / 16.f) };
    auto& spriteSheet{ animation.AddSpriteSheet(spriteSheetTexture, cols, rows) };

    animation.AddAnimation(
        "idle"_h,
        spriteSheet,
        {
            { .colIdx = 1, .rowIdx = rowIndex },
        });
    animation.AddAnimation(
        "walkDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = rowIndex, .duration = 4.f / 60.f },
            { .colIdx = 1, .rowIdx = rowIndex, .duration = 4.f / 60.f },
            { .colIdx = 2, .rowIdx = rowIndex, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkUp"_h,
        spriteSheet,
        {
            { .colIdx = 6, .rowIdx = rowIndex, .duration = 4.f / 60.f },
            { .colIdx = 7, .rowIdx = rowIndex, .duration = 4.f / 60.f },
            { .colIdx = 8, .rowIdx = rowIndex, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkLeft"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = rowIndex, .duration = 4.f / 60.f },
            { .colIdx = 4, .rowIdx = rowIndex, .duration = 4.f / 60.f },
            { .colIdx = 5, .rowIdx = rowIndex, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkRight"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = rowIndex, .duration = 4.f / 60.f, .flipX = true },
            { .colIdx = 4, .rowIdx = rowIndex, .duration = 4.f / 60.f, .flipX = true },
            { .colIdx = 5, .rowIdx = rowIndex, .duration = 4.f / 60.f, .flipX = true },
        });
    animation.AddAnimation(
        "pepperDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = rowIndex + 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "pepperLeft"_h,
        spriteSheet,
        {
            { .colIdx = 1, .rowIdx = rowIndex + 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "pepperRight"_h,
        spriteSheet,
        {
            { .colIdx = 1, .rowIdx = rowIndex + 1, .duration = 0.f, .flipX = true },
        });
    animation.AddAnimation(
        "pepperUp"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = rowIndex + 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "death"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = rowIndex + 1, .duration = 0.5f },
            { .colIdx = 4, .rowIdx = rowIndex + 1, .duration = 1.0f },
            { .colIdx = 5, .rowIdx = rowIndex + 1, .duration = 0.15f },
            { .colIdx = 6, .rowIdx = rowIndex + 1, .duration = 0.15f },
        });
    animation.AddAnimation(
        "dying"_h,
        spriteSheet,
        {
            { .colIdx = 7, .rowIdx = rowIndex + 1, .duration = 0.15f },
            { .colIdx = 8, .rowIdx = rowIndex + 1, .duration = 0.15f },
        });

    animation.SetAnimation("idle"_h, true);
}

void Entity::DefineAnimationsEnemy(gla::Animation& animation, int rowIndex)
{
    auto const spriteSheetTexture = gla::Locator::Get<gla::ResourceManager>().LoadTexture("Textures/spritesheet.png");

    auto const size{ spriteSheetTexture->GetSize() };
    auto const cols{ static_cast<int>(size.x / 16.f) };
    auto const rows{ static_cast<int>(size.y / 16.f) };
    auto& spriteSheet = animation.AddSpriteSheet(spriteSheetTexture, cols, rows);

    animation.AddAnimation(
        "idle"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = rowIndex },
        });
    animation.AddAnimation(
        "walkUp"_h,
        spriteSheet,
        {
            { .colIdx = 4, .rowIdx = rowIndex, .duration = 6.f / 60.f },
            { .colIdx = 5, .rowIdx = rowIndex, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = rowIndex, .duration = 6.f / 60.f },
            { .colIdx = 1, .rowIdx = rowIndex, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkLeft"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = rowIndex, .duration = 6.f / 60.f },
            { .colIdx = 3, .rowIdx = rowIndex, .duration = 6.f / 60.f },
        });
    animation.AddAnimation(
        "walkRight"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = rowIndex, .duration = 6.f / 60.f, .flipX = true },
            { .colIdx = 3, .rowIdx = rowIndex, .duration = 6.f / 60.f, .flipX = true },
        });
    animation.AddAnimation(
        "stunned"_h,
        spriteSheet,
        {
            { .colIdx = 4, .rowIdx = rowIndex + 1, .duration = 12.f / 60.f, .flipX = true },
            { .colIdx = 5, .rowIdx = rowIndex + 1, .duration = 12.f / 60.f, .flipX = true },
        });
    animation.AddAnimation(
        "dying"_h,
        spriteSheet,
        {
            {
                .colIdx = 0,
                .rowIdx = rowIndex + 1,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 1,
                .rowIdx = rowIndex + 1,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 2,
                .rowIdx = rowIndex + 1,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 3,
                .rowIdx = rowIndex + 1,
                .duration = 15.f / 60.f,
            },
            {
                .colIdx = 6,
                .rowIdx = rowIndex + 1,
                .duration = 15.f / 60.f,
            },
        });

    animation.SetAnimation("idle"_h, true);
}


}  // namespace bt