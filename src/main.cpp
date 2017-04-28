//GLEW
#define GLEW_STATIC
#include <GL\glew.h>
//GLFW
#include <GLFW\glfw3.h>
#include <iostream>
#include "myShader.h"
#include "camara.h"
#include "object.h" 
#include "material.h"
//para textura
#include <SOIL.h>
//para vector
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace glm;
using namespace std;
const GLint WIDTH = 800, HEIGHT = 800;
bool WIREFRAME = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
vec3 mov, rot, scal;
vec3 movement;
GLfloat radiansX,radiansY;
GLfloat mixValor;
GLfloat radX = 0;
GLfloat radY = 0;
GLint LightOption=3;
Camera myCamera({ 0,0,3 }, { 0,0,-1 }, 0.05, 45);

void MouseScroll(GLFWwindow* window, double xScroll, double yScroll) {
	myCamera.MouseScroll(window, xScroll, yScroll);
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos);



int main() {
	
	//initGLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);

	//set GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//create a window
	GLFWwindow* window;
	window = glfwCreateWindow(WIDTH, HEIGHT, "Primera ventana", nullptr, nullptr);
	if (!window) {
		cout << "Error al crear la ventana" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	//set GLEW and inicializate
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		cout << "Error al inicializar glew" << endl;
		glfwTerminate();
		return NULL;
	}

	//set function when callback
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, MouseScroll);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//GLFW_CURSOR_DISABLED

	//set windows and viewport
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	//fondo	
	
	//cargamos los shader
	glEnable(GL_DEPTH_TEST);
//	Shader LightShader("./src/LightVertex.vertexshader", "./src/LightFragment.fragmentshader");
	Shader ReceiveShader("./src/ReceiveVertex.vertexshader", "./src/ReceiveFragment.fragmentshader");

	Shader DireccionShader("./src/DirVertex.vertexshader", "./src/DirFragment.fragmentshader");
	Shader PointShader("./src/PointVertex.vertexshader", "./src/PointFragment.fragmentshader");
	Shader FocalShader("./src/FocalVertex.vertexshader", "./src/FocalFragment.fragmentshader");

	int width, height;
	unsigned char* especular = SOIL_load_image("./src/especular.png", &width, &height, 0, SOIL_LOAD_RGB);
	Material material("./src/difuso.png", "./src/especular.png", 32.0);
	
	Object cubA({ 0.3f,0.3f,0.3f }, { 0.f,0.f,0.0f }, { 0.f,0.3f,0.1f }, Object::cube);
	Object cubB({ 0.1,0.1,0.1 }, { -1.f,0.f,0.0f }, { 0.f,-0.3f,0.7f }, Object::cube);
	
	material.SetMaterial(&DireccionShader);
	material.SetMaterial(&PointShader);
	material.SetMaterial(&FocalShader);

	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
				
		//Establecer el color de fondo
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//establecer el shader
		
		GLint lightPosLoc, viewPosLoc,lightDirPos;
		mat4 proj;		mat4 view;		mat4 model;
		GLint modelLoc, viewLoc, projectionLoc;
//////////////////////////////////////////////////LIGHTS-DIRECCIONAL///////////////////////////////////////////////////
		//pulsar key 5
		if (LightOption ==1){
			DireccionShader.USE();
			material.SetShininess(&DireccionShader);
			material.ActivateTextures();
			lightPosLoc = glGetUniformLocation(DireccionShader.Program, "lightPos");
			viewPosLoc = glGetUniformLocation(DireccionShader.Program, "viewPos");

			//direccional
			lightDirPos = glGetUniformLocation(DireccionShader.Program, "material.Ldirection");
			glUniform3f(lightDirPos, -0.2f, -1.0f, -0.3f);
			glUniform3f(lightPosLoc, cubB.GetPosition().x, cubB.GetPosition().y, cubB.GetPosition().z);
			glUniform3f(viewPosLoc, myCamera.cameraPos.x, myCamera.cameraPos.y, myCamera.cameraPos.z);
		
			proj = perspective(radians(myCamera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
			myCamera.DoMovement(window);
			view = myCamera.LookAt();

			// Set lights properties
			glUniform3f(glGetUniformLocation(DireccionShader.Program, "material.Lambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(DireccionShader.Program, "material.Ldiffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(DireccionShader.Program, "material.Lspecular"), 1.0f, 1.0f, 1.0f);

			cubA.Rotate(radiansX, radiansY);
			cubA.Move(movement);
			model = glm::translate(model, cubA.GetPosition());
			model = cubA.GetModelMatrix();
		
			modelLoc = glGetUniformLocation(DireccionShader.Program,"model");	
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
			viewLoc = glGetUniformLocation(DireccionShader.Program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
			projectionLoc = glGetUniformLocation(DireccionShader.Program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));

			//pintar el VAO
			cubA.Draw();
		}
//////////////////////////////////////////////////LIGHTS-PUNTUAL///////////////////////////////////////////////////.
		//pulsar key 6
		if (LightOption == 2) {
			PointShader.USE();
			material.SetShininess(&PointShader);
			material.ActivateTextures();
			lightPosLoc = glGetUniformLocation(PointShader.Program, "lightPos");
			viewPosLoc = glGetUniformLocation(PointShader.Program, "viewPos");

			//Puntual
			glUniform3f(lightPosLoc, cubB.GetPosition().x, cubB.GetPosition().y, cubB.GetPosition().z);
			glUniform3f(viewPosLoc, myCamera.cameraPos.x, myCamera.cameraPos.y, myCamera.cameraPos.z);

			proj = perspective(radians(myCamera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
			myCamera.DoMovement(window);
			view = myCamera.LookAt();

			// Set lights properties
			glUniform3f(glGetUniformLocation(PointShader.Program, "material.Lambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(PointShader.Program, "material.Ldiffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(PointShader.Program, "material.Lspecular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(PointShader.Program, "material.Lconstant"), 1.0f);
			glUniform1f(glGetUniformLocation(PointShader.Program, "material.Llinear"), 0.09);
			glUniform1f(glGetUniformLocation(PointShader.Program, "material.Lquadratic"), 0.032);

			cubA.Rotate(radiansX, radiansY);
			cubA.Move(movement);
			model = glm::translate(model, cubA.GetPosition());
			model = cubA.GetModelMatrix();

			modelLoc = glGetUniformLocation(PointShader.Program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
			viewLoc = glGetUniformLocation(PointShader.Program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
			projectionLoc = glGetUniformLocation(PointShader.Program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));

			//pintar el VAO
			cubA.Draw();
		}
//////////////////////////////////////////////////LIGHTS-FOCAL///////////////////////////////////////////////////
		//pulsar key 7
		if (LightOption == 3) {
			FocalShader.USE();
			material.SetShininess(&FocalShader);
			material.ActivateTextures();
			lightPosLoc = glGetUniformLocation(FocalShader.Program, "lightPos");
			viewPosLoc = glGetUniformLocation(FocalShader.Program, "viewPos");
			lightDirPos = glGetUniformLocation(FocalShader.Program, "material.Ldirection");
			GLint lightSpotCutOffLoc = glGetUniformLocation(FocalShader.Program, "material.LcutOff");
			GLint lightSpotOuterCutOffLoc = glGetUniformLocation(FocalShader.Program, "material.LouterCutOff");
			
			glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(45.5f)));
			glUniform1f(lightSpotOuterCutOffLoc, glm::cos(glm::radians(60.5f)));
			//glUniform3f(lightDirPos, myCamera.cameraFront.x, myCamera.cameraFront.y, myCamera.cameraFront.z);
			glUniform3f(lightDirPos, 0, -1, 0);

			//Puntual
			glUniform3f(lightPosLoc, cubB.GetPosition().x, cubB.GetPosition().y, cubB.GetPosition().z);
			glUniform3f(viewPosLoc, myCamera.cameraPos.x, myCamera.cameraPos.y, myCamera.cameraPos.z);

			proj = perspective(radians(myCamera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
			myCamera.DoMovement(window);
			view = myCamera.LookAt();

			// Set lights properties
			glUniform3f(glGetUniformLocation(FocalShader.Program, "material.Lambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(FocalShader.Program, "material.Ldiffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(FocalShader.Program, "material.Lspecular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(FocalShader.Program, "material.Lconstant"), 1.0f);
			glUniform1f(glGetUniformLocation(FocalShader.Program, "material.Llinear"), 0.09);
			glUniform1f(glGetUniformLocation(FocalShader.Program, "material.Lquadratic"), 0.032);

			cubA.Rotate(radiansX, radiansY);
			cubA.Move(movement);
			model = glm::translate(model, cubA.GetPosition());
			model = cubA.GetModelMatrix();

			modelLoc = glGetUniformLocation(FocalShader.Program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
			viewLoc = glGetUniformLocation(FocalShader.Program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
			projectionLoc = glGetUniformLocation(FocalShader.Program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));

			//pintar el VAO
			cubA.Draw();
		}

////////////////////////////////////////////////ReceiveCub///////////////////////////////////////////////////////
		ReceiveShader.USE();
		
		model = cubB.GetModelMatrix();

		modelLoc = glGetUniformLocation(ReceiveShader.Program, "model");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
		viewLoc = glGetUniformLocation(ReceiveShader.Program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
		projectionLoc = glGetUniformLocation(ReceiveShader.Program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));

		//pintar el VAO
		cubB.Draw();
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// liberar la memoria de los VAO, EBO y VBO
	cubA.Delete();
	cubB.Delete();

	// Terminate GLFW, clearing any resources allocated by GLFW.
	exit(EXIT_SUCCESS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		WIREFRAME = !WIREFRAME;
	if (key == GLFW_KEY_1 && mixValor + 0.02 <= 1)
		mixValor += 0.02;
	if (key == GLFW_KEY_2 && mixValor - 0.02 >= 0)
		mixValor -= 0.02;
	if (key == GLFW_KEY_KP_4)
		radiansX -= 0.5;
	if (key == GLFW_KEY_KP_6)
		radiansX += 0.5;
	if (key == GLFW_KEY_KP_8)
		radiansY -= 0.5;
	if (key == GLFW_KEY_KP_2)
		radiansY += 0.5;
	
	if (key == GLFW_KEY_LEFT )
		movement.x -= 0.05;
	if (key == GLFW_KEY_RIGHT)
		movement.x += 0.05;
	if (key == GLFW_KEY_UP )
		movement.y += 0.05;
	if (key == GLFW_KEY_DOWN )
		movement.y -= 0.05;

	if (key == GLFW_KEY_5)
		LightOption = 1;
	if (key == GLFW_KEY_6)
		LightOption = 2;
	if (key == GLFW_KEY_7)
		LightOption = 3;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseMove(window, xpos, ypos);
};
