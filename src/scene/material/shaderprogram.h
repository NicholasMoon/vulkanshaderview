#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>

#include "../../core/swapchain.h"
#include "../../render/renderpass.h"
#include "../../image/msaabuffer.h"
#include "../../device/logicaldevice.h"
#include "../../vertex.h"

class RenderPass;
class MSAABuffer;


// problems with circular dependencies
// see primitive.h
// will patch later

#endif