#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "model_loader.h"

// 窗口宽高
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// 摄像机参数
glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);

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

    // 加载模型
    Model model("resources/model.obj");

    // 初始化着色器
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 输入处理
        processInput(window);

        // 清空屏幕
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用着色器
        shader.use();

        // 模型矩阵
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, modelPos);
        modelMat = glm::rotate(modelMat, glm::radians(modelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(modelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(modelScale));
        shader.setMat4("model", modelMat);

        // 视图和投影矩阵
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("view", view);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // 渲染模型
        model.draw(shader);

        // 交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理
    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        modelPos.z -= 0.05f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        modelPos.z += 0.05f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        modelPos.x -= 0.05f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        modelPos.x += 0.05f;
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    static float lastX = xpos, lastY = ypos;
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    modelRotation.y += xoffset * sensitivity;
    modelRotation.x += yoffset * sensitivity;
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    modelScale += yoffset * 0.1f;
    if (modelScale < 0.1f)
        modelScale = 0.1f;
}