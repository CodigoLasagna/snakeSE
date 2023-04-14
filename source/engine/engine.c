#include "engine.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, Tconfig *config)
{
	/*
	mouse_input(window, config);
	*/
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, 1);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, 1);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	/*
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		config->perspective = !(config->perspective);
	*/

}

GLFWwindow *prepareGLFW(int width, int height)
{
	GLFWwindow* window;
	if (glfwInit())
	{
		window = glfwCreateWindow(width, height, "Snake de bodega aurrera", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		/*
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
		*/
		stbi_set_flip_vertically_on_load(1);
		glDepthFunc(GL_LESS);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		/*
		glEnable(GL_FRAMEBUFFER_SRGB); 
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		*/
	}
	
	return window;
}

int prepareGLEW()
{
	GLenum err;
	glewExperimental = 1;
	err=glewInit();
	if(err!=GLEW_OK) {
		printf("glewInit failed: %s", glewGetErrorString(err));
		exit(1);
		return 1;
	}
	return 0;
}
