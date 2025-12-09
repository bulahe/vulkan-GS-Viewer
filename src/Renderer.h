#ifndef RENDERER_H
#define RENDERER_H

#define GLM_SWIZZLE

#include <atomic>
#include "3dgs.h"

#include "vulkan/Window.h"
#include "GSScene.h"
#include "vulkan/pipelines/ComputePipeline.h"
#include "vulkan/Swapchain.h"
#include <glm/gtc/quaternion.hpp>

#include "GUIManager.h"
#include "vulkan/ImguiManager.h"
#include "vulkan/QueryManager.h"
#include "CameraController.h"  // 需要 Camera 类型

class Renderer {
public:
    struct alignas(16) UniformBuffer {
        glm::vec4 camera_position;
        glm::mat4 proj_mat;
        glm::mat4 view_mat;
        glm::mat4 model_mat;  // 模型变换矩阵，用于旋转点云
        uint32_t width;
        uint32_t height;
        float tan_fovx;
        float tan_fovy;
    };

    struct VertexAttributeBuffer {
        glm::vec4 conic_opacity;
        glm::vec4 color_radii;
        glm::uvec4 aabb;
        glm::vec2 uv;
        float depth;
        uint32_t __padding[1];
    };

    // 使用 CameraController 中定义的 Camera 类型
    using Camera = ::Camera;

    struct RadixSortPushConstants {
        uint32_t g_num_elements; // == NUM_ELEMENTS
        uint32_t g_shift; // (*)
        uint32_t g_num_workgroups; // == NUMBER_OF_WORKGROUPS as defined in the section above
        uint32_t g_num_blocks_per_workgroup; // == NUM_BLOCKS_PER_WORKGROUP
    };

    explicit Renderer(VulkanSplatting::RendererConfiguration configuration);

    void createGui();

    void initialize();

    void handleInput();

    void retrieveTimestamps();

    void recreateSwapchain();

    void draw();

    void run();

    void stop();

    ~Renderer();

    Camera camera {
        .position = glm::vec3(0.0f, -1.0f, 0.0f),
        .rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        .fov = 45.0f,
        .nearPlane = 0.1f,
        .farPlane = 1000.0f
    };

    // 自动旋转设置（旋转点云本身，而不是相机）
    bool autoRotate = false;
    float autoRotateSpeed = 0.5f; // 每帧旋转的角度（度）
    glm::quat sceneRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // 场景旋转四元数

private:
    VulkanSplatting::RendererConfiguration configuration;
    std::shared_ptr<Window> window;
    std::shared_ptr<VulkanContext> context;
    std::shared_ptr<ImguiManager> imguiManager;
    std::shared_ptr<GSScene> scene;
    std::shared_ptr<QueryManager> queryManager = std::make_shared<QueryManager>();
    GUIManager guiManager {};
    std::unique_ptr<CameraController> cameraController;

    std::shared_ptr<ComputePipeline> preprocessPipeline;
    std::shared_ptr<ComputePipeline> renderPipeline;
    std::shared_ptr<ComputePipeline> prefixSumPipeline;
    std::shared_ptr<ComputePipeline> preprocessSortPipeline;
    std::shared_ptr<ComputePipeline> sortHistPipeline;
    std::shared_ptr<ComputePipeline> sortPipeline;
    std::shared_ptr<ComputePipeline> tileBoundaryPipeline;

    std::shared_ptr<Buffer> uniformBuffer;
    std::shared_ptr<Buffer> vertexAttributeBuffer;
    std::shared_ptr<Buffer> tileOverlapBuffer;
    std::shared_ptr<Buffer> prefixSumPingBuffer;
    std::shared_ptr<Buffer> prefixSumPongBuffer;
    std::shared_ptr<Buffer> sortKBufferEven;
    std::shared_ptr<Buffer> sortKBufferOdd;
    std::shared_ptr<Buffer> sortHistBuffer;
    std::shared_ptr<Buffer> totalSumBufferHost;
    std::shared_ptr<Buffer> tileBoundaryBuffer;
    std::shared_ptr<Buffer> sortVBufferEven;
    std::shared_ptr<Buffer> sortVBufferOdd;

    std::shared_ptr<DescriptorSet> inputSet;

    std::atomic<bool> running = true;

    std::vector<vk::UniqueFence> inflightFences;

    std::shared_ptr<Swapchain> swapchain;

    vk::UniqueCommandPool commandPool;

    vk::UniqueCommandBuffer preprocessCommandBuffer;
    vk::UniqueCommandBuffer renderCommandBuffer;

    uint32_t currentImageIndex;

    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;

#ifdef __APPLE__
    uint32_t numRadixSortBlocksPerWorkgroup = 256;
#else
    uint32_t numRadixSortBlocksPerWorkgroup = 32;
#endif

    int fpsCounter = 0;
    std::chrono::high_resolution_clock::time_point lastFpsTime = std::chrono::high_resolution_clock::now();

    unsigned int sortBufferSizeMultiplier = 1;

    void initializeVulkan();

    void loadSceneToGPU();

    void createPreprocessPipeline();

    void createPrefixSumPipeline();

    void createRadixSortPipeline();

    void createPreprocessSortPipeline();

    void createTileBoundaryPipeline();

    void createRenderPipeline();

    void recordPreprocessCommandBuffer();

    bool recordRenderCommandBuffer(uint32_t currentFrame);

    void createCommandPool();

    void updateUniforms();
};


#endif //RENDERER_H
