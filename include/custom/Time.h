#ifndef TIME_H
#define TIME_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

/*-----------------------------------------------------------------*/
/* TIME: Stores time information and creates time UBO for shaders. */
/*-----------------------------------------------------------------*/
class Time {
private:
    unsigned int uboTime;

    float deltaTime;

    float lastFrame;
    
    void UpdateImpl() {
        // Set delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Set time UBO data
        glBindBuffer(GL_UNIFORM_BUFFER, uboTime);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), (void*)&currentFrame);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float), sizeof(float), (void*)&deltaTime);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    float getDeltaTimeImpl() {
        return deltaTime;
    }

    Time() {
        deltaTime = 0.0f;
        lastFrame = 0.0f;

        // Generate time UBO
        glGenBuffers(1, &uboTime);
        glBindBuffer(GL_UNIFORM_BUFFER, uboTime);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboTime, 0, 2 * sizeof(float));
    }

    void FreeImpl() {
        glDeleteBuffers(1, &uboTime);
    }

public:
    Time(const Time&) = delete;
    static Time& getInstance() {
        static Time instance;
        return instance;
    }

    static void Update() {
        getInstance().UpdateImpl();
    }

    static float DeltaTime() {
        return getInstance().getDeltaTimeImpl();
    }

    static void Free() {
        getInstance().FreeImpl();
    }
};

#endif