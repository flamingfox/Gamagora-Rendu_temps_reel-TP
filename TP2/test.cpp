#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lib/objmanager.h"
#include "lib/mesh.h"

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
    gs.program = buildProgram("C:/Users/etu/workspace/code/Rendu temps reel/TP2/basic.vsl", "C:/Users/etu/workspace/code/Rendu temps reel/TP2/basic.fsl");


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

    glEnable(GL_DEPTH_TEST);


    // come from http://www.opengl-tutorial.org/fr/beginners-tutorials/tutorial-3-matrices/

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
     //glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) 16.0 / (float)9.0, 0.1f, 10.0f);
     glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) 640.0 / (float) 480.0, 0.1f, 10.0f);

     // Camera matrix
     glm::mat4 View = glm::lookAt(
                    glm::vec3(0,0,-5), // Camera is at (4,3,3), in World Space
                    glm::vec3(0,0,0), // and looks at the origin
                    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                    );

     // Model matrix : an identity matrix (model will be at the origin)
     glm::mat4 Model = glm::mat4(1.0f);

     // Our ModelViewProjection : multiplication of our 3 matrices
     glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

     // Get a handle for our "MVP" uniform
      // Only during the initialisation
      //GLuint MatrixID = glGetUniformLocation(program_id, "MVP");

      // Send our transformation to the currently bound shader, in the "MVP" uniform
      // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
      glProgramUniformMatrix4fv(gs.program, 23, 1, GL_FALSE, &mvp[0][0]);

}

void render(GLFWwindow* window)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
    double t = fmod(glfwGetTime(), 3.0)/3.0;
    double t2 = fmod(glfwGetTime(), 12.0)/12.0;

	glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(gs.program);
	glBindVertexArray(gs.vao);

    {
        float color[3] = {sin(t*3.14),1,0};
		glProgramUniform3fv(gs.program, 3, 1, color);

        glm::mat4 transf;

        //transf = glm::translate(glm::mat4(1.0f), glm::vec3(cos(t*6.28), sin(t*6.28), 0.0) );

        transf = glm::translate(glm::mat4(1.0f), glm::vec3(cos(t*6.28), sin(t*6.28), 0.0) )*
                glm::rotate(glm::mat4(1.0f), (float)(t2*180.0/3.0f), glm::vec3(0,1,0));


        glProgramUniformMatrix4fv(gs.program, 1, 1, GL_FALSE,  &transf[0][0]);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


        color[0] = 1; color[1] = 0, color[2] = sin(t*3.14);
        glProgramUniform3fv(gs.program, 3, 1, color);

        transf = glm::translate(glm::mat4(1.0f), glm::vec3(sin(-t*6.28), cos(t*6.18+3.14), 0.0) );

        glProgramUniformMatrix4fv(gs.program, 1, 1, GL_FALSE, &transf[0][0]);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	glBindVertexArray(0);
	glUseProgram(0);

}
