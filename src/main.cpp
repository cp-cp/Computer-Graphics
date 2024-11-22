#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"
#include "model_loader.h"
#include "stb_image.h"
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
glm::vec3 modelPos(0.0f, -10.0f, 0.0f);
glm::vec3 modelRotation(0.0f, 0.0f, 0.0f);
float modelScale = 0.6f;

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

    // 获取当前帧缓冲区大小
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // 手动调用回调函数
    framebufferSizeCallback(window, width, height);

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 加载模型和着色器
    // Model model("path/to/model.obj");
    // Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    // 加载模型
    Model model("/Users/cp_cp/GitHub/OpenGL/resources/model.obj");
    if (!model.isLoaded()) {
        std::cerr << "Failed to load model from path: /Users/cp_cp/GitHub/OpenGL/resources/model.obj" << std::endl;
    }

    // 初始化着色器
    // Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader shader("/Users/cp_cp/GitHub/OpenGL/shaders/vertex.glsl", "/Users/cp_cp/GitHub/OpenGL/shaders/fragment.glsl");

    // 定义一个大的平面顶点数据
    float planeVertices[] = {
        // 位置          // 法线
        100.0f,
        -10.0f,
        100.0f,
        0.0f,
        1.0f,
        0.0f,
        -100.0f,
        -10.0f,
        100.0f,
        0.0f,
        1.0f,
        0.0f,
        -100.0f,
        -10.0f,
        -100.0f,
        0.0f,
        1.0f,
        0.0f,
        100.0f,
        -10.0f,
        -100.0f,
        0.0f,
        1.0f,
        0.0f,
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
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 设置背景为白色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用着色器
        shader.use();

        // 设置光源属性
        glm::vec3 lightPos(100.0f, 100.0f, 100.0f);                       // 添加光源位置
        shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);   // 光源位置
        shader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z); // 观察者位置
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);                   // 光源颜色
        shader.setVec3("objectColor", 1.0f, 0.5f, 0.2f);                  // 物体颜色

        // 模型矩阵
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, modelPos);                                                // 应用平移
        modelMat = glm::rotate(modelMat, glm::radians(modelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // 绕 X 轴旋转
        modelMat = glm::rotate(modelMat, glm::radians(modelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // 绕 Y 轴旋转
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));                                       // 应用缩放
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

        // 绘制平面
        shader.use();
        glm::mat4 planeModel = glm::mat4(1.0f);
        shader.setMat4("model", planeModel);
        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    } // 清理
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &planeEBO);

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
        modelScale = 0.6f;                            // 复位模型缩放
    }
    // 添加相机复位逻辑
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {                                               // 按下 T 键复位相机
        cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);    // 复位相机位置
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 复位相机朝向
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
        //控制模型旋转
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
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    // static float lastX = xpos, lastY = ypos;
    // float xoffset = xpos - lastX;
    // float yoffset = lastY - ypos;
    // lastX = xpos;
    // lastY = ypos;

    // float sensitivity = 0.1f;                 // 调整灵敏度
    // modelRotation.y += xoffset * sensitivity; // 绕 Y 轴旋转
    // modelRotation.x += yoffset * sensitivity; // 绕 X 轴旋转
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    // modelScale += yoffset * 0.1f; // 根据滚轮滚动调整缩放
    // if (modelScale < 0.1f)
    //     modelScale = 0.1f; // 限制最小缩放值
}

void touchpadCallback(GLFWwindow *window, int touchpadId, int action)
{
    // 忽略触摸板输入
}
