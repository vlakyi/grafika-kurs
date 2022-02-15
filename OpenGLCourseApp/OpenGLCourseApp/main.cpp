#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Window dimentions
const GLint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO, shader;

// Vertex Shader - set vertex position
static const char* vShader = "											\n\
#version 330															\n\
																		\n\
layout (location = 0) in vec3 pos;										\n\
																		\n\
void main() {															\n\
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);						\n\
}";


// Fragment Shader - takes the result of vertex shader and modifies it
static const char* fShader = "											\n\
#version 330															\n\
																		\n\
out vec4 colour;														\n\
																		\n\
void main() {															\n\
	colour = vec4(1.0f, 0.0f, 0.0f, 1.0f);								\n\
}";

void CreateTriangle() {
	// center of the window is x = 0 and y = 0 So the x = -1.0f represents the left and 1.0f represents the right(window border). The same works for the y. So y = -1.0f is the bottom. And y = 1.0f is the top.
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	// create vertex array.
	glGenVertexArrays(1, &VAO);	// number of vertices and where to save id
	glBindVertexArray(VAO);

	// create buffer array and fill it with vertices data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // GL_STATIC_DRAW says that we are not going to change dynamically our vertices representation and those points will be static.
	
	// index - id of location for attributes in shader, size - of each chunck of values which we pass (in our case x, y, z)
	// type - of those values
	// normalized - true/false if we want or not to normalize the values
	// stride - values that we want to skip, offset - where the data start
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 

	// index, the same as above
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// unbind buffer and vertex array
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType); // create an empty shader of the specific type

	// save code to the variable
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	// get the code length
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	// add the source code to the newly created shader.

	// shader - shader where we would like to attach source code
	// count - number of source code chunks we would like to attach (because it can be splitted to multiple chunks)
	// codeLength - length of the code
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	// setup debugging
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// validate shader compile status to be sure that everything is working as expected.
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	// attach shader to the program
	glAttachShader(theProgram, theShader);
}

void CompileShaders() {
	// create a shader program
	shader = glCreateProgram();

	if (!shader) {
		printf("Error creating shader program");
		return;
	}

	// add vertex and fragment shaders to the program
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	// setup debugging
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// create the executables on the graphics card side, link it etc.
	glLinkProgram(shader);

	// validate link status to be sure that everything is working as expected.
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader); // checking if shader is valid for the current context OpenGL is working

	// validate link status to be sure that everything is working as expected.
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}
}

int main() 
{
	// Initialize GLFW
	if (!glfwInit()) {
		printf("GLFW Initialisation failed");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create window
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow) {
		printf("GLFW window creation failed!");
		return 1;
	}

	// Get Buffer size information - Aria inside the window, Viewport etc.
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use

	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features - WE WOULDN'T USE THEM
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup Viewport size (x, y, bufferWidth, bufferHeight)
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow)) {
		// Get + Handle user input events
		glfwPollEvents();

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use program related to the global shader variable
		glUseProgram(shader);
		glBindVertexArray(VAO);

		// Draw VAO as triangles starting from 0 and grap 3 elements at a time 
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Unbind 
		glBindVertexArray(0);
		glUseProgram(0);

		// You have 2 scenes at once. 1 user can see and another one user can't. At this point we are swapping two scenes.
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}