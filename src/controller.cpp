#include "controller.h"

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
