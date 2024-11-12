#include <Graphics/Vulkan/Utils.hpp>
#include <fstream>

static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebuggerDispatch;
static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebuggerDispatch;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                              const VkDebugUtilsMessengerCreateInfoEXT *create_info,
                                                              const VkAllocationCallbacks *allocator,
                                                              VkDebugUtilsMessengerEXT *messenger) {
    return vkCreateDebuggerDispatch(instance, create_info, allocator, messenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                           VkDebugUtilsMessengerEXT messenger,
                                                           VkAllocationCallbacks const *allocator) {
    vkDestroyDebuggerDispatch(instance, messenger, allocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VkDebugUtilsMessengerEXT_message_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                          VkDebugUtilsMessageTypeFlagsEXT type,
                                          VkDebugUtilsMessengerCallbackDataEXT const *data,
                                          void *user_data) {
    Dynamo::Log::warn("--- Vulkan::Debugger Message ---");
    Dynamo::Log::warn("Message name: {}", data->pMessageIdName);
    Dynamo::Log::warn("Message Id: {}", data->messageIdNumber);

    // Terminate on error
    if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        Dynamo::Log::error(data->pMessage);
    } else {
        Dynamo::Log::warn(data->pMessage);
    }
    Dynamo::Log::warn("");
    return VK_FALSE;
}

namespace Dynamo::Graphics::Vulkan {
    unsigned VkFormat_size(VkFormat format) {
        switch (format) {
        case VK_FORMAT_UNDEFINED:
            return 0;
        case VK_FORMAT_R4G4_UNORM_PACK8:
            return 1;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
            return 2;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
            return 2;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return 2;
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
            return 2;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
            return 2;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
            return 2;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            return 2;
        case VK_FORMAT_R8_UNORM:
            return 1;
        case VK_FORMAT_R8_SNORM:
            return 1;
        case VK_FORMAT_R8_USCALED:
            return 1;
        case VK_FORMAT_R8_SSCALED:
            return 1;
        case VK_FORMAT_R8_UINT:
            return 1;
        case VK_FORMAT_R8_SINT:
            return 1;
        case VK_FORMAT_R8_SRGB:
            return 1;
        case VK_FORMAT_R8G8_UNORM:
            return 2;
        case VK_FORMAT_R8G8_SNORM:
            return 2;
        case VK_FORMAT_R8G8_USCALED:
            return 2;
        case VK_FORMAT_R8G8_SSCALED:
            return 2;
        case VK_FORMAT_R8G8_UINT:
            return 2;
        case VK_FORMAT_R8G8_SINT:
            return 2;
        case VK_FORMAT_R8G8_SRGB:
            return 2;
        case VK_FORMAT_R8G8B8_UNORM:
            return 3;
        case VK_FORMAT_R8G8B8_SNORM:
            return 3;
        case VK_FORMAT_R8G8B8_USCALED:
            return 3;
        case VK_FORMAT_R8G8B8_SSCALED:
            return 3;
        case VK_FORMAT_R8G8B8_UINT:
            return 3;
        case VK_FORMAT_R8G8B8_SINT:
            return 3;
        case VK_FORMAT_R8G8B8_SRGB:
            return 3;
        case VK_FORMAT_B8G8R8_UNORM:
            return 3;
        case VK_FORMAT_B8G8R8_SNORM:
            return 3;
        case VK_FORMAT_B8G8R8_USCALED:
            return 3;
        case VK_FORMAT_B8G8R8_SSCALED:
            return 3;
        case VK_FORMAT_B8G8R8_UINT:
            return 3;
        case VK_FORMAT_B8G8R8_SINT:
            return 3;
        case VK_FORMAT_B8G8R8_SRGB:
            return 3;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return 4;
        case VK_FORMAT_R8G8B8A8_SNORM:
            return 4;
        case VK_FORMAT_R8G8B8A8_USCALED:
            return 4;
        case VK_FORMAT_R8G8B8A8_SSCALED:
            return 4;
        case VK_FORMAT_R8G8B8A8_UINT:
            return 4;
        case VK_FORMAT_R8G8B8A8_SINT:
            return 4;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return 4;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return 4;
        case VK_FORMAT_B8G8R8A8_SNORM:
            return 4;
        case VK_FORMAT_B8G8R8A8_USCALED:
            return 4;
        case VK_FORMAT_B8G8R8A8_SSCALED:
            return 4;
        case VK_FORMAT_B8G8R8A8_UINT:
            return 4;
        case VK_FORMAT_B8G8R8A8_SINT:
            return 4;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return 4;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            return 4;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
            return 4;
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
            return 4;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
            return 4;
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
            return 4;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
            return 4;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            return 4;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            return 4;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
            return 4;
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
            return 4;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
            return 4;
        case VK_FORMAT_A2R10G10B10_UINT_PACK32:
            return 4;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32:
            return 4;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return 4;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
            return 4;
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
            return 4;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
            return 4;
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
            return 4;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
            return 4;
        case VK_FORMAT_R16_UNORM:
            return 2;
        case VK_FORMAT_R16_SNORM:
            return 2;
        case VK_FORMAT_R16_USCALED:
            return 2;
        case VK_FORMAT_R16_SSCALED:
            return 2;
        case VK_FORMAT_R16_UINT:
            return 2;
        case VK_FORMAT_R16_SINT:
            return 2;
        case VK_FORMAT_R16_SFLOAT:
            return 2;
        case VK_FORMAT_R16G16_UNORM:
            return 4;
        case VK_FORMAT_R16G16_SNORM:
            return 4;
        case VK_FORMAT_R16G16_USCALED:
            return 4;
        case VK_FORMAT_R16G16_SSCALED:
            return 4;
        case VK_FORMAT_R16G16_UINT:
            return 4;
        case VK_FORMAT_R16G16_SINT:
            return 4;
        case VK_FORMAT_R16G16_SFLOAT:
            return 4;
        case VK_FORMAT_R16G16B16_UNORM:
            return 6;
        case VK_FORMAT_R16G16B16_SNORM:
            return 6;
        case VK_FORMAT_R16G16B16_USCALED:
            return 6;
        case VK_FORMAT_R16G16B16_SSCALED:
            return 6;
        case VK_FORMAT_R16G16B16_UINT:
            return 6;
        case VK_FORMAT_R16G16B16_SINT:
            return 6;
        case VK_FORMAT_R16G16B16_SFLOAT:
            return 6;
        case VK_FORMAT_R16G16B16A16_UNORM:
            return 8;
        case VK_FORMAT_R16G16B16A16_SNORM:
            return 8;
        case VK_FORMAT_R16G16B16A16_USCALED:
            return 8;
        case VK_FORMAT_R16G16B16A16_SSCALED:
            return 8;
        case VK_FORMAT_R16G16B16A16_UINT:
            return 8;
        case VK_FORMAT_R16G16B16A16_SINT:
            return 8;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return 8;
        case VK_FORMAT_R32_UINT:
            return 4;
        case VK_FORMAT_R32_SINT:
            return 4;
        case VK_FORMAT_R32_SFLOAT:
            return 4;
        case VK_FORMAT_R32G32_UINT:
            return 8;
        case VK_FORMAT_R32G32_SINT:
            return 8;
        case VK_FORMAT_R32G32_SFLOAT:
            return 8;
        case VK_FORMAT_R32G32B32_UINT:
            return 12;
        case VK_FORMAT_R32G32B32_SINT:
            return 12;
        case VK_FORMAT_R32G32B32_SFLOAT:
            return 12;
        case VK_FORMAT_R32G32B32A32_UINT:
            return 16;
        case VK_FORMAT_R32G32B32A32_SINT:
            return 16;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return 16;
        case VK_FORMAT_R64_UINT:
            return 8;
        case VK_FORMAT_R64_SINT:
            return 8;
        case VK_FORMAT_R64_SFLOAT:
            return 8;
        case VK_FORMAT_R64G64_UINT:
            return 16;
        case VK_FORMAT_R64G64_SINT:
            return 16;
        case VK_FORMAT_R64G64_SFLOAT:
            return 16;
        case VK_FORMAT_R64G64B64_UINT:
            return 24;
        case VK_FORMAT_R64G64B64_SINT:
            return 24;
        case VK_FORMAT_R64G64B64_SFLOAT:
            return 24;
        case VK_FORMAT_R64G64B64A64_UINT:
            return 32;
        case VK_FORMAT_R64G64B64A64_SINT:
            return 32;
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            return 32;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            return 4;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            return 4;
        default:
            Log::error("Invalid VkFormat size.");
            return 0;
        }
    }

    const char *VkFormat_string(VkFormat format) {
        switch (format) {
        case VK_FORMAT_UNDEFINED:
            return "VK_FORMAT_UNDEFINED";
        case VK_FORMAT_R4G4_UNORM_PACK8:
            return "VK_FORMAT_R4G4_UNORM_PACK8";
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
            return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
            return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return "VK_FORMAT_R5G6B5_UNORM_PACK16";
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
            return "VK_FORMAT_B5G6R5_UNORM_PACK16";
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
            return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
            return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
        case VK_FORMAT_R8_UNORM:
            return "VK_FORMAT_R8_UNORM";
        case VK_FORMAT_R8_SNORM:
            return "VK_FORMAT_R8_SNORM";
        case VK_FORMAT_R8_USCALED:
            return "VK_FORMAT_R8_USCALED";
        case VK_FORMAT_R8_SSCALED:
            return "VK_FORMAT_R8_SSCALED";
        case VK_FORMAT_R8_UINT:
            return "VK_FORMAT_R8_UINT";
        case VK_FORMAT_R8_SINT:
            return "VK_FORMAT_R8_SINT";
        case VK_FORMAT_R8_SRGB:
            return "VK_FORMAT_R8_SRGB";
        case VK_FORMAT_R8G8_UNORM:
            return "VK_FORMAT_R8G8_UNORM";
        case VK_FORMAT_R8G8_SNORM:
            return "VK_FORMAT_R8G8_SNORM";
        case VK_FORMAT_R8G8_USCALED:
            return "VK_FORMAT_R8G8_USCALED";
        case VK_FORMAT_R8G8_SSCALED:
            return "VK_FORMAT_R8G8_SSCALED";
        case VK_FORMAT_R8G8_UINT:
            return "VK_FORMAT_R8G8_UINT";
        case VK_FORMAT_R8G8_SINT:
            return "VK_FORMAT_R8G8_SINT";
        case VK_FORMAT_R8G8_SRGB:
            return "VK_FORMAT_R8G8_SRGB";
        case VK_FORMAT_R8G8B8_UNORM:
            return "VK_FORMAT_R8G8B8_UNORM";
        case VK_FORMAT_R8G8B8_SNORM:
            return "VK_FORMAT_R8G8B8_SNORM";
        case VK_FORMAT_R8G8B8_USCALED:
            return "VK_FORMAT_R8G8B8_USCALED";
        case VK_FORMAT_R8G8B8_SSCALED:
            return "VK_FORMAT_R8G8B8_SSCALED";
        case VK_FORMAT_R8G8B8_UINT:
            return "VK_FORMAT_R8G8B8_UINT";
        case VK_FORMAT_R8G8B8_SINT:
            return "VK_FORMAT_R8G8B8_SINT";
        case VK_FORMAT_R8G8B8_SRGB:
            return "VK_FORMAT_R8G8B8_SRGB";
        case VK_FORMAT_B8G8R8_UNORM:
            return "VK_FORMAT_B8G8R8_UNORM";
        case VK_FORMAT_B8G8R8_SNORM:
            return "VK_FORMAT_B8G8R8_SNORM";
        case VK_FORMAT_B8G8R8_USCALED:
            return "VK_FORMAT_B8G8R8_USCALED";
        case VK_FORMAT_B8G8R8_SSCALED:
            return "VK_FORMAT_B8G8R8_SSCALED";
        case VK_FORMAT_B8G8R8_UINT:
            return "VK_FORMAT_B8G8R8_UINT";
        case VK_FORMAT_B8G8R8_SINT:
            return "VK_FORMAT_B8G8R8_SINT";
        case VK_FORMAT_B8G8R8_SRGB:
            return "VK_FORMAT_B8G8R8_SRGB";
        case VK_FORMAT_R8G8B8A8_UNORM:
            return "VK_FORMAT_R8G8B8A8_UNORM";
        case VK_FORMAT_R8G8B8A8_SNORM:
            return "VK_FORMAT_R8G8B8A8_SNORM";
        case VK_FORMAT_R8G8B8A8_USCALED:
            return "VK_FORMAT_R8G8B8A8_USCALED";
        case VK_FORMAT_R8G8B8A8_SSCALED:
            return "VK_FORMAT_R8G8B8A8_SSCALED";
        case VK_FORMAT_R8G8B8A8_UINT:
            return "VK_FORMAT_R8G8B8A8_UINT";
        case VK_FORMAT_R8G8B8A8_SINT:
            return "VK_FORMAT_R8G8B8A8_SINT";
        case VK_FORMAT_R8G8B8A8_SRGB:
            return "VK_FORMAT_R8G8B8A8_SRGB";
        case VK_FORMAT_B8G8R8A8_UNORM:
            return "VK_FORMAT_B8G8R8A8_UNORM";
        case VK_FORMAT_B8G8R8A8_SNORM:
            return "VK_FORMAT_B8G8R8A8_SNORM";
        case VK_FORMAT_B8G8R8A8_USCALED:
            return "VK_FORMAT_B8G8R8A8_USCALED";
        case VK_FORMAT_B8G8R8A8_SSCALED:
            return "VK_FORMAT_B8G8R8A8_SSCALED";
        case VK_FORMAT_B8G8R8A8_UINT:
            return "VK_FORMAT_B8G8R8A8_UINT";
        case VK_FORMAT_B8G8R8A8_SINT:
            return "VK_FORMAT_B8G8R8A8_SINT";
        case VK_FORMAT_B8G8R8A8_SRGB:
            return "VK_FORMAT_B8G8R8A8_SRGB";
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
            return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
            return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
            return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
            return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
            return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
            return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
            return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
            return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
        case VK_FORMAT_A2R10G10B10_UINT_PACK32:
            return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
        case VK_FORMAT_A2R10G10B10_SINT_PACK32:
            return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
            return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
            return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
            return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
            return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
            return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
        case VK_FORMAT_R16_UNORM:
            return "VK_FORMAT_R16_UNORM";
        case VK_FORMAT_R16_SNORM:
            return "VK_FORMAT_R16_SNORM";
        case VK_FORMAT_R16_USCALED:
            return "VK_FORMAT_R16_USCALED";
        case VK_FORMAT_R16_SSCALED:
            return "VK_FORMAT_R16_SSCALED";
        case VK_FORMAT_R16_UINT:
            return "VK_FORMAT_R16_UINT";
        case VK_FORMAT_R16_SINT:
            return "VK_FORMAT_R16_SINT";
        case VK_FORMAT_R16_SFLOAT:
            return "VK_FORMAT_R16_SFLOAT";
        case VK_FORMAT_R16G16_UNORM:
            return "VK_FORMAT_R16G16_UNORM";
        case VK_FORMAT_R16G16_SNORM:
            return "VK_FORMAT_R16G16_SNORM";
        case VK_FORMAT_R16G16_USCALED:
            return "VK_FORMAT_R16G16_USCALED";
        case VK_FORMAT_R16G16_SSCALED:
            return "VK_FORMAT_R16G16_SSCALED";
        case VK_FORMAT_R16G16_UINT:
            return "VK_FORMAT_R16G16_UINT";
        case VK_FORMAT_R16G16_SINT:
            return "VK_FORMAT_R16G16_SINT";
        case VK_FORMAT_R16G16_SFLOAT:
            return "VK_FORMAT_R16G16_SFLOAT";
        case VK_FORMAT_R16G16B16_UNORM:
            return "VK_FORMAT_R16G16B16_UNORM";
        case VK_FORMAT_R16G16B16_SNORM:
            return "VK_FORMAT_R16G16B16_SNORM";
        case VK_FORMAT_R16G16B16_USCALED:
            return "VK_FORMAT_R16G16B16_USCALED";
        case VK_FORMAT_R16G16B16_SSCALED:
            return "VK_FORMAT_R16G16B16_SSCALED";
        case VK_FORMAT_R16G16B16_UINT:
            return "VK_FORMAT_R16G16B16_UINT";
        case VK_FORMAT_R16G16B16_SINT:
            return "VK_FORMAT_R16G16B16_SINT";
        case VK_FORMAT_R16G16B16_SFLOAT:
            return "VK_FORMAT_R16G16B16_SFLOAT";
        case VK_FORMAT_R16G16B16A16_UNORM:
            return "VK_FORMAT_R16G16B16A16_UNORM";
        case VK_FORMAT_R16G16B16A16_SNORM:
            return "VK_FORMAT_R16G16B16A16_SNORM";
        case VK_FORMAT_R16G16B16A16_USCALED:
            return "VK_FORMAT_R16G16B16A16_USCALED";
        case VK_FORMAT_R16G16B16A16_SSCALED:
            return "VK_FORMAT_R16G16B16A16_SSCALED";
        case VK_FORMAT_R16G16B16A16_UINT:
            return "VK_FORMAT_R16G16B16A16_UINT";
        case VK_FORMAT_R16G16B16A16_SINT:
            return "VK_FORMAT_R16G16B16A16_SINT";
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return "VK_FORMAT_R16G16B16A16_SFLOAT";
        case VK_FORMAT_R32_UINT:
            return "VK_FORMAT_R32_UINT";
        case VK_FORMAT_R32_SINT:
            return "VK_FORMAT_R32_SINT";
        case VK_FORMAT_R32_SFLOAT:
            return "VK_FORMAT_R32_SFLOAT";
        case VK_FORMAT_R32G32_UINT:
            return "VK_FORMAT_R32G32_UINT";
        case VK_FORMAT_R32G32_SINT:
            return "VK_FORMAT_R32G32_SINT";
        case VK_FORMAT_R32G32_SFLOAT:
            return "VK_FORMAT_R32G32_SFLOAT";
        case VK_FORMAT_R32G32B32_UINT:
            return "VK_FORMAT_R32G32B32_UINT";
        case VK_FORMAT_R32G32B32_SINT:
            return "VK_FORMAT_R32G32B32_SINT";
        case VK_FORMAT_R32G32B32_SFLOAT:
            return "VK_FORMAT_R32G32B32_SFLOAT";
        case VK_FORMAT_R32G32B32A32_UINT:
            return "VK_FORMAT_R32G32B32A32_UINT";
        case VK_FORMAT_R32G32B32A32_SINT:
            return "VK_FORMAT_R32G32B32A32_SINT";
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return "VK_FORMAT_R32G32B32A32_SFLOAT";
        case VK_FORMAT_R64_UINT:
            return "VK_FORMAT_R64_UINT";
        case VK_FORMAT_R64_SINT:
            return "VK_FORMAT_R64_SINT";
        case VK_FORMAT_R64_SFLOAT:
            return "VK_FORMAT_R64_SFLOAT";
        case VK_FORMAT_R64G64_UINT:
            return "VK_FORMAT_R64G64_UINT";
        case VK_FORMAT_R64G64_SINT:
            return "VK_FORMAT_R64G64_SINT";
        case VK_FORMAT_R64G64_SFLOAT:
            return "VK_FORMAT_R64G64_SFLOAT";
        case VK_FORMAT_R64G64B64_UINT:
            return "VK_FORMAT_R64G64B64_UINT";
        case VK_FORMAT_R64G64B64_SINT:
            return "VK_FORMAT_R64G64B64_SINT";
        case VK_FORMAT_R64G64B64_SFLOAT:
            return "VK_FORMAT_R64G64B64_SFLOAT";
        case VK_FORMAT_R64G64B64A64_UINT:
            return "VK_FORMAT_R64G64B64A64_UINT";
        case VK_FORMAT_R64G64B64A64_SINT:
            return "VK_FORMAT_R64G64B64A64_SINT";
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            return "VK_FORMAT_R64G64B64A64_SFLOAT";
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
        case VK_FORMAT_D16_UNORM:
            return "VK_FORMAT_D16_UNORM";
        case VK_FORMAT_X8_D24_UNORM_PACK32:
            return "VK_FORMAT_X8_D24_UNORM_PACK32";
        case VK_FORMAT_D32_SFLOAT:
            return "VK_FORMAT_D32_SFLOAT";
        case VK_FORMAT_S8_UINT:
            return "VK_FORMAT_S8_UINT";
        case VK_FORMAT_D16_UNORM_S8_UINT:
            return "VK_FORMAT_D16_UNORM_S8_UINT";
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return "VK_FORMAT_D24_UNORM_S8_UINT";
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return "VK_FORMAT_D32_SFLOAT_S8_UINT";
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
            return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
            return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
            return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
        case VK_FORMAT_BC2_UNORM_BLOCK:
            return "VK_FORMAT_BC2_UNORM_BLOCK";
        case VK_FORMAT_BC2_SRGB_BLOCK:
            return "VK_FORMAT_BC2_SRGB_BLOCK";
        case VK_FORMAT_BC3_UNORM_BLOCK:
            return "VK_FORMAT_BC3_UNORM_BLOCK";
        case VK_FORMAT_BC3_SRGB_BLOCK:
            return "VK_FORMAT_BC3_SRGB_BLOCK";
        case VK_FORMAT_BC4_UNORM_BLOCK:
            return "VK_FORMAT_BC4_UNORM_BLOCK";
        case VK_FORMAT_BC4_SNORM_BLOCK:
            return "VK_FORMAT_BC4_SNORM_BLOCK";
        case VK_FORMAT_BC5_UNORM_BLOCK:
            return "VK_FORMAT_BC5_UNORM_BLOCK";
        case VK_FORMAT_BC5_SNORM_BLOCK:
            return "VK_FORMAT_BC5_SNORM_BLOCK";
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:
            return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:
            return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
        case VK_FORMAT_BC7_UNORM_BLOCK:
            return "VK_FORMAT_BC7_UNORM_BLOCK";
        case VK_FORMAT_BC7_SRGB_BLOCK:
            return "VK_FORMAT_BC7_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:
            return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
        case VK_FORMAT_EAC_R11_SNORM_BLOCK:
            return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
            return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
            return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
        case VK_FORMAT_G8B8G8R8_422_UNORM:
            return "VK_FORMAT_G8B8G8R8_422_UNORM";
        case VK_FORMAT_B8G8R8G8_422_UNORM:
            return "VK_FORMAT_B8G8R8G8_422_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM";
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
            return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM";
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
            return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM";
        case VK_FORMAT_R10X6_UNORM_PACK16:
            return "VK_FORMAT_R10X6_UNORM_PACK16";
        case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
            return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16";
        case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
            return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
        case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
            return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
        case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
            return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
        case VK_FORMAT_R12X4_UNORM_PACK16:
            return "VK_FORMAT_R12X4_UNORM_PACK16";
        case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
            return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16";
        case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
            return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
        case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
            return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
        case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
            return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
        case VK_FORMAT_G16B16G16R16_422_UNORM:
            return "VK_FORMAT_G16B16G16R16_422_UNORM";
        case VK_FORMAT_B16G16R16G16_422_UNORM:
            return "VK_FORMAT_B16G16R16G16_422_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM";
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
            return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM";
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
            return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM";
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT";
        case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT:
            return "VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT";
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT";
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT";
        case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT:
            return "VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT";
        case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
            return "VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT";
        case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
            return "VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT";
        case VK_FORMAT_MAX_ENUM:
            return "VK_FORMAT_MAX_ENUM";
        default:
            return "";
        }
    }

    const char *VkSampleCountFlagBits_string(VkSampleCountFlagBits samples) {
        switch (samples) {
        case VK_SAMPLE_COUNT_1_BIT:
            return "VK_SAMPLE_COUNT_1_BIT";
        case VK_SAMPLE_COUNT_2_BIT:
            return "VK_SAMPLE_COUNT_2_BIT";
        case VK_SAMPLE_COUNT_4_BIT:
            return "VK_SAMPLE_COUNT_4_BIT";
        case VK_SAMPLE_COUNT_8_BIT:
            return "VK_SAMPLE_COUNT_8_BIT";
        case VK_SAMPLE_COUNT_16_BIT:
            return "VK_SAMPLE_COUNT_16_BIT";
        case VK_SAMPLE_COUNT_32_BIT:
            return "VK_SAMPLE_COUNT_32_BIT";
        case VK_SAMPLE_COUNT_64_BIT:
            return "VK_SAMPLE_COUNT_64_BIT";
        case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM:
            return "VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM";
        }
    }

    const char *VkPhysicalDeviceType_string(VkPhysicalDeviceType type) {
        switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "VK_PHYSICAL_DEVICE_TYPE_OTHER";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "VK_PHYSICAL_DEVICE_TYPE_CPU";
        case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
            return "VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM";
        default:
            return "";
        }
    }

    const char *VkShaderStageFlagBits_string(VkShaderStageFlagBits stage) {
        switch (stage) {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return "VK_SHADER_STAGE_VERTEX_BIT";
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT";
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT";
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return "VK_SHADER_STAGE_GEOMETRY_BIT";
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return "VK_SHADER_STAGE_FRAGMENT_BIT";
        case VK_SHADER_STAGE_COMPUTE_BIT:
            return "VK_SHADER_STAGE_COMPUTE_BIT";
        case VK_SHADER_STAGE_ALL_GRAPHICS:
            return "VK_SHADER_STAGE_ALL_GRAPHICS";
        case VK_SHADER_STAGE_ALL:
            return "VK_SHADER_STAGE_ALL";
        case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
            return "VK_SHADER_STAGE_RAYGEN_BIT_KHR";
        case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
            return "VK_SHADER_STAGE_ANY_HIT_BIT_KHR";
        case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
            return "VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR";
        case VK_SHADER_STAGE_MISS_BIT_KHR:
            return "VK_SHADER_STAGE_MISS_BIT_KHR";
        case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
            return "VK_SHADER_STAGE_INTERSECTION_BIT_KHR";
        case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
            return "VK_SHADER_STAGE_CALLABLE_BIT_KHR";
        case VK_SHADER_STAGE_TASK_BIT_NV:
            return "VK_SHADER_STAGE_TASK_BIT_NV";
        case VK_SHADER_STAGE_MESH_BIT_NV:
            return "VK_SHADER_STAGE_MESH_BIT_NV";
        case VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI:
            return "VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI";
        default:
            return "";
        }
    }

    const char *VkDescriptorType_string(VkDescriptorType type) {
        switch (type) {
        case VK_DESCRIPTOR_TYPE_SAMPLER:
            return "VK_DESCRIPTOR_TYPE_SAMPLER";
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
        case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
        case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
        case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
        case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT:
            return "VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT";
        case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR";
        case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV:
            return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV";
        case VK_DESCRIPTOR_TYPE_MUTABLE_VALVE:
            return "VK_DESCRIPTOR_TYPE_MUTABLE_VALVE";
        case VK_DESCRIPTOR_TYPE_MAX_ENUM:
            return "VK_DESCRIPTOR_TYPE_MAX_ENUM";
        default:
            return "";
        }
    }

    const char *VkResult_string(VkResult result) {
        switch (result) {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:
            return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:
            return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_NOT_PERMITTED_EXT:
            return "VK_ERROR_NOT_PERMITTED_EXT";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
            return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR:
            return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR:
            return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR:
            return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR:
            return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_PIPELINE_COMPILE_REQUIRED_EXT:
            return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
        case VK_RESULT_MAX_ENUM:
            return "VK_RESULT_MAX_ENUM";
        default:
            return "";
        }
    }

    const char *VkImageLayout_string(VkImageLayout layout) {
        switch (layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return "VK_IMAGE_LAYOUT_UNDEFINED";
        case VK_IMAGE_LAYOUT_GENERAL:
            return "VK_IMAGE_LAYOUT_GENERAL";
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
            return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL";
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL";
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            return "VK_IMAGE_LAYOUT_PREINITIALIZED";
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
            return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
            return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
            return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
            return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
            return "VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
            return "VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            return "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
        case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
            return "VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR";
        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
            return "VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT";
        case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
            return "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR";
        case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR:
            return "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR";
        case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR:
            return "VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR";
        case VK_IMAGE_LAYOUT_MAX_ENUM:
            return "VK_IMAGE_LAYOUT_MAX_ENUM";
        }
    }

    void VkResult_check(const std::string &op_message, VkResult result) {
        if (result != VK_SUCCESS) {
            Log::error("Vulkan {}: {}", op_message, VkResult_string(result));
        }
    }

    VkPolygonMode convert_fill(Fill fill) {
        switch (fill) {
        case Fill::Point:
            return VK_POLYGON_MODE_POINT;
        case Fill::Line:
            return VK_POLYGON_MODE_LINE;
        case Fill::Face:
            return VK_POLYGON_MODE_FILL;
        }
    }

    VkCullModeFlags convert_cull(Cull cull) {
        switch (cull) {
        case Cull::None:
            return VK_CULL_MODE_NONE;
        case Cull::Back:
            return VK_CULL_MODE_BACK_BIT;
        case Cull::Front:
            return VK_CULL_MODE_FRONT_BIT;
        }
    }

    VkPrimitiveTopology convert_topology(Topology topology) {
        switch (topology) {
        case Topology::Point:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case Topology::Triangle:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case Topology::Line:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        }
    }

    VkCompareOp convert_compare_op(CompareOp op) {
        switch (op) {
        case CompareOp::Always:
            return VK_COMPARE_OP_ALWAYS;
        case CompareOp::Never:
            return VK_COMPARE_OP_NEVER;
        case CompareOp::Equal:
            return VK_COMPARE_OP_EQUAL;
        case CompareOp::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::Less:
            return VK_COMPARE_OP_LESS;
        case CompareOp::LessEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::Greater:
            return VK_COMPARE_OP_GREATER;
        case CompareOp::GreaterEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        }
    }

    VkFormat convert_texture_format(TextureFormat format,
                                    const VkSurfaceFormatKHR &surface_format,
                                    VkFormat physical_depth_format) {
        switch (format) {
        case TextureFormat::R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;
        case TextureFormat::R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case TextureFormat::R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;
        case TextureFormat::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case TextureFormat::ColorSurface:
            // Depends on the swapchain state
            return surface_format.format;
        case TextureFormat::DepthSurface:
            // Depends on the physical device features
            return physical_depth_format;
        }
    }

    VkFilter convert_texture_filter(TextureFilter filter) {
        switch (filter) {
        case TextureFilter::Nearest:
            return VK_FILTER_NEAREST;
        case TextureFilter::Linear:
            return VK_FILTER_LINEAR;
        }
    }

    VkSamplerMipmapMode convert_texture_mipmap_filter(TextureFilter filter) {
        switch (filter) {
        case TextureFilter::Nearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case TextureFilter::Linear:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }

    VkSamplerAddressMode convert_texture_address_mode(TextureAddressMode address_mode) {
        switch (address_mode) {
        case TextureAddressMode::Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case TextureAddressMode::RepeatMirror:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case TextureAddressMode::Clamp:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case TextureAddressMode::ClampMirror:
            return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        case TextureAddressMode::ClampBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        }
    }

    VkSampleCountFlagBits convert_texture_samples(TextureSamples samples) {
        switch (samples) {
        case TextureSamples::S1:
            return VK_SAMPLE_COUNT_1_BIT;
        case TextureSamples::S2:
            return VK_SAMPLE_COUNT_2_BIT;
        case TextureSamples::S4:
            return VK_SAMPLE_COUNT_4_BIT;
        case TextureSamples::S8:
            return VK_SAMPLE_COUNT_8_BIT;
        case TextureSamples::S16:
            return VK_SAMPLE_COUNT_16_BIT;
        case TextureSamples::S32:
            return VK_SAMPLE_COUNT_32_BIT;
        case TextureSamples::S64:
            return VK_SAMPLE_COUNT_64_BIT;
        }
    }

    VkInstance VkInstance_create(const Display &display) {
        std::vector<const char *> extensions = display.get_vulkan_extensions();

        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = display.get_title().c_str();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Dynamo";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo instance_info = {};
        instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_info.pApplicationInfo = &app_info;

// Enable validation layers if debug mode is on
#ifdef DYN_DEBUG
        const char *layer = "VK_LAYER_KHRONOS_validation";
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        instance_info.enabledLayerCount = 1;
        instance_info.ppEnabledLayerNames = &layer;
#else
        instance_info.enabledLayerCount = 0;
        instance_info.ppEnabledLayerNames = nullptr;
#endif

        instance_info.enabledExtensionCount = extensions.size();
        instance_info.ppEnabledExtensionNames = extensions.data();

        Log::info("Required Vulkan extensions:");
        for (const char *ext : extensions) {
            Log::info("* {}", ext);
        }
        Log::info("");

        VkInstance instance;
        VkResult_check("Create Instance", vkCreateInstance(&instance_info, nullptr, &instance));
        return instance;
    }

    VkDebugUtilsMessengerEXT VkDebugUtilsMessengerEXT_create(VkInstance instance) {
        vkCreateDebuggerDispatch = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebuggerDispatch = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        // Create the debug messenger
        VkDebugUtilsMessengerCreateInfoEXT debugger_info = {};
        debugger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugger_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debugger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugger_info.pfnUserCallback = &VkDebugUtilsMessengerEXT_message_callback;

        VkDebugUtilsMessengerEXT debugger;
        VkResult_check("Create Debugger", vkCreateDebugUtilsMessengerEXT(instance, &debugger_info, nullptr, &debugger));
        return debugger;
    }

    VkDevice VkDevice_create(const PhysicalDevice &physical) {
        // Enable physical device features
        VkPhysicalDeviceFeatures device_features = {};
        device_features.samplerAnisotropy = true;
        device_features.sampleRateShading = true;
        device_features.fillModeNonSolid = true;
        device_features.multiViewport = true;

        // Enable descriptor indexing features
        VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing = {};
        descriptor_indexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descriptor_indexing.descriptorBindingPartiallyBound = true;
        descriptor_indexing.runtimeDescriptorArray = true;
        descriptor_indexing.descriptorBindingVariableDescriptorCount = true;

        std::vector<QueueFamilyRef> queue_families = physical.unique_queue_families();
        std::vector<const char *> extensions = physical.required_extensions();

        std::vector<VkDeviceQueueCreateInfo> queue_infos;
        for (const QueueFamily &family : queue_families) {
            VkDeviceQueueCreateInfo queue_info = {};
            queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info.queueCount = family.count;
            queue_info.queueFamilyIndex = family.index;
            queue_info.pQueuePriorities = family.priorities.data();
            queue_infos.push_back(queue_info);
        }

        VkDeviceCreateInfo device_info = {};
        device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_info.queueCreateInfoCount = queue_infos.size();
        device_info.pQueueCreateInfos = queue_infos.data();
        device_info.enabledExtensionCount = extensions.size();
        device_info.ppEnabledExtensionNames = extensions.data();
        device_info.pEnabledFeatures = &device_features;
        device_info.pNext = &descriptor_indexing;

        VkDevice device;
        VkResult_check("Create Device", vkCreateDevice(physical.handle, &device_info, nullptr, &device));
        return device;
    }

    VkDeviceMemory VkDeviceMemory_allocate(VkDevice device, unsigned type_index, VkDeviceSize size) {
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.memoryTypeIndex = type_index;
        alloc_info.allocationSize = size;

        VkDeviceMemory memory;
        VkResult_check("Allocate Memory", vkAllocateMemory(device, &alloc_info, nullptr, &memory));
        return memory;
    }

    VkBuffer VkBuffer_create(VkDevice device,
                             VkBufferUsageFlags usage,
                             VkDeviceSize size,
                             const QueueFamily *queue_families,
                             unsigned queue_family_count) {
        std::vector<unsigned> family_indices;
        for (unsigned i = 0; i < queue_family_count; i++) {
            family_indices.push_back(queue_families[i].index);
        }

        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.usage = usage;
        buffer_info.size = size;
        buffer_info.queueFamilyIndexCount = family_indices.size();
        buffer_info.pQueueFamilyIndices = family_indices.data();

        if (family_indices.size() > 1) {
            buffer_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        } else {
            buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        VkBuffer buffer;
        VkResult_check("Create Buffer", vkCreateBuffer(device, &buffer_info, nullptr, &buffer));
        return buffer;
    }

    VkImage VkImage_create(VkDevice device,
                           const VkExtent3D &extent,
                           VkFormat format,
                           VkImageLayout layout,
                           VkImageType type,
                           VkImageTiling tiling,
                           VkImageUsageFlags usage,
                           VkSampleCountFlagBits samples,
                           VkImageCreateFlags flags,
                           unsigned mip_levels,
                           unsigned array_layers,
                           const QueueFamily *queue_families,
                           unsigned queue_family_count) {
        std::vector<unsigned> family_indices;
        for (unsigned i = 0; i < queue_family_count; i++) {
            family_indices.push_back(queue_families[i].index);
        }

        VkImageCreateInfo image_info = {};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.flags = flags;
        image_info.extent = extent;
        image_info.format = format;
        image_info.imageType = type;
        image_info.usage = usage;
        image_info.initialLayout = layout;
        image_info.samples = samples;
        image_info.mipLevels = mip_levels;
        image_info.arrayLayers = array_layers;
        image_info.queueFamilyIndexCount = family_indices.size();
        image_info.pQueueFamilyIndices = family_indices.data();

        if (family_indices.size() > 1) {
            image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        } else {
            image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        VkImage image;
        VkResult_check("Create Image", vkCreateImage(device, &image_info, nullptr, &image));
        return image;
    }

    void VkImage_transition_layout(VkImage image,
                                   VkCommandBuffer command_buffer,
                                   VkImageLayout prev,
                                   VkImageLayout next,
                                   const VkImageSubresourceRange &subresources) {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.oldLayout = prev;
        barrier.newLayout = next;
        barrier.subresourceRange = subresources;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        VkPipelineStageFlags src_stage = 0;
        VkPipelineStageFlags dst_stage = 0;

        if (prev == VK_IMAGE_LAYOUT_UNDEFINED && next == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (prev == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && next == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            Log::error("Vulkan unsupported image layout transition: {} -> {}",
                       VkImageLayout_string(prev),
                       VkImageLayout_string(next));
        }

        vkCmdPipelineBarrier(command_buffer, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    VkImageView VkImageView_create(VkDevice device,
                                   VkImage image,
                                   VkFormat format,
                                   VkImageViewType type,
                                   const VkImageSubresourceRange &subresources,
                                   const VkComponentMapping &swizzle) {
        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.format = format;
        view_info.viewType = type;
        view_info.components = swizzle;
        view_info.subresourceRange = subresources;

        VkImageView view;
        VkResult_check("Create ImageView", vkCreateImageView(device, &view_info, nullptr, &view));
        return view;
    }

    VkSampler VkSampler_create(VkDevice device,
                               VkSamplerAddressMode address_mode_u,
                               VkSamplerAddressMode address_mode_v,
                               VkSamplerAddressMode address_mode_w,
                               VkFilter mag_filter,
                               VkFilter min_filter,
                               VkSamplerMipmapMode mipmap_mode,
                               VkBorderColor border_color,
                               float max_anisotropy,
                               unsigned mip_levels) {
        VkSamplerCreateInfo sampler_info = {};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.addressModeU = address_mode_u;
        sampler_info.addressModeV = address_mode_v;
        sampler_info.addressModeW = address_mode_w;
        sampler_info.magFilter = mag_filter;
        sampler_info.minFilter = min_filter;
        sampler_info.borderColor = border_color;
        sampler_info.maxAnisotropy = max_anisotropy;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

        sampler_info.mipmapMode = mipmap_mode;
        sampler_info.mipLodBias = 0;
        sampler_info.minLod = 0;
        sampler_info.maxLod = mip_levels;

        VkSampler sampler;
        VkResult_check("Create Sampler", vkCreateSampler(device, &sampler_info, nullptr, &sampler));
        return sampler;
    }

    VkDescriptorSetLayout VkDescriptorSetLayout_create(VkDevice device,
                                                       const VkDescriptorSetLayoutBinding *bindings,
                                                       unsigned binding_count) {
        VkDescriptorSetLayoutCreateInfo layout_info = {};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = binding_count;
        layout_info.pBindings = bindings;

        VkDescriptorSetLayout vk_layout;
        VkResult_check("Create Descriptor Set Layout",
                       vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &vk_layout));
        return vk_layout;
    }

    VkPipelineLayout VkPipelineLayout_create(VkDevice device,
                                             VkDescriptorSetLayout *descriptor_layouts,
                                             unsigned descriptor_layout_count,
                                             VkPushConstantRange *push_constant_ranges,
                                             unsigned push_constant_range_count) {
        VkPipelineLayoutCreateInfo layout_info = {};
        layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layout_info.setLayoutCount = descriptor_layout_count;
        layout_info.pSetLayouts = descriptor_layouts;
        layout_info.pushConstantRangeCount = push_constant_range_count;
        layout_info.pPushConstantRanges = push_constant_ranges;

        VkPipelineLayout layout;
        VkResult_check("Create Pipeline Layout", vkCreatePipelineLayout(device, &layout_info, nullptr, &layout));
        return layout;
    }

    VkShaderModule VkShaderModule_create(VkDevice device, const std::vector<uint32_t> &bytecode) {
        VkShaderModuleCreateInfo shader_info = {};
        shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_info.pCode = bytecode.data();
        shader_info.codeSize = bytecode.size() * sizeof(uint32_t);

        VkShaderModule shader;
        VkResult_check("Create Shader Module", vkCreateShaderModule(device, &shader_info, nullptr, &shader));
        return shader;
    }

    VkRenderPass VkRenderPass_create(VkDevice device,
                                     VkSampleCountFlagBits samples,
                                     VkFormat color_format,
                                     VkFormat depth_stencil_format) {
        VkAttachmentDescription color = {};
        color.format = color_format;
        color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color.samples = samples;
        color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        if (samples != VK_SAMPLE_COUNT_1_BIT) {
            color.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        VkAttachmentDescription depth_stencil = {};
        depth_stencil.format = depth_stencil_format;
        depth_stencil.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_stencil.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_stencil.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_stencil.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_stencil.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_stencil.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_stencil.samples = samples;

        VkAttachmentDescription color_resolve = {};
        color_resolve.format = color_format;
        color_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        color_resolve.samples = VK_SAMPLE_COUNT_1_BIT;

        VkAttachmentReference color_ref = {};
        color_ref.attachment = 0;
        color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_stencil_ref = {};
        depth_stencil_ref.attachment = 1;
        depth_stencil_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference color_resolve_ref = {};
        color_resolve_ref.attachment = 2;
        color_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_ref;
        subpass.pResolveAttachments = nullptr;
        subpass.pDepthStencilAttachment = &depth_stencil_ref;
        if (samples != VK_SAMPLE_COUNT_1_BIT) {
            subpass.pResolveAttachments = &color_resolve_ref;
        }

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        // If the sample count is 1, we ignore the color resolve attachment
        std::array<VkAttachmentDescription, 3> attachments = {
            color,
            depth_stencil,
            color_resolve,
        };
        VkRenderPassCreateInfo renderpass_info = {};
        renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpass_info.attachmentCount = samples != VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
        renderpass_info.pAttachments = attachments.data();
        renderpass_info.subpassCount = 1;
        renderpass_info.pSubpasses = &subpass;
        renderpass_info.dependencyCount = 1;
        renderpass_info.pDependencies = &dependency;

        VkRenderPass renderpass;
        VkResult result = vkCreateRenderPass(device, &renderpass_info, nullptr, &renderpass);
        VkResult_check("Create Render Pass", result);
        return renderpass;
    }

    VkPipelineCache VkPipelineCache_create(VkDevice device, const std::string &filename) {
        std::ifstream ifstream;
        ifstream.open(filename, std::ios::app | std::ios::binary);
        ifstream.seekg(0, ifstream.end);
        size_t size = ifstream.tellg();
        ifstream.seekg(0, ifstream.beg);
        std::vector<char> buffer(size);
        ifstream.read(buffer.data(), size);
        ifstream.close();

        VkPipelineCacheCreateInfo cache_info;
        cache_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        cache_info.initialDataSize = size;
        cache_info.pInitialData = buffer.data();

        VkPipelineCache cache;
        VkResult_check("Create Pipeline Cache", vkCreatePipelineCache(device, &cache_info, nullptr, &cache));
        return cache;
    }

    VkPipeline VkPipeline_create(VkDevice device,
                                 VkPipelineCache cache,
                                 VkPipelineLayout layout,
                                 VkRenderPass renderpass,
                                 VkShaderModule vertex,
                                 VkShaderModule fragment,
                                 VkPrimitiveTopology topology,
                                 VkPolygonMode fill,
                                 VkCullModeFlags cull,
                                 VkSampleCountFlagBits samples,
                                 VkColorComponentFlags color_mask,
                                 bool depth_test,
                                 bool depth_write,
                                 VkCompareOp depth_test_op,
                                 const VkVertexInputBindingDescription *vertex_bindings,
                                 unsigned vertex_binding_count,
                                 const VkVertexInputAttributeDescription *vertex_attributes,
                                 unsigned vertex_attribute_count) {
        VkGraphicsPipelineCreateInfo pipeline_info = {};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        // Dynamic pipeline states that need to be set during command recording
        std::array<VkDynamicState, 4> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_BLEND_CONSTANTS,
            VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        };
        VkPipelineDynamicStateCreateInfo dynamic = {};
        dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic.dynamicStateCount = dynamic_states.size();
        dynamic.pDynamicStates = dynamic_states.data();
        pipeline_info.pDynamicState = &dynamic;

        // Programmable shader stages
        std::array<VkPipelineShaderStageCreateInfo, 2> stages;

        VkPipelineShaderStageCreateInfo vertex_stage_info = {};
        vertex_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_stage_info.module = vertex;
        vertex_stage_info.pName = "main";
        stages[0] = vertex_stage_info;

        VkPipelineShaderStageCreateInfo fragment_stage_info = {};
        fragment_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_stage_info.module = fragment;
        fragment_stage_info.pName = "main";
        stages[1] = fragment_stage_info;

        pipeline_info.stageCount = stages.size();
        pipeline_info.pStages = stages.data();

        // Dynamic viewport state
        VkPipelineViewportStateCreateInfo viewport = {};
        viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport.viewportCount = 1;
        viewport.scissorCount = 1;
        pipeline_info.pViewportState = &viewport;

        // Vertex input
        VkPipelineVertexInputStateCreateInfo input = {};
        input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        input.vertexBindingDescriptionCount = vertex_binding_count;
        input.pVertexBindingDescriptions = vertex_bindings;
        input.vertexAttributeDescriptionCount = vertex_attribute_count;
        input.pVertexAttributeDescriptions = vertex_attributes;
        pipeline_info.pVertexInputState = &input;

        // Input assembly
        VkPipelineInputAssemblyStateCreateInfo assembly = {};
        assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assembly.topology = topology;
        assembly.primitiveRestartEnable = VK_FALSE;
        pipeline_info.pInputAssemblyState = &assembly;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.polygonMode = fill;
        rasterizer.cullMode = cull;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.lineWidth = 1;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0;
        rasterizer.depthBiasClamp = 0;
        rasterizer.depthBiasSlopeFactor = 0;
        pipeline_info.pRasterizationState = &rasterizer;

        // Multisampling
        VkPipelineMultisampleStateCreateInfo ms = {};
        ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        ms.sampleShadingEnable = VK_FALSE;
        ms.rasterizationSamples = samples;
        ms.minSampleShading = 1;
        ms.pSampleMask = nullptr;
        ms.alphaToCoverageEnable = VK_FALSE;
        ms.alphaToOneEnable = VK_FALSE;
        pipeline_info.pMultisampleState = &ms;

        // Color blending
        VkPipelineColorBlendAttachmentState blend_attachment = {};
        blend_attachment.colorWriteMask = color_mask;
        // TODO: Custom blends
        blend_attachment.blendEnable = VK_TRUE;
        blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo blend = {};
        blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blend.attachmentCount = 1;
        blend.pAttachments = &blend_attachment;
        blend.logicOpEnable = VK_FALSE;
        blend.logicOp = VK_LOGIC_OP_COPY;
        pipeline_info.pColorBlendState = &blend;

        // Depth and stencil testing
        VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable = depth_test;
        depth_stencil.depthWriteEnable = depth_write;
        depth_stencil.depthCompareOp = depth_test_op;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.minDepthBounds = 0;
        depth_stencil.maxDepthBounds = 1;

        // TODO: Stencil op state customization
        VkStencilOpState stencil_op = {};
        depth_stencil.stencilTestEnable = VK_FALSE;
        depth_stencil.front = stencil_op;
        depth_stencil.back = stencil_op;
        pipeline_info.pDepthStencilState = &depth_stencil;

        // Renderpass and layout
        pipeline_info.subpass = 0;
        pipeline_info.renderPass = renderpass;
        pipeline_info.layout = layout;

        // As pipeline derivation
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_info.basePipelineIndex = 0;

        // Build and cache
        VkPipeline pipeline;
        VkResult result = vkCreateGraphicsPipelines(device, cache, 1, &pipeline_info, nullptr, &pipeline);
        VkResult_check("Create Graphics Pipeline", result);
        return pipeline;
    }

    VkFramebuffer VkFramebuffer_create(VkDevice device,
                                       VkRenderPass renderpass,
                                       const VkExtent2D &extent,
                                       const VkImageView *views,
                                       unsigned view_count,
                                       unsigned layer_count) {
        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = renderpass;
        framebuffer_info.attachmentCount = view_count;
        framebuffer_info.pAttachments = views;
        framebuffer_info.width = extent.width;
        framebuffer_info.height = extent.height;
        framebuffer_info.layers = layer_count;

        VkFramebuffer framebuffer;
        VkResult_check("Create Framebuffer", vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer));
        return framebuffer;
    }

    VkCommandPool VkCommandPool_create(VkDevice device, QueueFamily family) {
        VkCommandPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = family.index;

        VkCommandPool pool;
        VkResult_check("Create Command Pool", vkCreateCommandPool(device, &pool_info, nullptr, &pool));
        return pool;
    }

    void VkCommandBuffer_allocate(VkDevice device,
                                  VkCommandPool pool,
                                  VkCommandBufferLevel level,
                                  VkCommandBuffer *dst,
                                  unsigned count) {
        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = pool;
        alloc_info.level = level;
        alloc_info.commandBufferCount = count;

        VkResult_check("Allocate Command Buffers", vkAllocateCommandBuffers(device, &alloc_info, dst));
    }

    VkDescriptorPool VkDescriptorPool_create(VkDevice device,
                                             const VkDescriptorPoolSize *sizes,
                                             unsigned size_count,
                                             unsigned max_sets) {
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = size_count;
        pool_info.pPoolSizes = sizes;
        pool_info.maxSets = max_sets;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

        VkDescriptorPool pool;
        VkResult_check("Create Descriptor Pool", vkCreateDescriptorPool(device, &pool_info, nullptr, &pool));
        return pool;
    }

    void VkDescriptorSet_allocate(VkDevice device,
                                  VkDescriptorPool pool,
                                  const VkDescriptorSetLayout *layouts,
                                  VkDescriptorSet *dst,
                                  unsigned count) {
        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = pool;
        alloc_info.descriptorSetCount = count;
        alloc_info.pSetLayouts = layouts;

        VkResult_check("Allocate Descriptor Sets", vkAllocateDescriptorSets(device, &alloc_info, dst));
    }

    void VkCommandBuffer_immediate_start(VkCommandBuffer command_buffer) {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);
    }

    void VkCommandBuffer_immediate_end(VkCommandBuffer command_buffer, VkQueue queue) {
        // Submit the command to the queue
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkEndCommandBuffer(command_buffer);
        vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);
    }

    VkFence VkFence_create(VkDevice device) {
        VkFenceCreateInfo fence_info = {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkFence fence;
        VkResult_check("Create Fence", vkCreateFence(device, &fence_info, nullptr, &fence));
        return fence;
    }

    VkSemaphore VkSemaphore_create(VkDevice device) {
        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkSemaphore semaphore;
        VkResult_check("Create Semaphore", vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore));
        return semaphore;
    }
} // namespace Dynamo::Graphics::Vulkan