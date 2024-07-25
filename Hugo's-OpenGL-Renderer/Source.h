#include <custom/Camera.h>
#include <custom/Shader.h>
#include <custom/Model.h>
#include <custom/BaseMesh.h>
#include <custom/GBuffer.h>
#include <custom/Time.h>
#include <custom/Light.h>
#include <custom/SkyBoxManager.h>
#include <custom/ShadingManager.h>
#include <custom/PostProcessingManager.h>

#include <GLFW/glfw3.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
GLFWwindow* InitialiseWindow();
void InitInput(GLFWwindow* window);
void PropertiesSetup();
void InitGUI(GLFWwindow* window);
void BeginGUIFrame();
void EndGUIFrame();
void EndGUI();
void RenderMenu();