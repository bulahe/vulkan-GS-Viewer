#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "vulkan/Window.h"
#include "GUIManager.h"
#include "3dgs.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// 定义 Camera 结构体（避免依赖 Renderer）
struct Camera {
    glm::vec3 position;
    glm::quat rotation;
    float fov;
    float nearPlane;
    float farPlane;

    void translate(glm::vec3 translation) {
        position += rotation * translation;
    }
};

class CameraController {
public:
    CameraController(std::shared_ptr<Window> window, GUIManager& guiManager, 
                     VulkanSplatting::RendererConfiguration& configuration)
        : window(window), guiManager(guiManager), configuration(configuration) {}

    void handleInput(Camera& camera, bool& autoRotate, float& autoRotateSpeed, glm::quat& sceneRotation);

private:
    std::shared_ptr<Window> window;
    GUIManager& guiManager;
    VulkanSplatting::RendererConfiguration& configuration;
};

#endif //CAMERA_CONTROLLER_H

