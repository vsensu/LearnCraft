//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_APPLICATION_H
#define LEARNCRAFT_APPLICATION_H

#include <tuple>

struct GLFWwindow;

class Application
{
public:
    virtual ~Application() {}

    Application();
    Application(int argc, char *argv[]);
    int Run();

    using Color = std::tuple<float, float, float, float>;

    static inline Color MakeColor(float r, float g, float b, float a)
    {
        return std::make_tuple(r, g, b, a);
    }

protected:
    double delta_time_;
    float fps_;

private:
    int InitOpenGL();
    int InitImgui();
    virtual void FixedUpdate(double fixedDeltaTime) {}
    virtual void Update() {}
    virtual void RenderScene() {}
    virtual void RenderUI() {}

    // no need to free memory in deconstructor, glfwTerminate will handle it.
    GLFWwindow* window_ {nullptr};
    Color clear_color_ { MakeColor(1.f, 1.f, 1.f, 1.f)};
};

#endif //LEARNCRAFT_APPLICATION_H
