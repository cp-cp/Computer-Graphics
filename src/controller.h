// controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 摄像机参数
extern glm::vec3 cameraPos(0.0f, 3.0f, 30.0f);
extern glm::vec3 cameraFront(0.0f, -0.3f, -1.0f);
extern glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

// 模型变换参数
extern glm::vec3 modelPos(0.0f, -10.0f, 0.0f);
extern glm::vec3 modelRotation(0.0f, 0.0f, 0.0f);
extern float modelScale = 1.6f;

// 添加舞蹈状态变量
extern bool isDancing = false;

// 定义插值参数
extern float interpolationFactor = 0.0f;
extern bool isInterpolating = false;

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

#endif // CONTROLLER_H