#include <Graphics/Vulkan/Context.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    Context::Context(const Display &display) :
        instance(VkInstance_create(display)),
#ifdef DYN_DEBUG
        debugger(VkDebugUtilsMessengerEXT_create(instance)),
#endif
        surface(display.create_vulkan_surface(instance)),
        physical(PhysicalDevice::select_best(instance, surface)),
        device(VkDevice_create(physical)),
        graphics_pool(VkCommandPool_create(device, physical.graphics_queues)),
        transfer_pool(VkCommandPool_create(device, physical.transfer_queues)) {
        vkGetDeviceQueue(device, physical.graphics_queues.index, 0, &graphics_queue);
        vkGetDeviceQueue(device, physical.present_queues.index, 0, &present_queue);
        vkGetDeviceQueue(device, physical.compute_queues.index, 0, &compute_queue);
        vkGetDeviceQueue(device, physical.transfer_queues.index, 0, &transfer_queue);
    }

    Context::~Context() {
        vkDestroyCommandPool(device, graphics_pool, nullptr);
        vkDestroyCommandPool(device, transfer_pool, nullptr);
        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
#ifdef DYN_DEBUG
        vkDestroyDebugUtilsMessengerEXT(instance, debugger, nullptr);
#endif
        vkDestroyInstance(instance, nullptr);
    }
} // namespace Dynamo::Graphics::Vulkan