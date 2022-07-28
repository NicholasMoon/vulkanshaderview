// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

// STB
#define STB_IMAGE_IMPLEMENTATION

// TINY OBJ
#define TINYOBJLOADER_IMPLEMENTATION

#include "myVK.h"

int main() {
    MyVK app = MyVK();

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}