#include <glad/glad.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

typedef uint32_t u32;

void process_input(GLFWwindow* window);

const u32 window_width = 800;
const u32 window_height = 600;

const std::vector<const char*> validation_layers = {
	"VK_LAYER_KHRONOS_validation"
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
			std::cout << "failed to set up debug messenger!" << std::endl;
			return -1;
		}
	}







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
			std::cout << "failed to set up clean up messenger object!" << std::endl;
			return -1;
		}
	}

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