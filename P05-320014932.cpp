#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Model Carro_Base_M;
Model Carro_Arriba_M;
Model Carro_Rueda1_M;
Model Carro_Rueda2_M;
Model Carro_Rueda3_M;
Model Carro_Rueda4_M;
Model Carro_Rin1_M;
Model Carro_Rin2_M;
Model Carro_Rin3_M;
Model Carro_Rin4_M;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	// Inicialización de la ventana
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	// Creación de objetos y shaders
	CreateObjects();
	CreateShaders();

	// Posición inicial de la cámara
	camera = Camera(glm::vec3(0.0f, 2.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 1.0f);

	// Inicialización de modelos
	Carro_Base_M = Model();
	Carro_Base_M.LoadModel("Models/CarroBase.fbx");
	Carro_Arriba_M = Model();
	Carro_Arriba_M.LoadModel("Models/CarroArriba.fbx");
	Carro_Rueda1_M = Model();
	Carro_Rueda1_M.LoadModel("Models/Llanta1.fbx");
	Carro_Rueda2_M = Model();
	Carro_Rueda2_M.LoadModel("Models/Llanta2.fbx");
	Carro_Rueda3_M = Model();
	Carro_Rueda3_M.LoadModel("Models/Llanta3.fbx");
	Carro_Rueda4_M = Model();
	Carro_Rueda4_M.LoadModel("Models/Llanta4.fbx");
	Carro_Rin1_M = Model();
	Carro_Rin1_M.LoadModel("Models/Rin1.fbx");
	Carro_Rin2_M = Model();
	Carro_Rin2_M.LoadModel("Models/Rin2.fbx");
	Carro_Rin3_M = Model();
	Carro_Rin3_M.LoadModel("Models/Rin3.fbx");
	Carro_Rin4_M = Model();
	Carro_Rin4_M.LoadModel("Models/Rin4.fbx");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelrin(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	// Loop principal
	while (!mainWindow.getShouldClose())
	{
		// Delta Time
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Dibujar el skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// DIBUJO DEL PISO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		// DIBUJO DE GODDARD
		// Base del Carro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.1f, 0.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getarticulacion1() - mainWindow.getarticulacion2(), 0.0f, 0.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Base_M.RenderModel();

		// Parte superior del Carro
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.35f, 0.0f, 1.9f));
		model = glm::rotate(model, (mainWindow.getarticulacion3() - mainWindow.getarticulacion4()) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		color = glm::vec3(0.0f, 0.0f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Arriba_M.RenderModel();

		// Ruedas y Rines del Carro
		// Rueda 1
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, -0.9f, 0.75f));
		model = glm::rotate(model, ((mainWindow.getarticulacion1() - mainWindow.getarticulacion2()) * 20.0f) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (mainWindow.getarticulacion5() - mainWindow.getarticulacion6()) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelrin = model;
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Rueda1_M.RenderModel();

		// Rin 1
		model = modelrin;
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		color = glm::vec3(0.0f, 0.0f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Rin1_M.RenderModel();

		// Rueda 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.9f, 0.75f));
		model = glm::rotate(model, ((mainWindow.getarticulacion1() - mainWindow.getarticulacion2()) * 20.0f) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (mainWindow.getarticulacion7() - mainWindow.getarticulacion8()) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelrin = model;
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Rueda2_M.RenderModel();

		// Rin 2
		model = modelrin;
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		color = glm::vec3(0.0f, 0.0f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Rin2_M.RenderModel();

		// Rueda 3
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.6f, -1.7f, 0.75f));
		model = glm::rotate(model, ((mainWindow.getarticulacion1() - mainWindow.getarticulacion2()) * 20.0f) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (mainWindow.getarticulacion9() - mainWindow.getarticulacion10()) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelrin = model;
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Rueda3_M.RenderModel();

		// Rin 3
		model = modelrin;
		color = glm::vec3(0.0f, 0.0f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Rin3_M.RenderModel();

		// Rueda 4
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.6f, 1.6f, 0.75f));
		model = glm::rotate(model, ((mainWindow.getarticulacion1() - mainWindow.getarticulacion2()) * 20.0f) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (mainWindow.getarticulacion11() - mainWindow.getarticulacion12()) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelrin = model;
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Rueda4_M.RenderModel();

		// Rin 4
		model = modelrin;
		color = glm::vec3(0.0f, 0.0f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Rin4_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}