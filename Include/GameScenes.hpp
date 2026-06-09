#ifndef BURGERTIME_SCENES_HPP
#define BURGERTIME_SCENES_HPP

namespace gla
{
class Scene;
}

namespace bt
{

void LoadStartScene(gla::Scene const& scene);
void UnloadStartScene();

void LoadGameScene(gla::Scene const& scene);

void LoadSinglePlayerGameScene(gla::Scene const& scene);
void UnloadGameScene();

void LoadCoopGameScene(gla::Scene const& scene);
void UnloadCoopGameScene();

void LoadVersusGameScene(gla::Scene const& scene);
void UnloadVersusGameScene();

}

#endif  // BURGERTIME_SCENES_HPP
