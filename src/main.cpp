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

	if (physical_device == VK_NULL_HANDLE || !required_extensions_set.empty())
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
	device_specification.enabledExtensionCount = 0;

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