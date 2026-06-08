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
void UnloadGameScene();

}

#endif  // BURGERTIME_SCENES_HPP
