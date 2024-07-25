#include "Source.h"

unsigned int INIT_SCR_WIDTH = 800;
unsigned int INIT_SCR_HEIGHT = 600;

Camera mainCamera = Camera(INIT_SCR_WIDTH, INIT_SCR_HEIGHT);

int main(){
    //Initialise Window and Input
    GLFWwindow* window = InitialiseWindow();
    if (window == NULL) {
        return -1;
    }

    InitInput(window);

    // Set up framebuffer Shader
    Shader frameBufferShader("Shaders/frameBufferShader.vert", "Shaders/frameBufferShader.frag");
    frameBufferShader.use();
    frameBufferShader.setInt("screenTexture", 0);
    
    // Initialise models
    Model testCube("resources/objects/backpack/backpack.obj");
    BaseMesh& baseMesh = BaseMesh::getInstance();
    unsigned int screenQuadVAO = baseMesh.screenQuadVAO;

    // Setup scene information
    PropertiesSetup();
    mainCamera.InitializeBuffers();
    InitGUI(window);
    Shader* currentShader;

    /* RENDER LOOP */
    while (!glfwWindowShouldClose(window)) {
        // Set up GBuffer
        GBuffer firstPass(mainCamera.GetWidth(), mainCamera.GetHeight());
        glDrawBuffers(2, firstPass.attachments);

        // Update scene information 
        Time::Update();
        processInput(window);
        mainCamera.Update();
        LightManager::Update();

        // First pass
        glBindFramebuffer(GL_FRAMEBUFFER, firstPass.screenBuffer.framebuffer);
        glEnable(GL_DEPTH_TEST);
        SkyBoxManager::SetClearColor();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw backpack
        currentShader = ShadingManager::GetShader();
        currentShader->use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
        currentShader->setMat4("model", model);
        testCube.Draw(*currentShader);

        // Render sky box, lights and post processing
        glDrawBuffers(1, &firstPass.attachments[0]);
        SkyBoxManager::Draw();
        PostProcessingManager::Render(firstPass, mainCamera);
        LightManager::Draw(mainCamera);
        

        // Blit to Screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        frameBufferShader.use();
        glBindVertexArray(screenQuadVAO);
        glBindTexture(GL_TEXTURE_2D, firstPass.screenBuffer.textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        RenderMenu();

        // End render loop
        glfwSwapBuffers(window);
        glfwPollEvents();
        firstPass.deleteGBuffer();
    }

    // Free data 
    baseMesh.freeData();
    frameBufferShader.deleteShader();
    testCube.DeleteModel();
    LightManager::Free();
    Time::Free();
    ShadingManager::Free();
    SkyBoxManager::Free();
    mainCamera.Free();
    PostProcessingManager::Free();

    EndGUI();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/* Initialise GLFW window as well as GLAD */
GLFWwindow* InitialiseWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(INIT_SCR_WIDTH, INIT_SCR_HEIGHT, "Game Title", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to initialise window." << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    // Initialise GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialise glad" << std::endl;
        return NULL;
    }
    glViewport(0, 0, INIT_SCR_WIDTH, INIT_SCR_HEIGHT);
    return window;
}

/* Setup callbacks for input and screen resizing */
void InitInput(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

/* Set properties for opengl drawing */
void PropertiesSetup() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/* Initialise ImGui */
void InitGUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

/* Render scene settings menu */
void RenderMenu() {
    BeginGUIFrame();

    ImGui::Begin("Scene Settings");
    LightManager::Menu();
    ShadingManager::Menu();
    SkyBoxManager::Menu();
    PostProcessingManager::Menu();
    ImGui::End();

    EndGUIFrame();
}

void BeginGUIFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EndGUIFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* Close ImGui */
void EndGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/* Resize window */
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    mainCamera.frameBufferSizeChange(width, height);
}

/* Keyboard input */
void processInput(GLFWwindow* window) {
    mainCamera.ProcessKeyboardInput(window);
}

/* Mouse position input */
void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    mainCamera.ProcessMouseInput(xPos, yPos);
}

/* Mouse middle button input */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    mainCamera.ProcessMouseButtonInput(button, action, mods);
}
