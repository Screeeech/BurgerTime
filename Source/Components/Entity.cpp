#include "Components/Entity.hpp"

#include "Commands/MoveCommand.hpp"
#include "Components/Animation.hpp"
#include "Components/BurgerPart.hpp"
#include "Components/CollisionRect.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/Pepper.hpp"
#include "Components/Stage.hpp"
#include "Components/Timer.hpp"
#include "Constants.hpp"
#include "Locator.hpp"
#include "Services/InputManager.hpp"
#include "Services/ISound.hpp"
#include "Services/ResourceManager.hpp"
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

void Entity::CreatePlayer(gla::GameObject* parent, int entityIndex, glm::vec2 startPosition)
{
    using namespace playerstates;

    auto* playerObject = parent->CreateChild(startPosition, std::format("Enemy {}", entityIndex));

    auto* playerEntity = playerObject->AddComponent<Entity>(entityIndex, Type::Player);
    playerObject->AddComponent<gla::Timer>();
    auto* moveComponent(playerObject->AddComponent<MoveComponent>(0.85f, 0.65f));
    auto* animation(playerObject->AddComponent<gla::Animation>(layers::player));
    DefineAnimationsPlayer(*animation);

    auto* pepperObject{ parent->CreateChild(0, 0, std::format("Pepper{}", entityIndex)) };
    pepperObject->AddComponent<Pepper>(*playerEntity);

    playerObject->AddComponent<PlayerStateMachine>(Context{
        .animation = *animation,
        .moveComponent = *moveComponent,
    });

    playerObject->AddComponent<gla::CollisionRect>(
        gla::Collider::Bits::Layer2,
        gla::Collider::Bits::Layer3,
        [=](auto& collider, auto&) -> void
        {
            playerObject->GetComponent<PlayerStateMachine>()->TransitionTo<Dying>();
            collider.Disable();

            gla::Locator::Get<gla::ISound>().PlayAudio("death"_h);
        },
        glm::vec2{ 3.f, 0.f },
        glm::vec2{ 10.f, 16.f });
}

void Entity::CreateEnemy(gla::GameObject* parent, int entityIndex, glm::vec2 startPosition, Type entityType)
{
    using namespace enemystates;

    auto* enemyObject = parent->CreateChild(startPosition, std::format("Enemy {}", entityIndex));

    switch (entityType)
    {
        case Type::Player:
            assert(false and "Cannot create of Player type");
            break;
        case Type::HotDog:
            DefineAnimationsEnemy(*enemyObject->AddComponent<gla::Animation>(layers::enemies), 2);
            break;
        case Type::Pickle:
            DefineAnimationsEnemy(*enemyObject->AddComponent<gla::Animation>(layers::enemies), 4);
            break;
        case Type::Egg:
            DefineAnimationsEnemy(*enemyObject->AddComponent<gla::Animation>(layers::enemies), 6);
            break;
    }

    auto* enemyEntity = enemyObject->AddComponent<Entity>(entityIndex, entityType);

    auto* pMoveComponent = enemyObject->AddComponent<MoveComponent>(0.6f, 0.5f);
    auto* pTimer = enemyObject->AddComponent<gla::Timer>();
    auto* pAnimation = enemyObject->GetComponent<gla::Animation>();

    auto* pPlayerHitBox = enemyObject->AddComponent<gla::CollisionRect>(
        gla::Collider::Bits::Layer1,
        gla::Collider::Bits::Layer2,
        gla::PlayerEvent("OnPepper"_h, entityIndex),
        glm::vec2{},
        glm::vec2{ 16.f, 16.f });

    auto* pHeadHurtBox = enemyObject->AddComponent<gla::CollisionRect>(
        gla::Collider::Bits::Layer5,
        0,
        [=](auto&, auto&) -> void { enemyObject->GetComponent<EnemyStateMachine>()->TransitionTo<Dying>(); },
        glm::vec2{},
        glm::vec2{ 16.f, 4.f });

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
        glm::vec2{ 0.f, 12.f },
        glm::vec2{ 16.f, 4.f });

    enemyObject->AddComponent<EnemyStateMachine>(Context{
        .animation = *pAnimation,
        .stunTimer = *pTimer,
        .moveComponent = *pMoveComponent,
        .playerHitbox = *pPlayerHitBox,
        .headBurtBox = *pHeadHurtBox,
        .feetHurtBox = *pFeetHurtBox,
        .entityIndex = entityIndex,
        .type = entityType,
    });
}

void Entity::DefineAnimationsPlayer(gla::Animation& animation)
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
            { .colIdx = 1, .rowIdx = 0 },
        });
    animation.AddAnimation(
        "walkDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 1, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 2, .rowIdx = 0, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkUp"_h,
        spriteSheet,
        {
            { .colIdx = 6, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 7, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 8, .rowIdx = 0, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkLeft"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 4, .rowIdx = 0, .duration = 4.f / 60.f },
            { .colIdx = 5, .rowIdx = 0, .duration = 4.f / 60.f },
        });
    animation.AddAnimation(
        "walkRight"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
            { .colIdx = 4, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
            { .colIdx = 5, .rowIdx = 0, .duration = 4.f / 60.f, .flipX = true },
        });
    animation.AddAnimation(
        "pepperDown"_h,
        spriteSheet,
        {
            { .colIdx = 0, .rowIdx = 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "pepperLeft"_h,
        spriteSheet,
        {
            { .colIdx = 1, .rowIdx = 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "pepperRight"_h,
        spriteSheet,
        {
            { .colIdx = 1, .rowIdx = 1, .duration = 0.f, .flipX = true },
        });
    animation.AddAnimation(
        "pepperUp"_h,
        spriteSheet,
        {
            { .colIdx = 2, .rowIdx = 1, .duration = 0.f },
        });
    animation.AddAnimation(
        "death"_h,
        spriteSheet,
        {
            { .colIdx = 3, .rowIdx = 1, .duration = 0.5f },
            { .colIdx = 4, .rowIdx = 1, .duration = 1.0f },
            { .colIdx = 5, .rowIdx = 1, .duration = 0.15f },
            { .colIdx = 6, .rowIdx = 1, .duration = 0.15f },
        });
    animation.AddAnimation(
        "dying"_h,
        spriteSheet,
        {
            { .colIdx = 7, .rowIdx = 1, .duration = 0.15f },
            { .colIdx = 8, .rowIdx = 1, .duration = 0.15f },
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