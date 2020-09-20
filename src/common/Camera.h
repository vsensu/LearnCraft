//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_CAMERA_H
#define LEARNCRAFT_CAMERA_H

#include <tuple>

class Camera {
public:
    virtual ~Camera() {}

    using Vector3 = std::tuple<float, float, float>;
};

#endif //LEARNCRAFT_CAMERA_H
