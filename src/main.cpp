#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <stdint.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>

typedef uint32_t u32;
constexpr u32 nullval = 4294967295;

void process_input(GLFWwindow* window);

const u32 window_width = 800;
const u32 window_height = 600;

const std::vector<const char*> validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool validation_layers_enabled = false;
#else
const bool validation_layers_enabled = true;
#endif

VKAPI_ATTR VkBool32 VKAPI_CALL debug_message_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		message_severity,
	VkDebugUtilsMessageTypeFlagsEXT				message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* user_data)
{
	std::cerr << "validation layer: " << callback_data->pMessage << std::endl;

	return VK_FALSE;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "vulkan", nullptr, nullptr);
	if (!window)
	{
		std::cout << "could not create glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}

	VkInstance vulkan_instance;
	VkApplicationInfo application_specification{};
	VkApplicationInfo appInfo{};
	application_specification.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	application_specification.pApplicationName = "Hello Triangle";
	application_specification.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	application_specification.pEngineName = "No Engine";
	application_specification.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	application_specification.apiVersion = VK_API_VERSION_1_0;


	VkInstanceCreateInfo instance_specification{};
	instance_specification.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_specification.pApplicationInfo = &appInfo;

	u32 glfw_extension_count = 0;
	const char** glfw_required_extensions;
	glfw_required_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<const char*> required_extensions(glfw_required_extensions, glfw_required_extensions + glfw_extension_count);
	if (validation_layers_enabled)
	{
		required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	std::cout << "required glfw extensions:\n";
	for (int i = 0; i < glfw_extension_count; i++)
	{
		std::cout << '\t' << glfw_required_extensions[i] << std::endl;
	}

	instance_specification.enabledExtensionCount = (u32)required_extensions.size();
	instance_specification.ppEnabledExtensionNames = required_extensions.data();


	VkDebugUtilsMessengerCreateInfoEXT debug_messenger_specification{};
	if (validation_layers_enabled)
	{
		instance_specification.enabledLayerCount = (u32)validation_layers.size();
		instance_specification.ppEnabledLayerNames = validation_layers.data();

		debug_messenger_specification.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_messenger_specification.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debug_messenger_specification.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_messenger_specification.pfnUserCallback = debug_message_callback;
		debug_messenger_specification.pUserData = nullptr;

		instance_specification.pNext = &debug_messenger_specification;
	}
	else
	{
		instance_specification.enabledLayerCount = 0;
		instance_specification.pNext = nullptr;
	}

	u32 extension_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data());

	std::cout << "available extensions:\n";
	for (const auto& extension : available_extensions)
	{
		std::cout << '\t' << extension.extensionName << std::endl;
	}

	bool all_required_extensions_supported = true;
	for (int i = 0; i < glfw_extension_count; i++)
	{
		bool found_required_extension = false;
		for (const auto& extension : available_extensions) {
			if (strcmp(glfw_required_extensions[i], extension.extensionName) == 0)
			{
				found_required_extension = true;
				break;
			}
		}
		if (!found_required_extension)
		{
			all_required_extensions_supported = false;
			std::cout << "unsupported required glfw extension: " << glfw_required_extensions[i] << std::endl;
		}
	}
	if (all_required_extensions_supported)
	{
		std::cout << "GLFW EXTENSION SUPPORT SUCCESSFULLY VALIDATED" << std::endl;
	}

	uint32_t validation_layer_count;
	vkEnumerateInstanceLayerProperties(&validation_layer_count, nullptr);

	std::vector<VkLayerProperties> available_validation_layers(validation_layer_count);
	vkEnumerateInstanceLayerProperties(&validation_layer_count, available_validation_layers.data());

	bool all_required_layers_supported = true;
	for (const char* layer : validation_layers)
	{
		bool found_required_layer = false;
		for (const auto& layer_properties : available_validation_layers) {
			if (strcmp(layer, layer_properties.layerName) == 0)
			{
				found_required_layer = true;
				break;
			}
		}
		if (!found_required_layer)
		{
			all_required_layers_supported = false;
			std::cout << "unsupported required layer: " << layer << std::endl;
		}
	}
	if (all_required_layers_supported)
	{
		std::cout << "VALIDATION LAYER SUPPORT SUCCESSFULLY VALIDATED" << std::endl;
	}

	if (vkCreateInstance(&instance_specification, nullptr, &vulkan_instance) != VK_SUCCESS) {
		std::cout << "failed to create vulkan instance!" << std::endl;
		return -1;
	}

	VkDebugUtilsMessengerEXT debug_messenger;
	if (validation_layers_enabled)
	{
		auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan_instance, "vkCreateDebugUtilsMessengerEXT");
		if (vkCreateDebugUtilsMessengerEXT) {
			vkCreateDebugUtilsMessengerEXT(vulkan_instance, &debug_messenger_specification, nullptr, &debug_messenger);
		}
		else {
			std::cout << "vkCreateDebugUtilsMessengerEXT could not be loaded!" << std::endl;
			return -1;
		}
	}

	VkSurfaceKHR surface;
#if 0

	VkWin32SurfaceCreateInfoKHR surface_specification{};
	surface_specification.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_specification.hwnd = glfwGetWin32Window(window);
	surface_specification.hinstance = GetModuleHandle(nullptr);

	if (vkCreateWin32SurfaceKHR(vulkan_instance, &surface_specification, nullptr, &surface) != VK_SUCCESS)
	{
		std::cout << "failed to create window surface!" << std::endl;
		return -1;
	}
#else
	if (glfwCreateWindowSurface(vulkan_instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		std::cout << "failed to create window surface!" << std::endl;
		return -1;
	}
#endif



	VkPhysicalDevice physical_device = VK_NULL_HANDLE;

	u32 device_count = 0;
	vkEnumeratePhysicalDevices(vulkan_instance, &device_count, nullptr);
	if (device_count < 1)
	{
		std::cout << "could not find GPU with Vulkan support!" << std::endl;
		return -1;
	}

	struct queue_family_indices
	{
		u32 graphics_family = nullval;
		u32 present_family = nullval;
	};
	queue_family_indices indices;

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(vulkan_instance, &device_count, devices.data());
	for (const auto& device : devices)
	{
		u32 queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

		std::cout << "queue family count: " << queue_family_count << std::endl;

		int i = 0;
		for (const auto& family : queue_families)
		{
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphics_family = i;
			}

			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

			if (present_support)
			{
				indices.present_family = i;
			}

			if (indices.graphics_family != nullval && indices.present_family != nullval)
			{
				break;
			}

			i++;
		}

		if (indices.graphics_family != nullval)
		{
			physical_device = device;
			break;
		}
	}

	u32 extension_count_set;
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count_set, nullptr);

	std::vector<VkExtensionProperties> available_extensions_set(extension_count_set);
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count_set, available_extensions_set.data());

	std::set<std::string> required_extensions_set(device_extensions.begin(), device_extensions.end());

	for (const auto& extension : available_extensions_set)
	{
		required_extensions_set.erase(extension.extensionName);
	}

	struct swap_chain_support_details
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	swap_chain_support_details swap_chain_support;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &swap_chain_support.capabilities);

	u32 format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);

	if (format_count != 0)
	{
		swap_chain_support.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, swap_chain_support.formats.data());
	}

	u32 present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);

	if (present_mode_count != 0)
	{
		swap_chain_support.present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, swap_chain_support.present_modes.data());
	}


	VkSurfaceFormatKHR preferred_format;
	for (const auto& available_format : swap_chain_support.formats)
	{
		if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			preferred_format = available_format;
			break;
		}
	}
	if (!&preferred_format)
	{
		std::cout << "failed to select preferred surface format!" << std::endl;
		return -1;
	}

	VkPresentModeKHR preferred_present_mode;
	for (const auto& available_present_mode : swap_chain_support.present_modes)
	{
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			preferred_present_mode = available_present_mode;
			break;
		}
	}
	if (swap_chain_support.present_modes.size() < 1)
	{
		std::cout << "present modes unavailable!" << std::endl;
		return -1;
	}
	if (!&preferred_present_mode)
	{
		preferred_present_mode = swap_chain_support.present_modes[0];
		std::cout << "mailbox mode unsupported, selecting default instead: MODE" << preferred_present_mode << std::endl;
	}
	if (!&preferred_present_mode)
	{
		std::cout << "failed to select preferred present mode!" << std::endl;
		return -1;
	}

	if (physical_device == VK_NULL_HANDLE || !required_extensions_set.empty() || swap_chain_support.formats.empty() || swap_chain_support.present_modes.empty())
	{
		std::cout << "failed to find a suitable GPU!" << std::endl;
		return -1;
	}
	else
	{
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(physical_device, &device_properties);
		std::cout << "GPU: " << device_properties.deviceName << std::endl;
	}
	if (indices.graphics_family == nullval)
	{
		std::cout << "graphics family uninitialized!" << std::endl;
		return -1;
	}
	if (indices.present_family == nullval)
	{
		std::cout << "graphics family uninitialized!" << std::endl;
		return -1;
	}
	VkDevice device;

	std::vector<VkDeviceQueueCreateInfo> queue_specification_vector;
	std::set<u32> unique_queue_families = { indices.graphics_family, indices.present_family };

	float queue_priority = 1.0f;
	for (u32 queue_family : unique_queue_families)
	{
		VkDeviceQueueCreateInfo queue_specification{};
		queue_specification.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_specification.queueFamilyIndex = indices.graphics_family;
		queue_specification.queueCount = 1;
		queue_specification.pQueuePriorities = &queue_priority;
		queue_specification_vector.push_back(queue_specification);
	}

	VkPhysicalDeviceFeatures device_features{};

	VkDeviceCreateInfo device_specification{};
	device_specification.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_specification.queueCreateInfoCount = (u32)queue_specification_vector.size();
	device_specification.pQueueCreateInfos = queue_specification_vector.data();
	device_specification.pEnabledFeatures = &device_features;
	device_specification.enabledExtensionCount = (u32)device_extensions.size();
	device_specification.ppEnabledExtensionNames = device_extensions.data();

	if (validation_layers_enabled)
	{
		device_specification.enabledLayerCount = (u32)validation_layers.size();
		device_specification.ppEnabledLayerNames = validation_layers.data();
	}
	else
	{
		device_specification.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physical_device, &device_specification, nullptr, &device) != VK_SUCCESS)
	{
		std::cout << "failed to create logical device!" << std::endl;
		return -1;
	}

	VkQueue graphics_queue;
	VkQueue present_queue;

	vkGetDeviceQueue(device, indices.graphics_family, 0, &graphics_queue);
	vkGetDeviceQueue(device, indices.present_family, 0, &present_queue);

	VkExtent2D preferred_swap_extent;
	if (swap_chain_support.capabilities.currentExtent.width != nullval)
	{
		preferred_swap_extent = swap_chain_support.capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actual_extent = {
			(u32)width,
			(u32)height
		};

		actual_extent.width = std::clamp(actual_extent.width, swap_chain_support.capabilities.minImageExtent.width, swap_chain_support.capabilities.maxImageExtent.width);
		actual_extent.height = std::clamp(actual_extent.height, swap_chain_support.capabilities.minImageExtent.height, swap_chain_support.capabilities.maxImageExtent.height);

		preferred_swap_extent = actual_extent;
	}

	u32 image_count = swap_chain_support.capabilities.minImageCount + 1;

	if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
		image_count = swap_chain_support.capabilities.minImageCount;

	VkSwapchainCreateInfoKHR swap_chain_specification{};
	swap_chain_specification.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swap_chain_specification.surface = surface;
	swap_chain_specification.minImageCount = image_count;
	swap_chain_specification.imageFormat = preferred_format.format;
	swap_chain_specification.imageColorSpace = preferred_format.colorSpace;
	swap_chain_specification.imageExtent = preferred_swap_extent;
	swap_chain_specification.imageArrayLayers = 1;
	swap_chain_specification.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (indices.graphics_family != indices.present_family)
	{
		swap_chain_specification.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swap_chain_specification.queueFamilyIndexCount = sizeof(queue_family_indices) / 4;
		swap_chain_specification.pQueueFamilyIndices = &indices.graphics_family;
	}
	else
	{
		swap_chain_specification.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swap_chain_specification.queueFamilyIndexCount = 0;
		swap_chain_specification.pQueueFamilyIndices = nullptr;
	}

	swap_chain_specification.preTransform = swap_chain_support.capabilities.currentTransform;
	swap_chain_specification.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swap_chain_specification.presentMode = preferred_present_mode;
	swap_chain_specification.clipped = VK_TRUE;
	swap_chain_specification.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR swap_chain;
	std::vector<VkImage> swap_chain_images;
	VkFormat swap_chain_image_format;
	VkExtent2D swap_chain_extent;

	if (vkCreateSwapchainKHR(device, &swap_chain_specification, nullptr, &swap_chain) != VK_SUCCESS)
	{
		std::cout << "failed to create swap chain!" << std::endl;
		return -1;
	}

	vkGetSwapchainImagesKHR(device, swap_chain, &image_count, nullptr);
	swap_chain_images.resize(image_count);
	vkGetSwapchainImagesKHR(device, swap_chain, &image_count, swap_chain_images.data());

	swap_chain_image_format = preferred_format.format;
	swap_chain_extent = preferred_swap_extent;


	std::vector<VkImageView> swap_chain_image_views(swap_chain_images.size());

	for (size_t i = 0; i < swap_chain_images.size(); i++) {
		VkImageViewCreateInfo image_view_specification{};
		image_view_specification.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_specification.image = swap_chain_images[i];
		image_view_specification.viewType = VK_IMAGE_VIEW_TYPE_2D;
		image_view_specification.format = swap_chain_image_format;
		image_view_specification.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_specification.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_specification.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_specification.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_specification.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_specification.subresourceRange.baseMipLevel = 0;
		image_view_specification.subresourceRange.levelCount = 1;
		image_view_specification.subresourceRange.baseArrayLayer = 0;
		image_view_specification.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &image_view_specification, nullptr, &swap_chain_image_views[i]) != VK_SUCCESS) {
			std::cout << "failed to create image views!" << std::endl;
			return -1;
		}
	}

	std::ifstream vert_file("shaders/vert.spv", std::ios::ate | std::ios::binary);
	if (!vert_file.is_open())
	{
		std::cout << "failed to open vertex shader file!" << std::endl;
		return -1;
	}
	size_t vert_file_size = (size_t)vert_file.tellg();
	std::vector<char> vert_file_buffer(vert_file_size);
	vert_file.seekg(0);
	vert_file.read(vert_file_buffer.data(), vert_file_size);
	vert_file.close();

	std::ifstream frag_file("shaders/frag.spv", std::ios::ate | std::ios::binary);
	if (!frag_file.is_open())
	{
		std::cout << "failed to open fragment shader file!" << std::endl;
		return -1;
	}
	size_t frag_file_size = (size_t)frag_file.tellg();
	std::vector<char> frag_file_buffer(frag_file_size);
	frag_file.seekg(0);
	frag_file.read(frag_file_buffer.data(), frag_file_size);
	frag_file.close();

	std::cout << "VERTEX/FRAGMENT SHADERS SUCCESSFULLY LOADED: " << std::endl
		<< "vertex shader file size: " << vert_file_size << std::endl
		<< "fragment shader file size: " << frag_file_size << std::endl;

	VkShaderModuleCreateInfo fragment_shader_module_specification{};
	fragment_shader_module_specification.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragment_shader_module_specification.codeSize = frag_file_size;
	fragment_shader_module_specification.pCode = reinterpret_cast<const u32*>(frag_file_buffer.data());

	VkShaderModule fragment_shader_module;
	if (vkCreateShaderModule(device, &fragment_shader_module_specification, nullptr, &fragment_shader_module) != VK_SUCCESS)
	{
		std::cout << "failed to create fragment shader module!" << std::endl;
		return -1;
	}

	VkShaderModuleCreateInfo vertex_shader_module_specification{};
	vertex_shader_module_specification.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertex_shader_module_specification.codeSize = vert_file_size;
	vertex_shader_module_specification.pCode = reinterpret_cast<const u32*>(vert_file_buffer.data());

	VkShaderModule vertex_shader_module;
	if (vkCreateShaderModule(device, &vertex_shader_module_specification, nullptr, &vertex_shader_module) != VK_SUCCESS)
	{
		std::cout << "failed to create vertex shader module!" << std::endl;
		return -1;
	}

	std::cout << "VERTEX/SHADER MODULES SUCCESSFULLY CREATED" << std::endl;






	vkDestroyShaderModule(device, fragment_shader_module, nullptr);
	vkDestroyShaderModule(device, vertex_shader_module, nullptr);





	while (!glfwWindowShouldClose(window))
	{
		process_input(window);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	if (validation_layers_enabled)
	{
		auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan_instance, "vkDestroyDebugUtilsMessengerEXT");
		if (vkDestroyDebugUtilsMessengerEXT) {
			vkDestroyDebugUtilsMessengerEXT(vulkan_instance, debug_messenger, nullptr);
		}
		else {
			std::cout << "vkDestroyDebugUtilsMessengerEXT could not be loaded!" << std::endl;
			return -1;
		}
	}

	for (auto image_view : swap_chain_image_views)
		vkDestroyImageView(device, image_view, nullptr);

	vkDestroySwapchainKHR(device, swap_chain, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(vulkan_instance, surface, nullptr);
	vkDestroyInstance(vulkan_instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}