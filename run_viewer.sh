#!/bin/bash
# Vulkan SDK environment variables for macOS
export VULKAN_SDK=/Users/bulahe/VulkanSDK/1.4.328.1/macOS
export VK_ICD_FILENAMES=$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json
export VK_LAYER_PATH=$VULKAN_SDK/share/vulkan/explicit_layer.d

# Run the viewer with all arguments passed to this script
./build/apps/viewer/3dgs_viewer "$@"

