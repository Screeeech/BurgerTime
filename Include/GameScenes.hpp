#ifndef BURGERTIME_SCENES_HPP
#define BURGERTIME_SCENES_HPP

namespace gla
{
class Scene;
}

namespace bt
{
class GameState;

void LoadStartScene(gla::Scene const& scene);
void LoadLoadingScene(gla::Scene const& scene);


void LoadGameScene(gla::Scene const& scene, GameState const* gameState);
void LoadSinglePlayerGameScene(gla::Scene const& scene);
void LoadCoopGameScene(gla::Scene const& scene);
void LoadVersusGameScene(gla::Scene const& scene);
void LoadGameOverScene(gla::Scene const& scene);

}

#endif  // BURGERTIME_SCENES_HPP
