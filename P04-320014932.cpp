#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

// Función para crear un cubo unitario
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

// Función para crear una pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, -0.25f,
		0.0f, -0.5f, -0.5f
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
}

// Función para crear un cilindro
void CrearCilindro(int res, float R)
{
	int n;
	GLfloat dt = 2 * PI / res, x, z;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	// Centro inferior (índice 0)
	vertices.push_back(0.0f);
	vertices.push_back(-0.5f);
	vertices.push_back(0.0f);

	// Centro superior (índice 1)
	vertices.push_back(0.0f);
	vertices.push_back(0.5f);
	vertices.push_back(0.0f);

	// Vértices del contorno (inferior y superior)
	int start_idx = 2;
	for (n = 0; n <= res; n++) {
		x = R * cos(n * dt);
		z = R * sin(n * dt);

		// Vértice inferior
		vertices.push_back(x);
		vertices.push_back(-0.5f);
		vertices.push_back(z);

		// Vértice superior
		vertices.push_back(x);
		vertices.push_back(0.5f);
		vertices.push_back(z);
	}

	// Se generan los índices para la topología de GL_TRIANGLES
	for (n = 0; n < res; n++) {
		int i0 = start_idx + n * 2;
		int i1 = start_idx + n * 2 + 1;
		int i2 = start_idx + n * 2 + 2;
		int i3 = start_idx + n * 2 + 3;

		// Dos triángulos para la cara lateral
		indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
		indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);

		// Triángulo para la tapa inferior
		indices.push_back(0); indices.push_back(i2); indices.push_back(i0);

		// Triángulo para la tapa superior
		indices.push_back(1); indices.push_back(i1); indices.push_back(i3);
	}

	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

// Función para crear un cono
void CrearCono(int res, float R)
{
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);

	for (i = 0; i < res + 2; i++) indices.push_back(i);

	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

// Función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4
	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

// Función para crear los shaders
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();

	CrearCubo();
	CrearPiramideTriangular();
	CrearCilindro(20, 0.5f);
	CrearCono(25, 0.5f);
	CrearPiramideCuadrangular();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init(); 
	sp.load();

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelbase(1.0);
	glm::mat4 modelCuerpo(1.0);
	glm::mat4 modelCabeza(1.0);
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		
		// Recibir eventos del usuario
		glfwPollEvents();
		
		// Control de cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Limpiar la ventana
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		// Enviar matrices de cámara y proyección al shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//// Actividad 1: Dibujo de grúa con jerarquía de transformaciones
		//// --- Dibujar cabina ---
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 6.0f, -4.0f));
		//modelaux = model; 
		//modelbase = model; 
		//model = glm::scale(model, glm::vec3(8.0f, 4.0f, 4.0f));
		//
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//color = glm::vec3(0.0f, 1.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//meshList[0]->RenderMesh();

		//// --- Brazo de la grúa ---
		//model = modelaux;
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		//// Primer brazo
		//model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[0]->RenderMesh(); 

		//// Segunda articulación
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		//
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//sp.render();

		//// Segundo brazo
		//model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[0]->RenderMesh(); 

		//// Tercera articulación
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		//
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//sp.render();

		//// Tercer brazo
		//model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		//
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[0]->RenderMesh(); 

		//// Cuarta articulación
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		//
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//sp.render();

		//// Canasta
		//model = glm::translate(model, glm::vec3(-2.0f, 2.5f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(4.0f, 5.0f, 4.0f));
		//
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.0f, 1.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[0]->RenderMesh(); 

		//// --- Base de la grúa y Ruedas ---
		//// Base
		//model = modelbase;
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(8.0f, 4.0f, 4.0f));
		//
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[4]->RenderMesh();

		//// Primera rueda 
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(-4.0f, -2.0f, 2.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 1.0f, 0.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[2]->RenderMesh();

		//// Cubo indicador en la primera rueda
		//glm::mat4 wheelCube = model;
		//wheelCube = glm::translate(wheelCube, glm::vec3(0.0f, 0.5f, 0.0f));
		//wheelCube = glm::scale(wheelCube, glm::vec3(0.3f, 0.3f, 0.3f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wheelCube));
		//color = glm::vec3(1.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//meshList[0]->RenderMesh();

		//// Segunda rueda
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(4.0f, -2.0f, 2.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[2]->RenderMesh();

		//// Cubo indicador en la segunda rueda
		//wheelCube = model;
		//wheelCube = glm::translate(wheelCube, glm::vec3(0.0f, 0.5f, 0.0f));
		//wheelCube = glm::scale(wheelCube, glm::vec3(0.3f, 0.3f, 0.3f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wheelCube));
		//color = glm::vec3(1.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//meshList[0]->RenderMesh();

		//// Tercera rueda
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(-4.0f, -2.0f, -2.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 1.0f, 0.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[2]->RenderMesh();

		//// Cubo indicador en la tercera rueda
		//wheelCube = model;
		//wheelCube = glm::translate(wheelCube, glm::vec3(0.0f, -0.5f, 0.0f));
		//wheelCube = glm::scale(wheelCube, glm::vec3(0.3f, 0.3f, 0.3f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wheelCube));
		//color = glm::vec3(1.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//meshList[0]->RenderMesh();

		//// Cuarta rueda
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(4.0f, -2.0f, -2.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 1.0f, 0.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		//meshList[2]->RenderMesh();

		//// Cubo indicador en la cuarta rueda
		//wheelCube = model;
		//wheelCube = glm::translate(wheelCube, glm::vec3(0.0f, -0.5f, 0.0f));
		//wheelCube = glm::scale(wheelCube, glm::vec3(0.3f, 0.3f, 0.3f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wheelCube));
		//color = glm::vec3(1.0f, 0.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//meshList[0]->RenderMesh();

		//Actividad 2:
		// --- Cuerpo de la araña ---
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		modelCuerpo = model;
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// --- Cabeza de la araña ---
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		modelCabeza = model;
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.45f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// Articulación para el colmillo derecho
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(-0.35f, -0.5f, -1.9f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion2()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// --- Colmillo derecho de la araña ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.25f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Articulación para el colmillo izquierdo
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(0.35f, -0.5f, -1.9f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion1()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// --- Colmillo izquierdo de la araña ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.25f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// --- Ojo derecho  de la araña ---
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(-0.5f, 0.5f, -1.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// --- Ojo izquierdo de la araña ---
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(0.5f, 0.5f, -1.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// --- Patas de la araña (derechas) ---
		// Pata 1 derecha
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, -1.75f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.45f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Pata 2 derecha
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-2.75f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.45f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Pata 3 derecha
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 1.75f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.45f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// --- Patas de la araña (izquierdas) ---
		// Pata 1 izquierda
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, -1.75f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.45f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Pata 2 izquierda
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(2.75f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.45f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Pata 3 izquierda
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 1.75f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.45f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
