//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_GAMEAPPLICATION_H
#define LEARNCRAFT_GAMEAPPLICATION_H

#include "common/Application.h"
class GameApplication : public Application
{
private:
    void Update() override;
    void RenderScene() override;
    void RenderUI() override;
};

#endif //LEARNCRAFT_GAMEAPPLICATION_H
