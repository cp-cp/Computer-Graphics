#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"
#include "model_loader.h"


// 窗口宽高
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// 摄像机参数
glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 模型变换参数
glm::vec3 modelPos(0.0f, 0.0f, 0.0f);
glm::vec3 modelRotation(0.0f, 0.0f, 0.0f);
float modelScale = 1.0f;

// 鼠标参数
bool firstMouse = true;
float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;

// 回调函数声明
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

int main()
{
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "3D Model Viewer", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置 OpenGL 视口
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 加载模型和着色器
    // Model model("path/to/model.obj");
    // Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    // 加载模型
    Model model("/Users/cp_cp/GitHub/OpenGL/resources/model.obj");

    // 初始化着色器
    // Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader shader("/Users/cp_cp/GitHub/OpenGL/shaders/vertex.glsl", "/Users/cp_cp/GitHub/OpenGL/shaders/fragment.glsl");

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 每帧时间逻辑
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入处理
        processInput(window);

        // 清空屏幕
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用着色器
        shader.use();

        // 模型矩阵
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, modelPos); // 应用平移
        modelMat = glm::rotate(modelMat, glm::radians(modelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // 绕 X 轴旋转
        modelMat = glm::rotate(modelMat, glm::radians(modelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // 绕 Y 轴旋转
        modelMat = glm::scale(modelMat, glm::vec3(modelScale)); // 应用缩放
        shader.setMat4("model", modelMat);

        // 视图和投影矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // 在渲染循环中设置模型颜色
        shader.setVec3("objectColor", 1.0f, 0.5f, 0.2f); // 设置为橙色

        // 绘制立方体
        model.draw(shader);

        // 交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

    // 控制相机移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // 向前移动
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // 向后移动
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // 向左移动
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // 向右移动
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    // 控制平移
    float moveSpeed = 0.1f; // 平移速度
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // 使用左箭头键向左平移
        modelPos.x -= moveSpeed; // 减少 x 坐标
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // 使用右箭头键向右平移
        modelPos.x += moveSpeed; // 增加 x 坐标
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // 使用上箭头键向上平移
        modelPos.y += moveSpeed; // 增加 y 坐标
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // 使用下箭头键向下平移
        modelPos.y -= moveSpeed; // 减少 y 坐标
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    static float lastX = xpos, lastY = ypos;
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // 调整灵敏度
    modelRotation.y += xoffset * sensitivity; // 绕 Y 轴旋转
    modelRotation.x += yoffset * sensitivity; // 绕 X 轴旋转
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    modelScale += yoffset * 0.1f; // 根据滚轮滚动调整缩放
    if (modelScale < 0.1f)
        modelScale = 0.1f; // 限制最小缩放值
}