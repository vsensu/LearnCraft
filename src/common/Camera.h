//
// Created by reili on 2020/9/21.
//

#ifndef MOT_CAMERA_H
#define MOT_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    [[maybe_unused]] inline auto View () const
    {
        return glm::lookAt(pos_, pos_ + forward_, up_);
    }

    [[maybe_unused]] inline void MoveForward(double deltaTime)
    {
        float speed = 2.5f * static_cast<float>(deltaTime);
        pos_ += forward_ * speed;
    }

    [[maybe_unused]] inline void MoveBackward(double deltaTime)
    {
        float speed = 2.5f * static_cast<float>(deltaTime);
        pos_ -= forward_ * speed;
    }

    [[maybe_unused]] inline void MoveLeft(double deltaTime)
    {
        float speed = 2.5f * static_cast<float>(deltaTime);
        pos_ -= glm::normalize(glm::cross(forward_, up_)) * speed;
    }

    [[maybe_unused]] inline void MoveRight(double deltaTime)
    {
        float speed = 2.5f * static_cast<float>(deltaTime);
        pos_ += glm::normalize(glm::cross(forward_, up_)) * speed;
    }

    [[maybe_unused]] void MouseCallback(GLFWwindow *window, double xpos, double ypos)
    {
        auto xposf = static_cast<float>(xpos);
        auto yposf = static_cast<float>(ypos);
        if(first_mouse_)
        {
            last_x_ = xposf;
            last_y_ = yposf;
            first_mouse_ = false;
        }

        float xoffset = xposf - last_x_;
        float yoffset = last_y_ - yposf;
        last_x_ = xposf;
        last_y_ = yposf;

        float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw_   += xoffset;
        pitch_ += yoffset;

        if(pitch_ > 89.0f)
            pitch_ = 89.0f;
        if(pitch_ < -89.0f)
            pitch_ = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        forward_ = glm::normalize(front);
    }

    [[maybe_unused]] inline auto Perspective (float aspect)
    {
        return glm::perspective(glm::radians(fov_), aspect, 0.1f, 100.0f);
    }

    [[maybe_unused]] void Zoom(double yoffset)
    {
        if (fov_ >= 1.0f && fov_ <= 45.0f)
            fov_ -= static_cast<float>(yoffset);
        if (fov_ <= 1.0f)
            fov_ = 1.0f;
        if (fov_ >= 45.0f)
            fov_ = 45.0f;
    }

    inline auto GetPos() const { return pos_; }
    inline auto GetForward() const { return forward_; }

private:
    glm::vec3 pos_{glm::vec3(0.f, 0.f, 3.f)};
    glm::vec3 up_ {glm::vec3(0.f, 1.f, 0.f)};
    glm::vec3 forward_ {glm::vec3(0.f, 0.f, -1.f)};

    float yaw_ {0.f}, pitch_ {0.f}, roll_ {0.f};
    float fov_ {45.f};

    bool first_mouse_ {true};
    float last_x_ {400.f}, last_y_ {300.f};
};

#endif //MOT_CAMERA_H
