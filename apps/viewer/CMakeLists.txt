cmake_minimum_required(VERSION 3.26)
project(3dgs_viewer)

add_executable(3dgs_viewer main.cpp)

target_include_directories(3dgs_viewer PRIVATE third_party)

target_link_libraries(3dgs_viewer PRIVATE
    libenvpp::libenvpp
    3dgs_cpp)

# On macOS, add Vulkan SDK lib directory to rpath
if(APPLE AND Vulkan_LIBRARY)
    get_filename_component(VULKAN_LIB_DIR ${Vulkan_LIBRARY} DIRECTORY)
    set_target_properties(3dgs_viewer PROPERTIES
        INSTALL_RPATH "${VULKAN_LIB_DIR}"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()