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
Skybox skybox;
Texture pisoTexture;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Modelos
Model Blackhawk_M;
Model Lamp_M;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Iluminación
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

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
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

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
	// Creación de la ventana
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	// Creación de objetos y shaders
	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(-4.0f, 5.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	// Inicialización de modelos
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Lamp_M = Model();
	Lamp_M.LoadModel("Models/lampara_00_UV.obj");

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

	// Luz ambiental
	mainLight = DirectionalLight(
		1.0f, 1.0f, 1.0f,
		0.35f, 0.2f,
		0.0f, -1.0f, -1.0f
	);

	// Contador de luces
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	// Spotlight amarrillo apuntando hacia abajo
	spotLights[0] = SpotLight(
		1.0f, 1.0f, 0.0f,
		0.65f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.0f,
		30.0f
	);
	spotLightCount++;

	// Pointlight cyan para la lámpara
	pointLights[0] = PointLight(
		0.0f, 1.0f, 1.0f,
		0.5f, 0.1f,
		15.0f, 1.5f, 15.0f,
		1.0f, 0.3f, 0.0f
	);
	pointLightCount++;

	// Posición de la luz del helicóptero
	glm::vec4 lightWorldPos;
	glm::vec4 lightLocalPos = glm::vec4(0.0f, -2.0f, 0.0f, 1.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// Loop principal
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

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
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// Renderizar piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		// Helicóptero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), 10.0f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		// Luz del helicóptero
		lightWorldPos = modelaux * lightLocalPos;
		spotLights[0].SetPos(glm::vec3(lightWorldPos));
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// Lámpara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 15.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lamp_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}