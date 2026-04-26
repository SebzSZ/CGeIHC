#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Camera.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "PointLight.h"
#include "Shader_light.h"
#include "Skybox.h"
#include "Sphere.h"
#include "SpotLight.h"
#include "Texture.h"
#include "Window.h"

// Variables globales
Camera camera;
Material Material_opaco;
Material Material_brillante;
Skybox skybox;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Texturas
Texture pisoTexture;
Texture waterTexture;

// Modelos
Model fishTank_M;
Model fishAntenna_M;
Model fishBody_M;
Model fishLight_M;
Model lamp_M;
Model ship_M;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Iluminación
DirectionalLight mainLight;
PointLight changeLights[MAX_POINT_LIGHTS];
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight shipLights[2];

// Shaders
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

void CreateObjects()
{
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	 0.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,	10.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f,	 0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f,  10.0f,	10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	Mesh* floorMesh = new Mesh();
	floorMesh->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(floorMesh);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	// Creación de la ventana
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	// Creación de objetos y shaders
	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	waterTexture = Texture("Textures/blue.png");
	waterTexture.LoadTextureA();

	// Inicialización de modelos
	fishTank_M = Model();
	fishTank_M.LoadModel("Models/fishTank.obj");
	fishAntenna_M = Model();
	fishAntenna_M.LoadModel("Models/antena_pez.obj");
	fishBody_M = Model();
	fishBody_M.LoadModel("Models/cuerpo_pez.obj");
	fishLight_M = Model();
	fishLight_M.LoadModel("Models/foco_pez.obj");
	lamp_M = Model();
	lamp_M.LoadModel("Models/lampara_00_UV.obj");
	ship_M = Model();
	ship_M.LoadModel("Models/nave.obj");

	// Inicialización del skybox
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	Material_opaco = Material(0.3f, 4);
	Material_brillante = Material(1.0f, 32);

	unsigned int changeLightCount = 0;
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	// Luz ambiental
	mainLight = DirectionalLight(
		1.00f,  1.00f,  1.00f,
		0.25f,  0.10f,
		0.00f, -1.00f,  0.00f
	);

	// Lámpara azul del pez
	changeLights[0] = PointLight(
		0.00f,  0.00f,  1.00f,
		0.50f,  0.10f,
		0.00f,  0.00f,  0.00f,
		0.50f, -0.01f,  0.002f
	);
	changeLightCount++;

	// Luz cyan para la lámpara
	changeLights[1] = PointLight(
		0.00f,  1.00f,  1.00f,
		0.50f,  0.10f,
		0.00f,  0.00f,  0.00f,
		1.00f,  0.30f,  0.00f
	);
	changeLightCount++;

	// Faro frontal de la nave
	shipLights[0] = SpotLight(
		1.00f,  1.00f,  0.00f,
		0.10f,  1.00f,
		0.00f,  0.00f,  0.00f,
	   -1.00f, -1.00f,  0.00f,
		1.00f,  0.05f,  0.00f,
		10.00f
	);

	// Faro trasero de la nave
	shipLights[1] = SpotLight(
		1.00f,  1.00f,  0.00f,
		0.10f,  1.00f,
		0.00f,  0.00f,  0.00f,
		1.00f, -1.00f,  0.00f,
		1.00f,  0.05f,  0.00f,
		10.00f
	);

	// Asignación inicial
	spotLights[0] = shipLights[0];
	spotLightCount++;

	// Faro rojo del pez
	spotLights[1] = SpotLight(
		1.00f,  0.00f,  0.00f,
		0.80f,  2.50f,
		0.00f,  0.00f,  0.00f,
		0.00f, -1.00f,  0.00f,
		1.00f,  0.05f,  0.00f,
		30.0f
	);
	spotLightCount++;

	// Matrices de modelo
	glm::mat4 model(1.0);
	glm::mat4 modelAux(1.0);
	glm::mat4 modelLight(1.0);

	// Color blanco
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	// Uniformes
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// Variable para rastrear la posición anterior de la nave
	GLfloat lastShipX = mainWindow.getmuevex();
	GLfloat currentShipX;

	// Control de las luces de las lámparas
	bool lampOn = true;
	bool fishLightOn = true;
	bool lastLampKey = false;
	bool lastFishLightKey = false;

	// Variables para el foco del pez
	glm::vec3 fishFlashDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	// Loop principal
	while (!mainWindow.getShouldClose())
	{
		// Calcular delta time
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Reiniciar contadores de luces
		pointLightCount = 0;

		// Switch para la luz de la lámpara
		bool lampKey = mainWindow.getsKeys()[GLFW_KEY_O];
		if (lampKey && !lastLampKey)
		{
			lampOn = !lampOn;
		}
		lastLampKey = lampKey;

		// Switch para la luz del foco del pez
		bool fishLightKey = mainWindow.getsKeys()[GLFW_KEY_I];
		if (fishLightKey && !lastFishLightKey)
		{
			fishLightOn = !fishLightOn;
		}
		lastFishLightKey = fishLightKey;

		// Eventos de usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Limpiar la ventana
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		// Informacion al shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Información de luces
		shaderList[0].SetDirectionalLight(&mainLight);

		// Renderizar piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		// Nave
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), 20.0f, 0.0f));
		modelLight = model;
		model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ship_M.RenderModel();

		// Intercambiar luces según la dirección
		currentShipX = mainWindow.getmuevex();
		if (currentShipX > lastShipX)
		{
			modelLight = glm::translate(modelLight, glm::vec3(-2.0f, 1.0f, 0.0f));
			spotLights[0] = shipLights[1];
		}
		else if (currentShipX < lastShipX)
		{
			modelLight = glm::translate(modelLight, glm::vec3(2.0f, 1.0f, 0.0f));
			spotLights[0] = shipLights[0];
		}
		lastShipX = currentShipX;
		spotLights[0].SetPos(modelLight);

		// Pez
		// Cuerpo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, mainWindow.getDiagonal() + 30.0f, mainWindow.getDiagonal()));
		modelAux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.3f, 1.0f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		fishBody_M.RenderModel();

		// Antena
		model = modelAux;
		model = glm::translate(model, glm::vec3(1.3f, 6.7f, -2.28));
		modelAux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		fishAntenna_M.RenderModel();

		// Foco
		model = modelAux;
		model = glm::translate(model, glm::vec3(-0.3f, 8.85f, 6.6f));
		modelLight = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		fishLight_M.RenderModel();

		// Luz del foco
		changeLights[0].SetPos(modelLight);

		// Faro del foco
		fishFlashDirection = glm::vec3(
					sin(glm::radians(mainWindow.getXMove())),
			-1.0f + cos(glm::radians(mainWindow.getYMove())),
					sin(glm::radians(mainWindow.getZMove()))
		);
		fishFlashDirection = glm::normalize(fishFlashDirection);
		spotLights[1].SetFlash(modelLight, fishFlashDirection);

		// Lámpara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f));
		modelLight = model;
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		lamp_M.RenderModel();

		// Luz de la lámpara
		changeLights[1].SetPos(modelLight);

		// Cambiar el arreglo de luces según el estado de los switches
		if (lampOn)
		{
			pointLights[pointLightCount] = changeLights[1];
			pointLightCount++;
		}

		if (fishLightOn)
		{
			pointLights[pointLightCount] = changeLights[0];
			pointLightCount++;
		}

		// Enviar las luces al shader
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// Textura de agua en la cima de la pesera
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 55.01f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		waterTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		// Pecera
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 30.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 15.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		fishTank_M.RenderModel();
		glDisable(GL_BLEND);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}