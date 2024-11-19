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

// 回调函数声明
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

int main()
{
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "3D Model Viewer", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // 设置 OpenGL 视口
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 加载模型
    Model model("/Users/cp_cp/GitHub/OpenGL/resources/model.obj");

    // 初始化着色器
    Shader shader("/Users/cp_cp/GitHub/OpenGL/shaders/vertex.glsl", "/Users/cp_cp/GitHub/OpenGL/shaders/fragment.glsl");

    // 立方体的顶点数据
    float vertices[] = {
        // 位置          // 法线
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
        // 其他面...
    };

    // 创建 VAO 和 VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 检查着色器编译错误
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader.ID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader.ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 清空屏幕
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用着色器
        shader.use();

        // 模型矩阵
        glm::mat4 modelMat = glm::mat4(1.0f);
        shader.setMat4("model", modelMat);

        // 视图和投影矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // 绘制立方体
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 立方体有 36 个顶点
        glBindVertexArray(0);

        // 交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}