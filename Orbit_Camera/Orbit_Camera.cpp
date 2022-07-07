#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "j3a.hpp"


#pragma comment (lib, "glew32s")
#pragma comment (lib, "opengl32")
#pragma commnet (lib, "glfw3")


#include <vector>
#define GLM_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "toys.h"


using namespace glm;

void render(GLFWwindow* window);
void init();
void mouseButtonCallback(GLFWwindow*, int, int, int);
void cursorMotionCallback(GLFWwindow*, double, double);

GLuint triangleVBO;
GLuint vertexArrayID;
GLuint indexVBOID;
Program program;

float cameraDistance = 3;
glm::vec3 sceneCenter = glm::vec3(0, 0, 0);
float cameraYaw = 0.f;
float cameraPitch = 0.f;
float cameraFov = 60.f;
int lastX, lastY;
int main(int argc, const char* argv[])
{
	if (!glfwInit()) exit(EXIT_FAILURE);


	glfwWindowHint(GLFW_SAMPLES, 8);
	GLFWwindow* window = glfwCreateWindow(640, 480, "201621034", 0, 0);
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorMotionCallback);

	glewInit();


	init();
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		render(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();


}



void init() {

	loadJ3A("bunny.j3a");
	program.loadShaders("shader.vert", "shader.frag");

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices[0] * sizeof(glm::vec3), vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &indexVBOID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTriangles[0] * sizeof(glm::u32vec3), triangles[0], GL_STATIC_DRAW);

}

float rotAngle = 0;

void render(GLFWwindow* window) {

	int w, h;
	
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(0, 0, .5, 0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(program.programID);
	glBindVertexArray(vertexArrayID);
	glEnable(GL_DEPTH_TEST);

	mat4 projMat = perspective(cameraFov * 3.141592f / 180, w / float(h), 0.01f, 100.f);


	vec3 cameraPosition = vec3(0, 0, cameraDistance);
	cameraPosition = vec3(rotate(cameraPitch, vec3(-1, 0, 0)) * vec4(cameraPosition, 1));
	cameraPosition = vec3(rotate(cameraYaw, vec3(0, 1, 0)) * vec4(cameraPosition, 1));
	mat4 viewMat = lookAt(cameraPosition, sceneCenter, vec3(0, 1, 0));

	rotAngle += 0.1 / 180.f * 3.141592;
	GLuint loc = glGetUniformLocation(program.programID, "modelMat");
	//mat4 rotMat = rotate(rotAngle, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(loc, 1, 0, value_ptr(rotate(90/180.f*3.141592f, vec3(1, 0, 0))));

	loc = glGetUniformLocation(program.programID, "viewMat");
	glUniformMatrix4fv(loc, 1, 0, value_ptr(viewMat));
	
	loc = glGetUniformLocation(program.programID, "projMat");
	glUniformMatrix4fv(loc, 1, 0, value_ptr(projMat));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
	glDrawElements(GL_TRIANGLES, nTriangles[0] * 3, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
	
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		lastX = int(xpos);
		lastY = int(ypos);
		
	}
}

void cursorMotionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			cameraFov += (ypos - lastY) / 30;
		}
		else {
			cameraPitch += (ypos - lastY) / 300;
			cameraPitch = glm::clamp(cameraPitch, -1.f, 1.f);
			cameraYaw -= (xpos - lastX) / 300;
		}
		lastX = int(xpos);
		lastY = int(ypos);
	}
}