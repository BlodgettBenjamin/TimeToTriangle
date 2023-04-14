#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <stdint.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

typedef uint32_t u32;

const u32 window_width = 800;
const u32 window_height = 600;

const std::string vss = R"glsl(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aColor;

	out vec3 ourColor;

	void main()
	{
		ourColor = aColor;
		gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	}
)glsl";
const char* vertex_shader_source = vss.c_str();

const std::string fss = R"glsl(
	#version 330 core

	in vec3 ourColor;

	out vec4 FragColor;

	void main()
	{
		FragColor = vec4(ourColor, 1.0f);
	} 
)glsl";
const char* fragment_shader_source = fss.c_str();

float vertices[] = {
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "learnopengl", nullptr, nullptr);
	if (!window)
	{
		std::cout << "could not create glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize glad" << std::endl;
		return -1;
	}
	
	glViewport(0, 0, window_width, window_height);

	//shader
	u32 vertex_shader, fragment_shader, shader_program;
	int  success;
	char info_log[512];

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetShaderiv(shader_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader_program, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	//buffers
	u32 vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	//drawing setup
	glUseProgram(shader_program);
	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}