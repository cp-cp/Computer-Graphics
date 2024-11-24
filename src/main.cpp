#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include "shader.h"
#include "model_loader.h"
#include "stb_image.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 窗口宽高
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// 摄像机参数
glm::vec3 cameraPos(0.0f, 3.0f, 30.0f);
glm::vec3 cameraFront(0.0f, -0.3f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 模型变换参数
glm::vec3 modelPos(0.0f, -10.0f, 0.0f);
glm::vec3 modelRotation(0.0f, 0.0f, 0.0f);
float modelScale = 0.6f;

// 鼠标参数
bool firstMouse = true;
float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;

// 光源位置
glm::vec3 lightPos(0.0f, 10.0f, 10.0f);

// 定义起始和终止姿态
glm::quat startOrientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));                             // 初始姿态
glm::quat endOrientation = glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(45.0f), 0.0f)); // 终止姿态

// 定义插值参数
float interpolationFactor = 0.0f;
bool isInterpolating = false;

// 添加舞蹈状态变量
bool isDancing = false;

// 全局变量
unsigned int texture1, texture2;

// 回调函数声明
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

void danceMovement(float time)
{
    if (isDancing)
    {
        // 平移
        modelPos.x = 5.0f * sin(1.5f * time);          // X轴移动
        modelPos.y = -10.0f + 2.0f * sin(2.0f * time); // Y轴上下移动
        modelPos.z = 5.0f * cos(1.5f * time);          // Z轴移动

        // 使用���元数进行旋转
        glm::quat rotationX = glm::angleAxis(0.5f * sin(2.0f * time), glm::vec3(1.0f, 0.0f, 0.0f)); // 绕X轴旋转
        glm::quat rotationY = glm::angleAxis(0.5f * cos(2.0f * time), glm::vec3(0.0f, 1.0f, 0.0f)); // 绕Y轴旋转
        glm::quat rotationZ = glm::angleAxis(0.5f * sin(1.0f * time), glm::vec3(0.0f, 0.0f, 1.0f)); // 绕Z轴旋转

        // 组合旋转
        modelRotation = rotationZ * rotationY * rotationX * modelRotation; // 更新模型旋转

        // 缩放变化
        modelScale = 0.6f + 0.1f * sin(1.0f * time); // 根据时间变化缩放
    }
}
// 封装四元数旋转的函数
glm::quat createQuaternionFromEuler(const glm::vec3 &eulerAngles)
{
    return glm::quat(glm::vec3(eulerAngles)); // 根据欧拉角创建四元数
}

void loadTextures()
{
    // 生成纹理
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 设置纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 设置纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载图像，创建纹理并生成 mipmaps
    int width, height, nrChannels;
    unsigned char *data = stbi_load("path/to/your/texture.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

int main()
{
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "3D Model Viewer", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // 设置 OpenGL 视口
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // 获取当前帧缓冲区大小
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // 手动调用回调函数
    framebufferSizeCallback(window, width, height);

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    Model model("/Users/cp_cp/GitHub/OpenGL/resources/model.obj");

    Shader shader("/Users/cp_cp/GitHub/OpenGL/shaders/vertex.glsl", "/Users/cp_cp/GitHub/OpenGL/shaders/fragment.glsl");

    float planeVertices[] = {
        // 位置          // 法线
        100.0f,-10.0f,100.0f,0.0f,1.0f,0.0f,
        -100.0f,-10.0f,100.0f,0.0f,1.0f,0.0f,
        -100.0f,-10.0f,-100.0f,0.0f,1.0f,0.0f,
        100.0f,-10.0f,-100.0f,0.0f,1.0f,0.0f,
    };
    // 定义平面索引
    unsigned int planeIndices[] = {
        0, 1, 2,
        0, 2, 3};

    // 创建平面 VAO 和 VBO
    unsigned int planeVAO, planeVBO, planeEBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 设置 ImGui 样式
    ImGui::StyleColorsClassic();
    // 自定义主题
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;        // 窗口圆角
    style.FrameRounding = 5.0f;         // 控件圆角
    style.GrabRounding = 5.0f;          // 拖动条圆角
    style.ItemSpacing = ImVec2(10, 10); // 控件间距
    style.FramePadding = ImVec2(5, 5);  // 控件内边距

    // 自定义颜色
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);      // 窗口背景色
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);       // 标题背景色
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); // 活动标题背景色
    style.Colors[ImGuiCol_Button] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);        // 按钮背景色
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // 悬停按钮背景色
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);  // 激活按钮背景色

    // 定义前后位置变量
    glm::vec3 newPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 newRotation(0.0f, 0.0f, 0.0f);
    glm::vec3 startPosition = modelPos;      // 初始位置
    glm::vec3 startRotation = modelRotation; // 初始旋转
    float interpolationFactor = 0.0f;        // 插值因子
    bool isInterpolating = false;            // 是否正在插值

    // 加载纹理
    loadTextures();

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 每帧刷新逻辑
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入处理
        processInput(window);

        // 清空屏幕
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 设置背景为白色
        // 设置为灰色
        glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用着色器
        shader.use();

        // 绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        shader.setInt("texture1", 0);

        // 设置光源属性
        shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);   // 光源位置
        shader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z); // 观察者位置
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);                   // 光源颜色
        // shader.setVec3("objectColor", 1.0f, 0.9f, 0.9f);                  // 设置物体表面颜色为灰色

        // 启动新的 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 创建一个窗口并固定位置
        ImGui::SetNextWindowPos(ImVec2(0, height * 0.3), ImGuiCond_Always);                                                         // 固定窗口位置
        ImGui::SetNextWindowSize(ImVec2(width, height * 0.2), ImGuiCond_Always);                                                    // 设置窗口大小
        ImGui::Begin("Position Input", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse); // 禁用移动、调整大小和折叠

        ImGui::InputFloat3("New Position", &newPosition[0]); // 输入新的位置
        ImGui::Text("Model Position: (%.2f, %.2f, %.2f)", modelPos.x, modelPos.y, modelPos.z);
        ImGui::InputFloat3("New Rotation", &newRotation[0]); // 输入新的旋转
        ImGui::Text("Model Front Direction: (%.2f, %.2f, %.2f)", modelRotation.x, modelRotation.y, modelRotation.z);

        // 计算按钮宽度并居中
        float buttonWidth = 100.0f;                                           // 按钮宽度
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.2f); // 设置光标位置为窗口中心
        if (ImGui::Button("Apply", ImVec2(buttonWidth, 0)))                   // 创建按钮
        {
            // 开始插值
            startPosition = modelPos;      // 设置起始位置
            startRotation = modelRotation; // 设置起始旋转
            interpolationFactor = 0.0f;    // 重置插值因子
            isInterpolating = true;        // 开始插值
        }
        ImGui::End();

        float currentTime = glfwGetTime();

        danceMovement(currentTime);

        if (isInterpolating)
        {
            interpolationFactor += deltaTime * 0.5f; // 控制插值速度
            if (interpolationFactor > 1.0f)
            {
                interpolationFactor = 1.0f;
                isInterpolating = false; // 停止插值
            }
            modelPos = glm::mix(startPosition, newPosition, interpolationFactor);      // 使用线性插值
            modelRotation = glm::mix(startRotation, newRotation, interpolationFactor); // 同时对旋转进行线性插值
        }

        // 更新模型矩阵
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, modelPos);                                   // 应用平移
        modelMat = glm::rotate(modelMat, modelRotation.x, glm::vec3(0.01f, 0.0f, 0.0f)); // 应用旋转
        modelMat = glm::rotate(modelMat, modelRotation.y, glm::vec3(0.0f, 0.01f, 0.0f)); // 应用旋转
        modelMat = glm::rotate(modelMat, modelRotation.z, glm::vec3(0.0f, 0.0f, 0.01f)); // 应用旋转
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));                          // 应用缩放

        shader.setMat4("model", modelMat);

        // 视图和投影矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // 在渲染循环中设置模型颜色
        shader.setVec3("objectColor", 1.0f, 0.9f, 0.9f); 

        // 绘制立方体
        model.draw(shader);

        // 绘制平面
        shader.use();
        shader.setVec3("objectColor", 0.7f, 0.8f, 0.9f); // 设置为蓝色
        glm::mat4 planeModel = glm::mat4(1.0f);
        shader.setMat4("model", planeModel);
        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 渲染 ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &planeEBO);

    // 清理 ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float modelMoveSpeed = 0.1;
    float rotateSpeed = 0.2;
    // 添加模型复位逻辑
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {                                                // 按下 R 键复位模型
        modelPos = glm::vec3(0.0f, -10.0f, 0.0f);    // 复位模型位置
        modelRotation = glm::vec3(0.0f, 0.0f, 0.0f); // 复位模型旋转
        modelScale = 0.6f;                           // 复位模型缩放
    }
    // 添加相机复位逻辑
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {                                                // 按下 T 键复位相机
        cameraPos = glm::vec3(0.0f, 3.0f, 30.0f);    // 复位相机位置
        cameraFront = glm::vec3(0.0f, -0.3f, -1.0f); // 复位相机朝向
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)           // 向上移动
        cameraPos += modelMoveSpeed * glm::vec3(0.0, 0.1, 0.0); // 添加向上移动
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)           // 向下移动
        cameraPos -= modelMoveSpeed * glm::vec3(0.0, 0.1, 0.0); // 添加向下移动
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {

        // 控制相机旋转
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // 向上旋转
            cameraFront = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(-rotateSpeed), glm::normalize(glm::cross(cameraUp, cameraFront)))) * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // 向下旋转
            cameraFront = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(rotateSpeed), glm::normalize(glm::cross(cameraUp, cameraFront)))) * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // 向左旋转
            cameraFront = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(-rotateSpeed), cameraUp)) * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // 向右旋转
            cameraFront = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(rotateSpeed), cameraUp)) * cameraFront;
        // 控制模型旋转
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // 向上旋转
            modelRotation.x += rotateSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // 向下旋转
            modelRotation.x -= rotateSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // 向左旋转
            modelRotation.y -= rotateSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // 向右旋转
            modelRotation.y += rotateSpeed;
    }
    else
    {
        // 控制相机移动
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // 向前移动
            cameraPos += modelMoveSpeed * glm::vec3(0.0, 0.0, -0.5);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // 向后移动
            cameraPos -= modelMoveSpeed * glm::vec3(0.0, 0.0, -0.5);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // 向左移动
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * modelMoveSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // 向右移动
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * modelMoveSpeed;

        // 控制模型平移
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // 向上平移
            modelPos.y += modelMoveSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // 向下平移
            modelPos.y -= modelMoveSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // 向左平移
            modelPos.x -= modelMoveSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // 向右平移
            modelPos.x += modelMoveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // 放大模型
        modelScale += 0.01;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // 缩小模型
        modelScale -= 0.01;

    // 开始插值
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        isInterpolating = true;
        interpolationFactor = 0.0f;
    }

    // 按下 O 键触发舞蹈
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        isDancing = !isDancing; // 切换舞蹈状态
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    // modelScale += yoffset * 0.1f; // 根据滚轮滚动调整缩放
    // if (modelScale < 0.1f)
    //     modelScale = 0.1f; // 限制最小缩放值
}
