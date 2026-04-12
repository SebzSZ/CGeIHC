/*
Práctica 6: Texturizado
*/
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
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture dadoOchoTexture;
Texture pisoTexture;

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

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";


void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	// Piso
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f,	0.0f,  10.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f,  10.0f,	10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	Mesh* piso = new Mesh();
	piso->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(piso); // meshList[0]
}


void CrearOctaedro()
{
	unsigned int octaedro_indices[] = {
		0,  1,  2,
		3,  4,  5,
		6,  7,  8,
		9,  10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23
	};

	GLfloat vertices_octaedro[] = {
		// X      Y      Z       S      T       Nx    Ny    Nz

		// Cara 1
		0.0f,  1.0f,  0.0f,   0.5f,  0.75f,  0.0f, 0.0f, 0.0f,
		1.0f,  0.0f,  0.0f,   0.25f, 0.5f,   0.0f, 0.0f, 0.0f,
		0.0f,  0.0f,  1.0f,   0.75f, 0.5f,   0.0f, 0.0f, 0.0f,

		// Cara 2
		0.0f,  1.0f,  0.0f,   0.25f, 0.5f,   0.0f, 0.0f, 0.0f,
		0.0f,  0.0f,  1.0f,   0.0f,  0.25f,  0.0f, 0.0f, 0.0f,
	   -1.0f,  0.0f,  0.0f,   0.5f,  0.25f,  0.0f, 0.0f, 0.0f,

	   // Cara 3
	   0.0f,  1.0f,  0.0f,   0.0f,  0.75f,  0.0f, 0.0f, 0.0f,
	  -1.0f,  0.0f,  0.0f,   0.5f,  0.75f,  0.0f, 0.0f, 0.0f,
	   0.0f,  0.0f, -1.0f,   0.25f, 0.5f,   0.0f, 0.0f, 0.0f,

	   // Cara 4
	   0.0f,  1.0f,  0.0f,   0.75f, 0.5f,   0.0f, 0.0f, 0.0f,
	   0.0f,  0.0f, -1.0f,   0.25f, 0.5f,   0.0f, 0.0f, 0.0f,
	   1.0f,  0.0f,  0.0f,   0.5f,  0.25f,  0.0f, 0.0f, 0.0f,

	   // Cara 5
	   0.0f, -1.0f,  0.0f,   0.75f, 1.0f,   0.0f, 0.0f, 0.0f,
	   0.0f,  0.0f,  1.0f,   0.5f,  0.75f,  0.0f, 0.0f, 0.0f,
	   1.0f,  0.0f,  0.0f,   1.0f,  0.75f,  0.0f, 0.0f, 0.0f,

	   // Cara 6
	   0.0f, -1.0f,  0.0f,   0.75f, 0.5f,   0.0f, 0.0f, 0.0f,
	  -1.0f,  0.0f,  0.0f,   0.5f,  0.25f,  0.0f, 0.0f, 0.0f,
	   0.0f,  0.0f,  1.0f,   1.0f,  0.25f,  0.0f, 0.0f, 0.0f,

	   // Cara 7
	   0.0f, -1.0f,  0.0f,   1.0f,  0.75f,  0.0f, 0.0f, 0.0f,
	   0.0f,  0.0f, -1.0f,   0.5f,  0.75f,  0.0f, 0.0f, 0.0f,
	  -1.0f,  0.0f,  0.0f,   0.75f, 0.5f,   0.0f, 0.0f, 0.0f,

	  // Cara 8
	  0.0f, -1.0f,  0.0f,   1.0f,  0.25f,  0.0f, 0.0f, 0.0f,
	  1.0f,  0.0f,  0.0f,   0.75f, 0.0f,   0.0f, 0.0f, 0.0f,
	  0.0f,  0.0f, -1.0f,   0.5f,  0.25f,  0.0f, 0.0f, 0.0f,
	};

	calcAverageNormals(octaedro_indices, 24, vertices_octaedro, 192, 8, 5);

	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(vertices_octaedro, octaedro_indices, 192, 24);
	meshList.push_back(octaedro);
}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CrearOctaedro();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 2.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 1.0f);

	dadoOchoTexture = Texture("Textures/dado_ocho.jpg");
	dadoOchoTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	// Modelos del carro
	Carro_Base_M = Model();
	Carro_Base_M.LoadModel("Models/CarroBase.fbx");
	Carro_Arriba_M = Model();
	Carro_Arriba_M.LoadModel("Models/CarroArriba.obj");
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

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelrin(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// Piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		meshList[0]->RenderMesh(); 

		// Octaedro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.0f, 3.0f, -2.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glDisable(GL_CULL_FACE);
		dadoOchoTexture.UseTexture();
		meshList[1]->RenderMesh();
		glEnable(GL_CULL_FACE);

		// Carro
		// Base
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.1f, 0.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getarticulacion1() - mainWindow.getarticulacion2(), 0.0f, 0.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Base_M.RenderModel();

		// Parte superior
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.35f, 0.0f, 0.1f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (mainWindow.getarticulacion3() - mainWindow.getarticulacion4()) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Arriba_M.RenderModel();

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