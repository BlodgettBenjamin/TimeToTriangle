/*
#include <glad/glad.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

typedef uint32_t u32;

void process_input(GLFWwindow* window);

const u32 window_width = 800;
const u32 window_height = 600;

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

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
*/