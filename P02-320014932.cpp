#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

// Grados a radianes
const float toRadians = 3.14159265f / 180.0f;

// Valores globales
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader> shaderList;
float angulo = 0.0f;

// Shaders base
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";

// Shaders con color en VAO
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

// Shaders de colores específicos
static const char* fShaderRojo = "shaders/shaderrojo.frag";
static const char* fShaderVerde = "shaders/shaderverde.frag";
static const char* fShaderAzul = "shaders/shaderazul.frag";
static const char* fShaderVerdeOscuro = "shaders/shaderverdeoscuro.frag";
static const char* fShaderCafe = "shaders/shadercafe.frag";

// Vértices para crear una pírámide triangular regular
void CreaPiramide()
{
	unsigned int indices[] = {
		0, 1, 2,
		1, 3, 2,
		3, 0, 2,
		1, 0, 3
	};

	GLfloat vertices[] = {
		-0.5f, -0.5f,  0.0f,    // 0
		 0.5f, -0.5f,  0.0f,    // 1
		 0.0f,  0.5f, -0.25f,   // 2
		 0.0f, -0.5f, -0.5f     // 3
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
}

// Vértices para crear un cubo
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Crear figuras con color definido en los vértices
void CrearLetrasyFiguras()
{
	GLfloat vertices_letras[] = {
		// X		Y			Z			R		G		B
		// Letra S
		-0.80f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.80f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.70f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.80f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.70f,		-0.70f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.70f,		-0.70f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.70f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.60f,		-0.70f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.45f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.45f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.60f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.45f,		-0.60f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.45f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.45f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.60f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.65f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.65f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.60f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.65f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.65f,		-0.50f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.80f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.65f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.65f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.80f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.80f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.80f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.65f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.65f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.65f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.80f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.65f,		-0.20f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.65f,		-0.20f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.65f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.55f,		-0.20f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.55f,		-0.20f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.65f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.45f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,

		-0.45f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.45f,		-0.40f,		0.5f,		1.0f,	0.0f,	0.0f,
		-0.55f,		-0.30f,		0.5f,		1.0f,	0.0f,	0.0f,

		// Letra B
		-0.20f,		-0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.20f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,

		-0.20f,		-0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.00f,		-0.25f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.00f,		-0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.05f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.00f,		-0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.10f,		-0.25f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.10f,		-0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.15f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.15f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.05f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.15f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.15f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.05f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.15f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.10f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.05f,		0.10f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		0.10f,		0.5f,		0.0f,	1.0f,	0.0f,

		-0.10f,		0.10f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,

		-0.10f,		-0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.20f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		0.10f,		0.5f,		0.0f,	1.0f,	0.0f,

		-0.10f,		0.10f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.20f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.20f,		0.25f,		0.5f,		0.0f,	1.0f,	0.0f,

		-0.20f,		0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		0.10f,		0.5f,		0.0f,	1.0f,	0.0f,

		-0.10f,		0.20f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.25f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.05f,		0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		-0.10f,		0.20f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.20f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.05f,		0.20f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.25f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.15f,		0.20f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.15f,		0.20f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.20f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.15f,		0.15f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.15f,		0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.20f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.15f,		0.5f,		0.0f,	1.0f,	0.0f,

		 0.05f,		0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.15f,		0.15f,		0.5f,		0.0f,	1.0f,	0.0f,
		 0.05f,		0.00f,		0.5f,		0.0f,	1.0f,	0.0f,

		 // Letra L
		  0.40f,	0.20f,		0.5f,		0.0f,	0.0f,	1.0f,
		  0.40f,	0.70f,		0.5f,		0.0f,	0.0f,	1.0f,
		  0.50f,	0.20f,		0.5f,		0.0f,	0.0f,	1.0f,

		  0.50f,	0.20f,		0.5f,		0.0f,	0.0f,	1.0f,
		  0.40f,	0.70f,		0.5f,		0.0f,	0.0f,	1.0f,
		  0.50f,	0.70f,		0.5f,		0.0f,	0.0f,	1.0f,

		  0.50f,	0.20f,		0.5f,		0.0f,	0.0f,	1.0f,
		  0.50f,	0.30f,		0.5f,		0.0f,	0.0f,	1.0f,
		  0.75f,	0.20f,		0.5f,		0.0f,	0.0f,	1.0f,

		  0.75f,	0.20f,		0.5f,		0.0f,	0.0f,	1.0f,
		  0.75f,	0.30f,		0.5f,		0.0f,	0.0f,	1.0f,
		  0.50f,	0.30f,		0.5f,		0.0f,	0.0f,	1.0f
	};
	MeshColor* letras = new MeshColor();
	letras->CreateMeshColor(vertices_letras, 702);
	meshColorList.push_back(letras);

	GLfloat vertices_triangulorojo[] = {
		// X	Y		Z       R		G		B
		-1.0f,	-1.0f,  0.5f,	1.0f,	0.0f,	0.0f,
		1.0f,	-1.0f,  0.5f,	1.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.5f,	1.0f,	0.0f,	0.0f
	};
	MeshColor* triangulorojo = new MeshColor();
	triangulorojo->CreateMeshColor(vertices_triangulorojo, 18);
	meshColorList.push_back(triangulorojo);

	GLfloat vertices_trianguloazul[] = {
		// X	Y       Z       R		G		B
		-1.0f,	-1.0f,  0.5f,	0.0f,	0.0f,	1.0f,
		1.0f,	-1.0f,  0.5f,	0.0f,	0.0f,	1.0f,
		0.0f,  1.0f,	0.5f,	0.0f,	0.0f,	1.0f
	};
	MeshColor* trianguloazul = new MeshColor();
	trianguloazul->CreateMeshColor(vertices_trianguloazul, 18);
	meshColorList.push_back(trianguloazul);

	GLfloat vertices_cuadradoverde[] = {
		// X	Y       Z       R		G		B
		-0.5f,	-0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,  0.5f,	0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,  0.5f,	0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		-0.5f,  0.5f,	0.5f,	0.0f,	1.0f,	0.0f
	};
	MeshColor* cuadradoverde = new MeshColor();
	cuadradoverde->CreateMeshColor(vertices_cuadradoverde, 36);
	meshColorList.push_back(cuadradoverde);

	GLfloat vertices_cuadradorojo[] = {
		// X	Y       Z       R		G		B
		-0.5f,	-0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
		0.5f,	-0.5f,  0.5f,	1.0f,	0.0f,	0.0f,
		0.5f,	0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
		-0.5f,	-0.5f,  0.5f,	1.0f,	0.0f,	0.0f,
		0.5f,	0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,	0.5f,	1.0f,	0.0f,	0.0f
	};
	MeshColor* cuadradorojo = new MeshColor();
	cuadradorojo->CreateMeshColor(vertices_cuadradorojo, 36);
	meshColorList.push_back(cuadradorojo);
}

// Cargar Shaders en el índice correspondiente de shaderList
void CreateShaders()
{
	// Shader base para objetos con índices
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	// Shader para objetos con color definido
	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2);

	// Shader rojo
	Shader* shaderRojo = new Shader();
	shaderRojo->CreateFromFiles(vShader, fShaderRojo);
	shaderList.push_back(*shaderRojo);

	// Shader verde
	Shader* shaderVerde = new Shader();
	shaderVerde->CreateFromFiles(vShader, fShaderVerde);
	shaderList.push_back(*shaderVerde);

	// Shader azul
	Shader* shaderAzul = new Shader();
	shaderAzul->CreateFromFiles(vShader, fShaderAzul);
	shaderList.push_back(*shaderAzul);

	// Shader verde oscuro
	Shader* shaderVerdeOscuro = new Shader();
	shaderVerdeOscuro->CreateFromFiles(vShader, fShaderVerdeOscuro);
	shaderList.push_back(*shaderVerdeOscuro);

	// Shader café
	Shader* shaderCafe = new Shader();
	shaderCafe->CreateFromFiles(vShader, fShaderCafe);
	shaderList.push_back(*shaderCafe);
}

int main()
{
	// Inicialización de ventana y geometría
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	CreaPiramide();
	CrearCubo();
	CrearLetrasyFiguras();
	CreateShaders();
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

	// Configuración de proyección. Ortogonal para Actividad 1, Perspectiva para Actividad 2
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	// glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

	glm::mat4 model(1.0);

	// Loop principal
	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//// Actividad 1: Dibujo de letras S, B y L con colores diferentes
		//shaderList[1].useShader();
		//uniformModel = shaderList[1].getModelLocation();
		//uniformProjection = shaderList[1].getProjectLocation();

		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//meshColorList[0]->RenderMeshColor();

		// Actividad 2: Dibujo de una casa con cubos y pirámides
		// Cuerpo principal (cubo rojo)
		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		// Ventana izquierda (cubo verde)
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.3f, 0.3f, -2.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, glm::radians(5.75f), glm::vec3(1.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		// Ventana derecha (cubo verde)
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.3f, 0.3f, -2.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, glm::radians(5.75f), glm::vec3(1.0f, -1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		// Entrada (cubo verde)
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.36f, -2.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		// Techo (pirámide azul)
		shaderList[4].useShader();
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, -3.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.75f, 2.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		// Maceta izquierda (cubo café)
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.75f, -0.36f, -2.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		// Maceta derecha (cubo café)
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.75f, -0.36f, -2.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, -1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		// Planta izquierda (pirámide verde oscura)
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.7f, 0.25f, -1.8f));
		model = glm::scale(model, glm::vec3(0.4f, 1.0f, 0.5f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		// Planta derecha (pirámide verde oscura)
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.7f, 0.25f, -1.8f));
		model = glm::scale(model, glm::vec3(0.4f, 1.0f, 0.5f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}