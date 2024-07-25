#ifndef LIGHT_H
#define LIGHT_H

#include <custom/Shader.h>
#include <custom/TexUtility.h>
#include <custom/BaseMesh.h>
#include <custom/Camera.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <string>

//Structs for directional and point lights (vectors are sized to work with std140)
struct DirLight {
    glm::vec4 color;
    glm::vec4 direction;
};

struct PointLight {
    glm::vec4 color;
    glm::vec3 position;
    float radius;
};

const int MAX_POINT_LIGHTS = 4;

/*----------------------------------------------------------------------------------------------------*/
/* LIGHT MANAGER: Stores and draws the scene lights, as well as implementing the light settings menu. */
/*----------------------------------------------------------------------------------------------------*/
class LightManager {
private:
    unsigned int uboLights;

    DirLight dirLight = { glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f) };

    bool pointLightsEnabled = false;
    int pointLightCount = 0;
    int pointLightsCreated = 0;
    PointLight pointLights[MAX_POINT_LIGHTS];
    Shader lightShader = Shader("Shaders/PointLightShader.vert", "Shaders/PointLightShader.frag");
    unsigned int quadVAO;
    unsigned int lightTexture = texUtility::loadTexture("Textures/Lights/light.png");
    bool drawLights;

    LightManager()
    {
        BaseMesh& baseMesh = BaseMesh::getInstance();
        this->quadVAO = baseMesh.quadVAO;
        for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
            pointLights[i].color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            pointLights[i].position = glm::vec3(0.0f, 0.0f, 0.0f);
            pointLights[i].radius = 4.0f;
        }
        lightShader.use();
        lightShader.setInt("_MainTex", 0);

        // Generate lights UBO
        glGenBuffers(1, &uboLights);
        glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4) + 2 * MAX_POINT_LIGHTS * sizeof(glm::vec4) + sizeof(int), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 3, uboLights, 0, 4 * sizeof(glm::vec4) + 2 * MAX_POINT_LIGHTS * sizeof(glm::vec4) + sizeof(int));
    }

    void UpdateImpl() {
        // Update lights UBO
        glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(glm::vec4), (void*)&dirLight);
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), 2 * MAX_POINT_LIGHTS * sizeof(glm::vec4), pointLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4) + 2 * MAX_POINT_LIGHTS * sizeof(glm::vec4), sizeof(int), (void*)&pointLightCount);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    /* Draw point lights */
    void DrawImpl(Camera camera) {
        if (drawLights) {
            glm::mat4 model = glm::mat4(1.0);
            glCullFace(GL_FRONT);
            lightShader.use();
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, lightTexture);
            for (int i = 0; i < pointLightCount; i++) {
                model = camera.BillboardMatrix(pointLights[i].position);
                lightShader.setMat4("model", model);
                lightShader.setVec3("color", pointLights[i].color);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            glCullFace(GL_BACK);
            glBindVertexArray(0);
        }
        
    }

    void MenuImpl() {
        if (ImGui::CollapsingHeader("Lighting")) {
            ImGui::Indent(20.0f);
            if (ImGui::CollapsingHeader("Direction Light")) {
                ImGui::Indent(20.0f);
                ImGui::ColorPicker3("Color", glm::value_ptr(dirLight.color));
                if (ImGui::InputFloat3("Direction", glm::value_ptr(dirLight.direction))) {
                    dirLight.direction = glm::normalize(dirLight.direction);
                }
                ImGui::Unindent();
            }
            ImGui::Checkbox("Draw Lights", &drawLights);
            if (ImGui::Checkbox("Enable Point Lights", &pointLightsEnabled)) {
                if (pointLightsEnabled) {
                    pointLightCount = 1;
                }
                else {
                    pointLightCount = 0;
                }
            }
            if (pointLightCount > pointLightsCreated) {
                for (int i = pointLightsCreated; i < pointLightCount; i++) {
                    randomiseLight(pointLights[i]);
                    pointLightsCreated += 1;
                }
            }
            if (pointLightsEnabled) {
                ImGui::Indent(20.0f);
                if (ImGui::Button("Randomise All Point Lights")) {
                    for (int i = 0; i < pointLightCount; i++) {
                        randomiseLight(pointLights[i]);
                    }
                };

                for (int i = 0; i < pointLightCount; i++) {
                    if (ImGui::CollapsingHeader((std::string("Point Light ") + std::to_string(i + 1)).c_str())) {
                        ImGui::Indent(20.0f);
                        ImGui::ColorPicker3((std::string("Color ") + std::to_string(i + 1)).c_str(), glm::value_ptr(pointLights[i].color));
                        ImGui::InputFloat3((std::string("Position ") + std::to_string(i + 1)).c_str(), glm::value_ptr(pointLights[i].position));
                        ImGui::SliderFloat((std::string("Radius ") + std::to_string(i + 1)).c_str(), &pointLights[i].radius, 4.0f, 10.0f);
                        if (ImGui::Button((std::string("Randomise Light ") + std::to_string(i + 1)).c_str())) {
                            randomiseLight(pointLights[i]);
                        }
                        if (ImGui::Button((std::string("Remove Light ") + std::to_string(i + 1)).c_str())) {
                            for (int j = i; j < pointLightCount - 1; j++) {
                                pointLights[j].color = pointLights[j + 1].color;
                                pointLights[j].position = pointLights[j + 1].position;
                                pointLights[j].radius = pointLights[j + 1].radius;
                            }
                            pointLightCount -= 1;
                        }
                        ImGui::Unindent();
                    }
                }
                if (ImGui::Button("Add Point Light") && pointLightCount < MAX_POINT_LIGHTS) {
                    pointLightCount += 1;
                }
                ImGui::Unindent();
            }
            ImGui::Unindent();
        }
    }
    void FreeImpl() {
        lightShader.deleteShader();
        glDeleteBuffers(1, &uboLights);
        glDeleteTextures(1, &lightTexture);
    }

    void randomiseLight(PointLight& light) {
        light.color.x = ((float)rand() / RAND_MAX);
        light.color.y = ((float)rand() / RAND_MAX);
        light.color.z = ((float)rand() / RAND_MAX);

        light.position.x = ((float)rand() / RAND_MAX) * 10;
        light.position.y = ((float)rand() / RAND_MAX) * 10;
        light.position.z = ((float)rand() / RAND_MAX) * 10;

        light.radius = ((float)rand() / RAND_MAX) * 6 + 4;
    }
public:
    LightManager(const LightManager&) = delete;
    static LightManager& getInstance() {
        static LightManager instance;
        return instance;
    }

    static void Update() {
        getInstance().UpdateImpl();
    }

    static void Menu() {
        getInstance().MenuImpl();
    }

    static void Draw(Camera camera) {
        getInstance().DrawImpl(camera);
    }

    static void Free() {
        getInstance().FreeImpl();
    }
};
#endif
