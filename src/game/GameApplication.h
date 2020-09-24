//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_GAMEAPPLICATION_H
#define LEARNCRAFT_GAMEAPPLICATION_H

#include <memory>

#include "common/Application.h"
#include "common/world/World.h"
#include "common/Camera.h"

class GameApplication : public Application
{
public:
	using Application::Application;
private:
	void Init() override;
    void Update() override;
    void RenderScene() override;
    void RenderUI() override;

    std::shared_ptr<World> world_ { std::make_shared<World>(32, 32, 32) };
    Camera camera_;
};

#endif //LEARNCRAFT_GAMEAPPLICATION_H
