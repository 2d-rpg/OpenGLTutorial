#include <cstdlib>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shape.h"

// Display the compile result for shader objects
// shader: shader object name
// str: string that shows the line with the compile error
GLboolean printShaderInfoLog(GLuint shader, const char *str) {
	// Get the compile result
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "Compile error at '" << str << "'" << std::endl;

	// Get the length of the compilation log
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		// Get the shader compilation log
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

// Display the result of object linking
// program: program object name
GLboolean printProgramInfoLog(GLuint program) {
	// Get the link result
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "Link error." << std::endl;

	// Get the link log length
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		// Get the link log
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

// Create program object
// vsrc: vertex shader source program string
// fsrc: fragment shader source program string
GLuint createProgram(const char *vsrc, const char *fsrc) {
	// Create an empty program object
	const GLuint program(glCreateProgram());

	if (vsrc != NULL) {
		// Create the shader ofbject for the vertex shader
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		// Integrate the shader object of the vertex shader in the program object
		if(printShaderInfoLog(vobj, "vertex shader"))
			glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}

	if (fsrc != NULL) {
		// Create the shader object for the fragment shader
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		// Integrate the shader object of the fragment shader in the program object
		if (printShaderInfoLog(fobj, "fragment shader"))
			glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}
	// Link the program object
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);

	// Return the created program object
	if(printProgramInfoLog(program))
		return program;

	// Returns 0 if failed to create the program object
	glDeleteProgram(program);
	return 0;
}

// Returns the memory data of the read source file
// name: The name of the shader source file
// buffer: The text of the read source file
bool readShaderSource(const char *name, std::vector<GLchar> &buffer) {
	// If name is null
	if (name == NULL) return false;

	// Open source file
	std::ifstream file(name, std::ios::binary);
	if (file.fail()) {
		// Could not open soure file
		std::cerr << "Error: Can't open source file: " << name << std::endl;
		return false;
	}

	// Go to file end to get the file size
	file.seekg(0L, std::ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());

	// Get the memory for the source file
	buffer.resize(length + 1);

	// Read file from the beginnning
	file.seekg(0L, std::ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';

	if (file.fail()) {
		// Failed to read
		std::cerr << "Error: Could not read source file: " << name << std::endl;
		file.close();
		return false;
	}

	// Success
	file.close();
	return true;
}

// Read shader source files and create a program object from them
// vert: source file name for the vertex shader
// frag: source file name for the fragment shader
GLuint loadProgram(const char *vert, const char *frag) {
	// Read the shader souce files
	std::vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	std::vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));

	// Create program object
	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

// 矩形の頂点の位置
constexpr Object::Vertex rectangleVertex[] = {
	{-0.5f, -0.5f},
	{ 0.5f, -0.5f},
	{ 0.5f,  0.5f},
	{-0.5f,  0.5f}
};

int main() {
	// Initialize GLFW 
	if (glfwInit() == GL_FALSE) {
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return 1;
	}

	// Sets the procedue when program exits
	atexit(glfwTerminate);

	// Select "OpenGL Version 3.2 Core"
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	GLFWwindow *const window(glfwCreateWindow(640, 480, "2D Game", NULL, NULL));
	if (window == NULL) {
		// Failed to initialize window
		std::cerr << "Failed to create GLFW window." << std::endl;
		return 1;
	}

	// Select current window for drawing
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW." << std::endl;
		return 1;
	}

	// Wait for vertical refresh
	glfwSwapInterval(1);

	// Fill screen with color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//static constexpr GLchar vsrc[] =
	//	"#version 150 core\n"
	//	"in vec4 position;\n"
	//	"void main()\n"
	//	"{\n"
	//	"	gl_Position = position;\n"
	//	"}\n";

	//static constexpr GLchar fsrc[] =
	//	"#version 150 core\n"
	//	"out vec4 fragment;\n"
	//	"void main()\n"
	//	"{\n"
	//	"	fragment = vec4(1.0, 0.0, 0.0, 1.0);\n"
	//	"}\n";

	//// Create program object
	//const GLuint program(createProgram(vsrc, fsrc));

	const GLuint program(loadProgram("point.vert", "point.frag"));

	// 図形データが作成する
	std::unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex));

	// Do while the window is open
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		// Clear the window
		glClear(GL_COLOR_BUFFER_BIT);

		// Use shader program
		glUseProgram(program);

		// Draw the screen here
		shape->draw();

		// Change the color buffer
		glfwSwapBuffers(window);

		// Take events
		glfwWaitEvents();
	}

	return 0;
}