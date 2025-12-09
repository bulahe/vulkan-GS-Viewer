#include "CameraController.h"
#include <spdlog/spdlog.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void CameraController::handleInput(Camera& camera, bool& autoRotate, float& autoRotateSpeed, glm::quat& sceneRotation) {
    auto translation = window->getCursorTranslation();
    auto keys = window->getKeys(); // W, A, S, D

    // 切换自动旋转（R键）
    static bool rKeyPressedLastFrame = false;
    bool rKeyPressed = window->isKeyPressed(GLFW_KEY_R);
    if (rKeyPressed && !rKeyPressedLastFrame) {
        autoRotate = !autoRotate;
        spdlog::info("Auto-rotation: {} (Speed: {:.2f} deg/frame)", autoRotate ? "ON" : "OFF", autoRotateSpeed);
    }
    rKeyPressedLastFrame = rKeyPressed;

    if ((!configuration.enableGui || (!guiManager.wantCaptureMouse() && !guiManager.mouseCapture)) && window->
        getMouseButton()[0]) {
        window->mouseCapture(true);
        guiManager.mouseCapture = true;
    }

    // 自动旋转点云本身（围绕Y轴，即高度轴）
    if (autoRotate) {
        float rotationAngle = glm::radians(autoRotateSpeed);
        sceneRotation = glm::rotate(sceneRotation, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // rotate camera (正常响应鼠标)
    if (!configuration.enableGui || guiManager.mouseCapture) {
        if (translation[0] != 0.0 || translation[1] != 0.0) {
            camera.rotation = glm::rotate(camera.rotation, static_cast<float>(translation[0]) * 0.005f,
                                          glm::vec3(0.0f, -1.0f, 0.0f));
            camera.rotation = glm::rotate(camera.rotation, static_cast<float>(translation[1]) * 0.005f,
                                          glm::vec3(-1.0f, 0.0f, 0.0f));
        }
    }

    // move camera
    if (!configuration.enableGui || !guiManager.wantCaptureKeyboard()) {
        glm::vec3 horizontalDirection = glm::vec3(0.0f, 0.0f, 0.0f);  // 前后左右（相对移动）
        float verticalMovement = 0.0f;  // 上下（绝对移动）
        
        if (keys[0]) {  // W - 向前
            horizontalDirection += glm::vec3(0.0f, 0.0f, -1.0f);
        }
        if (keys[1]) {  // A - 向左
            horizontalDirection += glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        if (keys[2]) {  // S - 向后
            horizontalDirection += glm::vec3(0.0f, 0.0f, 1.0f);
        }
        if (keys[3]) {  // D - 向右
            horizontalDirection += glm::vec3(1.0f, 0.0f, 0.0f);
        }
        if (keys[4]) {  // E - 向上（绝对坐标）
            verticalMovement += 1.0f;
        }
        if (keys[5]) {  // Q - 向下（绝对坐标）
            verticalMovement -= 1.0f;
        }
        if (keys[6]) {
            window->mouseCapture(false);
            guiManager.mouseCapture = false;
        }
        
        // 前后左右使用相机坐标系
        if (horizontalDirection != glm::vec3(0.0f, 0.0f, 0.0f)) {
            horizontalDirection = glm::normalize(horizontalDirection);
            glm::vec4 transformed = glm::mat4_cast(camera.rotation) * glm::vec4(horizontalDirection, 1.0f);
            camera.position += glm::vec3(transformed) * 0.3f;
        }
        
        // 上下使用世界坐标系（绝对坐标）
        if (verticalMovement != 0.0f) {
            camera.position.z += verticalMovement * 0.3f;
        }
    }
}

