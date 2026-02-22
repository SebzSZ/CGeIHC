#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>
#include <stdlib.h>
#include <time.h>

// Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;

// Variables globales de OpenGL
GLuint VAO, VBO, shader;

// Estructura para el color RGBA
struct Colour
{
	float r, g, b, a;
};

// Actividad 1: Cambio aleatorio del color de fondo
Colour backgroundColour;
float lastChangeTime = glfwGetTime();
const float CHANGE_INTERVAL = 2.0f;
Colour GenerateRandomColour();

// Vertex Shader: Definir la posición de los vértices
static const char* vShader = "						\n\
#version 330										\n\
layout (location = 0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x, pos.y, pos.z, 1.0f); 		\n\
}";

//Fragment Shader: Definir el color de los fragmentos
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(0.3843f, 0.0f, 0.0431f, 1.0f); 			\n\
}";

void CrearTriangulo()
{
	// Actividad 2: Dibujar iniciales "S", "B" y "L" en diagonal
	GLfloat vertices[] = {
		// Letra S
		-0.8f, -0.5f, 0.0f,
		-0.8f, -0.6f, 0.0f,
		-0.7f, -0.6f, 0.0f,

		-0.8f, -0.6f, 0.0f,
		-0.7f, -0.7f, 0.0f,
		-0.6f, -0.6f, 0.0f,

		-0.7f, -0.7f, 0.0f,
		-0.6f, -0.6f, 0.0f,
		-0.6f, -0.7f, 0.0f,

		-0.6f, -0.7f, 0.0f,
		-0.6f, -0.6f, 0.0f,
		-0.45f, -0.6f, 0.0f,

		-0.45f, -0.6f, 0.0f,
		-0.6f, -0.6f, 0.0f,
		-0.6f, -0.5f, 0.0f,

		-0.6f, -0.5f, 0.0f,
		-0.45f, -0.6f, 0.0f,
		-0.45f, -0.5f, 0.0f,

		-0.45f, -0.5f, 0.0f,
		-0.6f, -0.5f, 0.0f,
		-0.6f, -0.4f, 0.0f,

		-0.6f, -0.4f, 0.0f,
		-0.6f, -0.5f, 0.0f,
		-0.65f, -0.4f, 0.0f,

		-0.65f, -0.4f, 0.0f,
		-0.6f, -0.5f, 0.0f,
		-0.65f, -0.5f, 0.0f,

		-0.65f, -0.5f, 0.0f,
		-0.8f, -0.4f, 0.0f,
		-0.65f, -0.4f, 0.0f,

		-0.65f, -0.4f, 0.0f,
		-0.8f, -0.4f, 0.0f,
		-0.8f, -0.3f, 0.0f,

		-0.8f, -0.3f, 0.0f,
		-0.65f, -0.4f, 0.0f,
		-0.65f, -0.3f, 0.0f,

		-0.65f, -0.3f, 0.0f,
		-0.8f, -0.3f, 0.0f,
		-0.65f, -0.2f, 0.0f,

		-0.65f, -0.2f, 0.0f,
		-0.65f, -0.3f, 0.0f,
		-0.55f, -0.2f, 0.0f,

		-0.55f, -0.2f, 0.0f,
		-0.65f, -0.3f, 0.0f,
		-0.45f, -0.3f, 0.0f,

		-0.45f, -0.3f, 0.0f,
		-0.45f, -0.4f, 0.0f,
		-0.55f, -0.3f, 0.0f,

		// Letra B
		-0.2f, -0.25f, 0.0f,
		-0.1f, -0.15f, 0.0f,
		-0.2f, 0.0f, 0.0f,

		-0.2f, -0.25f, 0.0f,
		-0.1f, -0.15f, 0.0f,
		0.0f, -0.25f, 0.0f,

		0.0f, -0.25f, 0.0f,
		-0.1f, -0.15f, 0.0f,
		0.05f, -0.15f, 0.0f,

		0.05f, -0.15f, 0.0f,
		0.0f, -0.25f, 0.0f,
		0.1f, -0.25f, 0.0f,

		0.1f, -0.25f, 0.0f,
		0.05f, -0.15f, 0.0f,
		0.15f, -0.15f, 0.0f,

		0.15f, -0.15f, 0.0f,
		0.05f, -0.15f, 0.0f,
		0.05f, 0.0f, 0.0f,

		0.05f, 0.0f, 0.0f,
		0.15f, 0.0f, 0.0f,
		0.15f, -0.15f, 0.0f,

		0.05f, 0.0f, 0.0f,
		0.15f, 0.0f, 0.0f,
		0.05f, 0.1f, 0.0f,

		0.05f, 0.1f, 0.0f,
		0.05f, 0.0f, 0.0f,
		-0.1f, 0.1f, 0.0f,

		-0.1f, 0.1f, 0.0f,
		0.05f, 0.0f, 0.0f,
		-0.1f, 0.0f, 0.0f,

		-0.1f, -0.15f, 0.0f,
		-0.2f, 0.0f, 0.0f,
		-0.1f, 0.1f, 0.0f,

		-0.1f, 0.1f, 0.0f,
		-0.2f, 0.0f, 0.0f,
		-0.2f, 0.25f, 0.0f,

		-0.2f, 0.25f, 0.0f,
		-0.1f, 0.25f, 0.0f,
		-0.1f, 0.1f, 0.0f,

		-0.1f, 0.2f, 0.0f,
		-0.1f, 0.25f, 0.0f,
		0.05f, 0.25f, 0.0f,

		0.05f, 0.25f, 0.0f,
		-0.1f, 0.2f, 0.0f,
		0.05f, 0.2f, 0.0f,

		0.05f, 0.2f, 0.0f,
		0.05f, 0.25f, 0.0f,
		0.15f, 0.2f, 0.0f,

		0.15f, 0.2f, 0.0f,
		0.05f, 0.2f, 0.0f,
		0.15f, 0.15f, 0.0f,

		0.15f, 0.15f, 0.0f,
		0.05f, 0.2f, 0.0f,
		0.05f, 0.15f, 0.0f,

		0.05f, 0.15f, 0.0f,
		0.15f, 0.15f, 0.0f,
		0.05f, 0.0f, 0.0f,

		// Letra L
		0.4f, 0.2f, 0.0f,
		0.4f, 0.7f, 0.0f,
		0.5f, 0.2f, 0.0f,

		0.5f, 0.2f, 0.0f,
		0.4f, 0.7f, 0.0f,
		0.5, 0.7f, 0.0f,

		0.5f, 0.2f, 0.0f,
		0.5f, 0.3, 0.0f,
		0.75f, 0.2f, 0.0f,

		0.75f, 0.2f, 0.0f,
		0.75f, 0.3f, 0.0f,
		0.5f, 0.3, 0.0f
	};

	// Generara y asignar VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Generar y asignar VBO con los vértices
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configurar los atributos de los vértices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Desvincular VBO y VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	// Crear el shader
	GLuint theShader = glCreateShader(shaderType);

	// Asignar el código al shader
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);
	glShaderSource(theShader,1, theCode, codeLength);

	// Compilar el shader
	glCompileShader(theShader);

	// Verificar errores de compilación
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}

	// Adjuntar el shader al programa
	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	// Crear el programa de shader
	shader= glCreateProgram();
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}

	// Agregar vertex y fragment shaders
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	// Vincular el programa de shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}

	// Validar el programa de shader
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}
}

int main()
{
	// Inicializar GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	// Configurar GLFW para usar OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Configurar GLFW para usar el perfil core de OpenGL sin retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Crear la ventana principal
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);
	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}

	// Obtener dimensiones del buffer de la ventana
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	// Asignar el contexto de OpenGL a la ventana
	glfwMakeContextCurrent(mainWindow);

	// Inicializar GLEW con soporte para extensiones
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Configurar el viewport de OpenGL
	glViewport(0, 0, BufferWidth, BufferHeight);

	// Semilla aleatoria para la generación de colores
	srand(time(NULL));

	// Asignación del primer color de fondo
	backgroundColour = GenerateRandomColour();

	// Inicializar los datos de los triángulos y compilar los shaders
	CrearTriangulo();
	CompileShaders();

	// Loop principal de renderizado
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Obtener tiempo actual
		double currentTime = glfwGetTime();

		// Realizar un cambio cada intervalo definido
		if (currentTime - lastChangeTime >= CHANGE_INTERVAL)
		{
			backgroundColour = GenerateRandomColour();
			lastChangeTime = currentTime;
		}

		// Establecer el color de fondo y limpiar el buffer de color
		glClearColor(backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
		glClear(GL_COLOR_BUFFER_BIT);

		// Recibir eventos de entrada
		glfwPollEvents();

		// Renderizar la geometría
		glUseProgram(shader);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 117);
		glBindVertexArray(0);
		glUseProgram(0);

		// Intercambiar buffers
		glfwSwapBuffers(mainWindow);
	}
	return 0;
}

// Función para generar un color aleatorio
Colour GenerateRandomColour()
{
	// Estructura de color RGBA
	Colour color;

	// Obtener valores aleatorios para RGB
	color.r = (float)rand() / RAND_MAX;
	color.g = (float)rand() / RAND_MAX;
	color.b = (float)rand() / RAND_MAX;

	// Opacidad fija en 1.0
	color.a = 1.0f;

	// Devolver el color generado
	return color;
}