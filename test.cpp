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
#include <glm/ext.hpp>

#include "lib/objmanager.h"
#include "lib/mesh.h"

void render(GLFWwindow*);

void init();

#define glInfo(a) std::cout << #a << ": " << glGetString(a) << std::endl

int nbVertex;

float angleX =0, angleY =0, anglePhiLight = 0, angleTetaLight = 0;

void ajoutSol(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3, const Vector3D& normal,
              int tailleTableau, float* tableauVertex, float* tableauNormal);

//constant for the mvp
glm::mat4 Projection, Model;

//Input management for rotation
void inputHandling(GLFWwindow* window){
    //For object rotation
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        angleY -= 0.1;
    else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        angleY += 0.1;

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        angleX += 0.1;
    else if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        angleX -= 0.1;


    //For light placement

    if(glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
        anglePhiLight -= 0.1;
    else if(glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
        anglePhiLight += 0.1;


    if(glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
        angleTetaLight -= 0.1;
    else if(glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
        angleTetaLight += 0.1;


    //For exit
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

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
        message[999] = '/0';

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
        message[999] = '/0';

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
    GLuint programView; // shaders for view camera
    GLuint programShadow; // shaders for light camera
	GLuint vao; // a vertex array object

    GLuint depthTexture; // texture from the light camera
    GLuint fbo; //framebuffer for shadow
} gs;

void init()
{
    //std::string racineProjet = "C:/Users/etu/workspace/code/Rendu temps reel/";
    //std::string racineProjet = "C:/Users/etu/Documents/GitHub/Gamagora-Rendu_temps_reel-TP/";
    std::string racineProjet = "B:/Utilisateur/git/code/Gamagora-Rendu_temps_reel-TP/";

	// Build our program and an empty VAO
    gs.programView = buildProgram((racineProjet+(std::string)"basic.vsl").c_str(), (racineProjet+(std::string)"basic.fsl").c_str());

    gs.programShadow = buildProgram((racineProjet+(std::string)"light.vsl").c_str(), (racineProjet+(std::string)"light.fsl").c_str());


    Mesh m;
    m = ObjManager::loadFromOBJ(Vector3D(0,0,0), (racineProjet+(std::string)"monkey.obj").c_str());

    nbVertex = m.nbface()+6; //nbface + quad "sol"

    float data[nbVertex*4];
    float dataNormal[nbVertex*4];

    std::vector<Vector3D> vertex = m.getvertex();
    std::vector<int> face = m.getface();
    std::vector<Vector3D> normals = m.getNormals();
    std::vector<int> normalIds = m.getNormalIds();

    int i=0;
    for(int j=0; j<face.size(); j++){
        //set vertex
        data[i] = vertex[face[j]].x;
        data[i+1] = vertex[face[j]].y;
        data[i+2] = vertex[face[j]].z;
        data[i+3] = 1;



        dataNormal[i] = normals[normalIds[j]].x;
        dataNormal[i+1] = normals[normalIds[j]].y;
        dataNormal[i+2] = normals[normalIds[j]].z;
        dataNormal[i+3] = 1;

        i+=4;

        /*
        data[i+4] = vertex[face[j+1]].x;
        data[i+5] = vertex[face[j+1]].y;
        data[i+6] = vertex[face[j+1]].z;
        data[i+7] = 1;

        data[i+8] = vertex[face[j+2]].x;
        data[i+9] = vertex[face[j+2]].y;
        data[i+10] = vertex[face[j+2]].z;
        data[i+11] = 1;


        //set normal
        Vector3D normal = ( vertex[face[j+1]] - vertex[face[j]] ).crossProduct( ( vertex[face[j+2]] - vertex[face[j]] ) );
        normal.normalize();

        dataNormal[i] = normal.x;       dataNormal[i+4] = normal.x;     dataNormal[i+8] = normal.x;
        dataNormal[i+1] = normal.y;     dataNormal[i+5] = normal.y;     dataNormal[i+9] = normal.y;
        dataNormal[i+2] = normal.z;     dataNormal[i+6] = normal.z;     dataNormal[i+10] = normal.z;
        dataNormal[i+3] = 1;            dataNormal[i+7] = 1;            dataNormal[i+11] = 1;

        i+=12;*/
    }

    //ajout du quad pour faire le sol
    ajoutSol(Vector3D(-3,-1,-3), Vector3D(3,-1,-3), Vector3D(3,-1,3), Vector3D(-3,-1,3), Vector3D(0,1,0),
                  nbVertex*4, data, dataNormal);



	GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, nbVertex*4*4, data, GL_STATIC_DRAW);

    GLuint buffer2;
    glGenBuffers(1, &buffer2);
    glBindBuffer(GL_ARRAY_BUFFER, buffer2);
    glBufferData(GL_ARRAY_BUFFER, nbVertex*4*4, dataNormal, GL_STATIC_READ);

	glCreateVertexArrays(1, &gs.vao);

	glBindVertexArray(gs.vao);

        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(12);

        glBindBuffer(GL_ARRAY_BUFFER, buffer2);
        glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(13);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);


    // come from http://www.opengl-tutorial.org/fr/beginners-tutorials/tutorial-3-matrices/

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
     //glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) 16.0 / (float)9.0, 0.1f, 10.0f);
     glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) 640.0 / (float) 480.0, 0.1f, 10.0f);

     // Camera matrix
     glm::mat4 ViewCamera = glm::lookAt(
                    glm::vec3(0,2,5), // Camera is at (4,3,3), in World Space
                    glm::vec3(0,0,0), // and looks at the origin
                    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                    );

     // Model matrix : an identity matrix (model will be at the origin)
     glm::mat4 Model = glm::mat4(1.0f);

     // Our ModelViewProjection : multiplication of our 3 matrices
     glm::mat4 mvpCamera = Projection * ViewCamera * Model; // Remember, matrix multiplication is the other way around

     // Get a handle for our "MVP" uniform
      // Only during the initialisation
      //GLuint MatrixID = glGetUniformLocation(program_id, "MVP");

      // Send our transformation to the currently bound shader, in the "MVP" uniform
      // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
      glProgramUniformMatrix4fv(gs.programView, 23, 1, GL_FALSE, &mvpCamera[0][0]);





      // create the depth texture
      glGenTextures(1, &gs.depthTexture);
      glBindTexture(GL_TEXTURE_2D, gs.depthTexture);
      glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 200, 200);

      // Framebuffer
      glGenFramebuffers(1, &gs.fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, gs.fbo);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gs.depthTexture, 0);

      assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    inputHandling(window);


    glm::mat4 transf;

    transf = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(0,1,0)) *
            glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(1,0,0));

    glProgramUniformMatrix4fv(gs.programView, 1, 1, GL_FALSE,  &transf[0][0]);
    glProgramUniformMatrix4fv(gs.programShadow, 1, 1, GL_FALSE,  &transf[0][0]);


    /********** Section lumière **********/
    glm::vec3 lightPosition(10.0f, 0, 10.f);

    glm::vec4 lightPositionTransformed =
            glm::rotate(glm::mat4(1.0f), anglePhiLight, glm::vec3(0,1,0)) *
            glm::rotate(glm::mat4(1.0f), angleTetaLight, glm::vec3(1,0,0)) * glm::vec4(lightPosition,1.0f);

    /*** calcul du mvp de la caméra lumière (déplacement de la lumière donc calcule ici) ***/

    lightPosition = glm::vec3(XYZ(lightPositionTransformed));

    // Light Camera matrix
    glm::mat4 ViewLightCamera = glm::lookAt(
                   lightPosition, // Camera is at (4,3,3), in World Space
                   glm::vec3(0,0,0), // and looks at the origin
                   glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                   );

    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 mvpLightCamera = Projection * ViewLightCamera * Model; // Remember, matrix multiplication is the other way around

    glProgramUniformMatrix4fv(gs.programShadow, 22, 1, GL_FALSE, &mvpLightCamera[0][0]);
    glProgramUniformMatrix4fv(gs.programView, 22, 1, GL_FALSE, &mvpLightCamera[0][0]);




    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);



    glUseProgram(gs.programView);
    glBindVertexArray(gs.vao);

    glViewport(0, 0, width, height);

    {
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, gs.depthTexture);

        glDrawArrays(GL_TRIANGLES, 0, nbVertex*4);
    }


    glBindVertexArray(0);
    glUseProgram(0);



    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);

    glUseProgram(gs.programView);
    glBindVertexArray(gs.vao);

    {
        float color[3] = {0,1,0};

        glProgramUniform3fv(gs.programView, 3, 1, color);
        glProgramUniform3fv(gs.programView, 4, 1, glm::value_ptr( lightPosition ));

        glDrawArrays(GL_TRIANGLES, 0, nbVertex*4);
    }

    glBindVertexArray(0);
    glUseProgram(0);

}

void ajoutSol(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3, const Vector3D& normal,
              int tailleTableau, float* tableauVertex, float* tableauNormal){

    int index = tailleTableau - (6*4);

    Vector3D vertexSol[6] = {p0, p1, p2, p2, p3, p0};

    for(Vector3D& vec : vertexSol){

        tableauVertex[index] = vec.x;
        tableauVertex[index+1] = vec.y;
        tableauVertex[index+2] = vec.z;
        tableauVertex[index+3] = 1;

        tableauNormal[index] = normal.x;
        tableauNormal[index+1] = normal.y;
        tableauNormal[index+2] = normal.z;
        tableauNormal[index+3] = 1;

        index+=4;
    }
}
