//
// Created by reili on 2020/10/8.
//

#include "DrawDebug.h"

void DrawDebug::DrawDebugBox(const Position &pos, const glm::vec3 &size)
{
    box_list.push_back(pos);
}

void DrawDebug::Render()
{
    box_shader->Use();
    box_shader->LoadUniform("projView", proj_view);
    box_shader->LoadUniform("model", glm::mat4(1.f));
    // draw all box
    for(const auto &pos: box_list)
    {
        box_shader->LoadUniform("pos", pos);
        box.draw();
    }
    box_list.clear();
}