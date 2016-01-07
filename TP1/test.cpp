#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

void render(GLFWwindow*);

void init();

#define glInfo(a) std::cout << #a << ": " << glGetString(a) << std::endl

// This function is called on any openGL API error
void debug(GLenum, // source
		   GLenum, // type
		   GLuint, // id
		   GLenum, // severity
		   GLsizei, // length
		   const GLchar *message,
		   const void *) // userParam
{
	std::cout << "DEBUG: " << message << std::endl;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
    if (!glfwInit())
	{
		std::cerr << "Could not init glfw" << std::endl;
		return -1;
	}

	// This is a debug context, this is slow, but debugs, which is interesting
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		std::cerr << "Could not init window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		std::cerr << "Could not init GLEW" << std::endl;
		std::cerr << glewGetErrorString(err) << std::endl;
		glfwTerminate();
		return -1;
	}

	// Now that the context is initialised, print some informations
	glInfo(GL_VENDOR);
	glInfo(GL_RENDERER);
	glInfo(GL_VERSION);
	glInfo(GL_SHADING_LANGUAGE_VERSION);

	// And enable debug
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    //glDebugMessageCallback(debug, nullptr);

	// This is our openGL init function which creates ressources
	init();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		render(window);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// Build a shader from a string
GLuint buildShader(GLenum const shaderType, std::string const src)
{
	GLuint shader = glCreateShader(shaderType);

	const char* ptr = src.c_str();
	GLint length = src.length();

	glShaderSource(shader, 1, &ptr, &length);

	glCompileShader(shader);

	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if(!res)
	{
		std::cerr << "shader compilation error" << std::endl;

		char message[1000];

		GLsizei readSize;
		glGetShaderInfoLog(shader, 1000, &readSize, message);
		message[999] = '\0';

		std::cerr << message << std::endl;

		glfwTerminate();
		exit(-1);
	}

	return shader;
}

// read a file content into a string
std::string fileGetContents(const std::string path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();

	return buffer.str();
}

// build a program with a vertex shader and a fragment shader
GLuint buildProgram(const std::string vertexFile, const std::string fragmentFile)
{
	auto vshader = buildShader(GL_VERTEX_SHADER, fileGetContents(vertexFile));
	auto fshader = buildShader(GL_FRAGMENT_SHADER, fileGetContents(fragmentFile));

	GLuint program = glCreateProgram();

	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	glLinkProgram(program);

	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if(!res)
	{
		std::cerr << "program link error" << std::endl;

		char message[1000];

		GLsizei readSize;
		glGetProgramInfoLog(program, 1000, &readSize, message);
		message[999] = '\0';

		std::cerr << message << std::endl;

		glfwTerminate();
		exit(-1);
	}

	return program;
}

/****************************************************************
 ******* INTERESTING STUFFS HERE ********************************
 ***************************************************************/

// Store the global state of your program
struct
{
	GLuint program; // a shader
	GLuint vao; // a vertex array object
} gs;

void init()
{
	// Build our program and an empty VAO
	gs.program = buildProgram("C:/Users/etu/workspace/rendu/basic.vsl", "C:/Users/etu/workspace/rendu/basic.fsl");

	float data[16] = {-0.5, -0.5, 0, 1,
					0.5, -0.5, 0, 1,
					0.5, 0.5, 0, 1,
					-0.5, 0.5, 0, 1};

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 16*4, data, GL_STATIC_DRAW);

	glCreateVertexArrays(1, &gs.vao);

	glBindVertexArray(gs.vao);

		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(12, 3, GL_FLOAT, GL_FALSE, 16, 0);
		glEnableVertexAttribArray(12);

	glBindVertexArray(0);
}

void render(GLFWwindow* window)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
    double t = fmod(glfwGetTime(), 3.0)/3.0;

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(gs.program);
	glBindVertexArray(gs.vao);

	{
        float color[3] = {sin(t*3.14),1,0};
		glProgramUniform3fv(gs.program, 3, 1, color);
        float transf[2] = {cos(t*6.28), sin(t*6.28)};
		glProgramUniform2fv(gs.program, 1, 1, transf);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        color[0] = 1; color[1] = 0, color[2] = sin(t*3.14);
		glProgramUniform3fv(gs.program, 3, 1, color);
        transf[0] = sin(-t*6.28); transf[1] = cos(t*6.18+3.14);
		glProgramUniform2fv(gs.program, 1, 1, transf);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	glBindVertexArray(0);
	glUseProgram(0);

}
