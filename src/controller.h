// controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 摄像机参数
extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

// 模型变换参数
extern glm::vec3 modelPos;
extern glm::vec3 modelRotation;
extern float modelScale;

// 添加舞蹈状态变量
extern bool isDancing;

// 定义插值参数
extern float interpolationFactor;
extern bool isInterpolating;

// 回调函数声明
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

// 时间相关
extern float deltaTime;
extern float lastFrame;

#endif // CONTROLLER_H