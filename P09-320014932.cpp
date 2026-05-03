#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <math.h>
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
#include "SpotLight.h"
#include "Texture.h"
#include "Window.h"

// Variables globales
Camera camera;
Window mainWindow;
Material Material_brillante;
Material Material_opaco;
Skybox skybox;
Texture pisoTexture;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Modelos
Model CarroBase;
Model CarroArriba;
Model CarroRueda1;
Model CarroRueda2;
Model CarroRueda3;
Model CarroRueda4;
Model CarroRin1;
Model CarroRin2;
Model CarroRin3;
Model CarroRin4;
Model Pista;
Model Nave;
Model Ala;
Model Ala2;
Model Helice1;
Model Helice2;

// Animación del carro
float carSpeed;
float carSpeedOffset;
float carRotation;
float carRotationOffset;
float carPitch;
float carPitchOffset;
float wheelRotation;
float wheelRotationOffset;
float carHeight;

// Waypoints
std::vector<glm::vec3> routePoints = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(-30.0f, 0.5f, 3.0f),
    glm::vec3(-55.0f, 1.2f, 11.0f),
    glm::vec3(-80.0f, 7.0f, -9.5f),
    glm::vec3(-100.0f, 17.0f, -4.6f),
    glm::vec3(-142.0f, 28.0f, -10.0f),
    glm::vec3(-180.0f, 25.0f, -23.0f)
};

// Estado del recorrido
int currentPoint;
float distanceTraveled;
glm::vec3 carPosition;
glm::vec3 carDirection;
float carYrotation;
float targetYrotation;
float carInclination;
float targetInclination;
bool carShouldStop;

// Entrada
bool keyRwasPressed;

// Animación de la nave
int naveCurrentPoint;
float naveDistanceTraveled;
glm::vec3 navePosition;
glm::vec3 naveDirection;
float naveYrotation;
float naveTargetYrotation;
float naveInclination;
float naveTargetInclination;
bool naveHasArrived;
bool naveIsDescending;
float naveSpeedOffset;
float naveRotationOffset;
float navePitchOffset;
float naveHeight;
float naveDescentSpeed;
float heliceRotation;
float heliceRotationSpeed;
float alaRotation;
float alaRotationSpeed;
float alaDirection;

// Delta time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Luz
DirectionalLight mainLight;

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
        -10.0f, 0.0f, -10.0f,  0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
         10.0f, 0.0f, -10.0f, 10.0f,  0.0f,  0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f,  10.0f,  0.0f, 10.0f,  0.0f, -1.0f, 0.0f,
         10.0f, 0.0f,  10.0f, 10.0f, 10.0f,  0.0f, -1.0f, 0.0f
    };

    Mesh* floorMesh = new Mesh();
    floorMesh->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(floorMesh);
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

void ResetAnimation()
{
    currentPoint = 0;
    distanceTraveled = 0.0f;
    carPosition = routePoints[0];
    carDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    carInclination = 0.0f;
    targetInclination = 0.0f;
    wheelRotation = 0.0f;
    carShouldStop = false;

    if (routePoints.size() >= 2)
    {
        glm::vec3 segment = routePoints[1] - routePoints[0];
        targetYrotation = glm::radians(atan2(segment.x, segment.z));
        carYrotation = targetYrotation;
    }
}

void InitializeShipAnimation()
{
    naveCurrentPoint = (int)routePoints.size() - 2;
    naveDistanceTraveled = 0.0f;
    navePosition = routePoints[routePoints.size() - 1] + glm::vec3(0.0f, 10.0f, 0.0f);
    naveDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    naveYrotation = 0.0f;
    naveInclination = 0.0f;
    naveTargetInclination = 0.0f;
    naveHasArrived = false;
    naveIsDescending = false;
    heliceRotation = 0.0f;
    alaRotation = 0.0f;
    alaDirection = 1.0f;
    naveDescentSpeed = 0.5f;

    if (naveCurrentPoint >= 0)
    {
        glm::vec3 segment = routePoints[naveCurrentPoint] - routePoints[naveCurrentPoint + 1];
        naveTargetYrotation = atan2(segment.x, segment.z) * (180.0f / 3.14159265f);
        naveYrotation = naveTargetYrotation;
    }
}

int main()
{
    // Crear ventana
    mainWindow = Window(1366, 768);
    mainWindow.Initialise();

    // Crear objetos y shaders
    CreateObjects();
    CreateShaders();

    // Configurar cámara
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

    // Cargar texturas
    pisoTexture = Texture("Textures/piso.tga");
    pisoTexture.LoadTextureA();

    // Cargar modelos
    CarroBase = Model();
    CarroBase.LoadModel("Models/CarroBase.obj");
    CarroArriba = Model();
    CarroArriba.LoadModel("Models/CarroArriba.obj");
    CarroRueda1 = Model();
    CarroRueda1.LoadModel("Models/Llanta1.obj");
    CarroRueda2 = Model();
    CarroRueda2.LoadModel("Models/Llanta2.obj");
    CarroRueda3 = Model();
    CarroRueda3.LoadModel("Models/Llanta3.obj");
    CarroRueda4 = Model();
    CarroRueda4.LoadModel("Models/Llanta4.obj");
    CarroRin1 = Model();
    CarroRin1.LoadModel("Models/Rin1.obj");
    CarroRin2 = Model();
    CarroRin2.LoadModel("Models/Rin2.obj");
    CarroRin3 = Model();
    CarroRin3.LoadModel("Models/Rin3.obj");
    CarroRin4 = Model();
    CarroRin4.LoadModel("Models/Rin4.obj");

    Pista = Model();
    Pista.LoadModel("Models/pista.obj");
    Nave = Model();
    Nave.LoadModel("Models/nave.obj");
    Ala = Model();
    Ala.LoadModel("Models/ala.obj");
	Ala2 = Model();
	Ala2.LoadModel("Models/ala2.obj");
	Helice1 = Model();
	Helice1.LoadModel("Models/helice1.obj");
    Helice2 = Model();
	Helice2.LoadModel("Models/helice2.obj");

    // Cargar skybox
    std::vector<std::string> skyboxFaces;
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
    skybox = Skybox(skyboxFaces);

    // Materiales
    Material_brillante = Material(4.0f, 256);
    Material_opaco = Material(0.3f, 4);

    // Luz direccional
    mainLight = DirectionalLight(
        1.0f, 1.0f, 1.0f,
        0.5f, 0.5f,
        0.0f, -1.0f, -1.0f
    );

    // Spotlights
    unsigned int spotLightCount = 0;

    SpotLight spotLights[MAX_SPOT_LIGHTS];

    // Spotlight amarillo de la nave
    spotLights[0] = SpotLight(
        1.0f, 1.0f, 0.0f,
        0.65f, 2.0f,
        0.0f, 3.0f, 1.5f,
        0.0f, -1.0f, 0.0f,
        1.0f, 0.05f, 0.0f,
        30.0f
    );
    spotLightCount++;

    // Faro del carro (se actualiza cada frame)
    spotLights[1] = SpotLight(
        0.0f, 0.0f, 1.0f,
        0.65f, 2.0f,
        0.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        1.0f, 0.05f, 0.0f,
        30.0f
    );
    spotLightCount++;

    // Inicializar animación
    currentPoint = 0;
    distanceTraveled = 0.0f;
    carPosition = routePoints[0];
    carDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    carYrotation = 0.0f;
    carInclination = 0.0f;
    wheelRotation = 0.0f;
    carHeight = 1.25f;
    carShouldStop = false;
    keyRwasPressed = false;

    // Animación del carro
    carSpeedOffset = 0.4f;
    carRotationOffset = 0.5f;
    carPitchOffset = 0.5f;
    wheelRotationOffset = 1.0f;

    // Orientar hacia el segundo waypoint
    if (routePoints.size() >= 2)
    {
        glm::vec3 segment = routePoints[1] - routePoints[0];
        targetYrotation = atan2(segment.x, segment.z) * (180.0f / 3.14159265f);
        carYrotation = targetYrotation;
    }

    // Inicializar animación de la nave
    naveSpeedOffset = 0.4f;
    naveRotationOffset = 0.5f;
    navePitchOffset = 0.5f;
    naveHeight = 35.0f;
    heliceRotationSpeed = 10.0f;
    alaRotationSpeed = 2.0f;
    InitializeShipAnimation();

    // Configurar proyección
    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0,
        uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
    GLuint uniformColor = 0;

    glm::mat4 projection = glm::perspective(
        45.0f,
        (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(),
        0.1f, 1000.0f
    );

    // Matrices de modelo
    glm::mat4 model(1.0f);
    glm::mat4 modelAux(1.0f);
    glm::mat4 modelRin(1.0f);
    glm::vec3 color(1.0f);

    lastTime = glfwGetTime();


    // Loop principal
    while (!mainWindow.getShouldClose())
    {
        // Calcular delta time
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        // Entrada
        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Reset con tecla R (pulso único)
        bool keyRCurrent = mainWindow.getsKeys()[GLFW_KEY_R];
        if (keyRCurrent && !keyRwasPressed)
            ResetAnimation();
        keyRwasPressed = keyRCurrent;

        // Movimiento del carro por la ruta
        if (!carShouldStop && routePoints.size() >= 2)
        {
            glm::vec3 current = routePoints[currentPoint];
            glm::vec3 goal = routePoints[currentPoint + 1];
            glm::vec3 segment = goal - current;
            float segmentLength = glm::length(segment);

            if (segmentLength < 0.1f)
            {
                distanceTraveled = 0.0f;
                currentPoint++;
                if (currentPoint >= (int)routePoints.size() - 1)
                {
                    carPosition = routePoints.back();
                    carShouldStop = true;
                }
            }
            else
            {
                distanceTraveled += carSpeedOffset * deltaTime;

                while (distanceTraveled >= segmentLength && !carShouldStop)
                {
                    distanceTraveled -= segmentLength;
                    currentPoint++;

                    if (currentPoint >= (int)routePoints.size() - 1)
                    {
                        carPosition = routePoints.back();
                        carShouldStop = true;
                        break;
                    }

                    current = routePoints[currentPoint];
                    goal = routePoints[currentPoint + 1];
                    segment = goal - current;
                    segmentLength = glm::length(segment);
                    if (segmentLength < 0.1f) segmentLength = 0.1f;
                }

                if (!carShouldStop)
                {
                    glm::vec3 dirSegment = glm::normalize(segment);
                    carPosition = current + dirSegment * distanceTraveled;
                    carDirection = dirSegment;

                    // Rotación en Y
                    targetYrotation = atan2(dirSegment.x, dirSegment.z) * (180.0f / 3.14159265f);

					// Ajustar rotación para que tome el camino más corto
                    float diffYaw = targetYrotation - carYrotation;
                    if (diffYaw > 180.0f) diffYaw -= 360.0f;
                    if (diffYaw < -180.0f) diffYaw += 360.0f;
                    carYrotation += diffYaw * carRotationOffset * deltaTime;
                    if (carYrotation > 180.0f) carYrotation -= 360.0f;
                    if (carYrotation < -180.0f) carYrotation += 360.0f;

                    // Inclinación de la rampa
                    float horizontalDist = sqrtf(segment.x * segment.x + segment.z * segment.z);
                    targetInclination = atan2(segment.y, horizontalDist) * (180.0f / 3.14159265f);
                    float diffPitch = targetInclination - carInclination;
                    carInclination += diffPitch * carPitchOffset * deltaTime;

                    // Rotación de llantas
                    wheelRotation += wheelRotationOffset * deltaTime;
                    if (wheelRotation >= 360.0f) wheelRotation -= 360.0f;
                }
            }
        }

        // Faro del carro sigue al vehículo
        {
            glm::vec3 dirHorizontal = glm::vec3(carDirection.x, 0.0f, carDirection.z);
            glm::vec3 headlightPos = carPosition + glm::vec3(0.0f, carHeight + 1.65f, 0.0f) + dirHorizontal * 2.3f;
            glm::vec3 headlightDir = glm::normalize(dirHorizontal + glm::vec3(0.0f, -0.25f, 0.0f));
            spotLights[1].SetFlash(headlightPos, headlightDir);
        }

        // Movimiento de la nave por la ruta (hacia atrás)
        if (!naveIsDescending && !naveHasArrived && naveCurrentPoint >= 0 && routePoints.size() >= 2)
        {
            glm::vec3 current = routePoints[naveCurrentPoint + 1] + glm::vec3(0.0f, 10.0f, 0.0f);
            glm::vec3 goal = routePoints[naveCurrentPoint] + glm::vec3(0.0f, 10.0f, 0.0f);
            glm::vec3 segment = goal - current;
            float segmentLength = glm::length(segment);

            if (segmentLength < 0.1f)
            {
                naveDistanceTraveled = 0.0f;
                naveCurrentPoint--;
                if (naveCurrentPoint < 0)
                {
                    navePosition = routePoints[0] + glm::vec3(0.0f, 10.0f, 0.0f);
                    naveIsDescending = true;
                }
            }
            else
            {
                naveDistanceTraveled += naveSpeedOffset * deltaTime;

                while (naveDistanceTraveled >= segmentLength && !naveIsDescending)
                {
                    naveDistanceTraveled -= segmentLength;
                    naveCurrentPoint--;

                    if (naveCurrentPoint < 0)
                    {
                        navePosition = routePoints[0] + glm::vec3(0.0f, 10.0f, 0.0f);
                        naveIsDescending = true;
                        break;
                    }

                    current = routePoints[naveCurrentPoint + 1] + glm::vec3(0.0f, 10.0f, 0.0f);
                    goal = routePoints[naveCurrentPoint] + glm::vec3(0.0f, 10.0f, 0.0f);
                    segment = goal - current;
                    segmentLength = glm::length(segment);
                    if (segmentLength < 0.1f) segmentLength = 0.1f;
                }

                if (!naveIsDescending)
                {
                    glm::vec3 dirSegment = glm::normalize(segment);
                    navePosition = current + dirSegment * naveDistanceTraveled;
                    naveDirection = dirSegment;

                    // Rotación en Y
                    naveTargetYrotation = atan2(dirSegment.x, dirSegment.z) * (180.0f / 3.14159265f);

                    // Ajustar rotación para que tome el camino más corto
                    float diffYaw = naveTargetYrotation - naveYrotation;
                    if (diffYaw > 180.0f) diffYaw -= 360.0f;
                    if (diffYaw < -180.0f) diffYaw += 360.0f;
                    naveYrotation += diffYaw * naveRotationOffset * deltaTime;
                    if (naveYrotation > 180.0f) naveYrotation -= 360.0f;
                    if (naveYrotation < -180.0f) naveYrotation += 360.0f;

                    // Inclinación de la rampa
                    float horizontalDist = sqrtf(segment.x * segment.x + segment.z * segment.z);
                    naveTargetInclination = atan2(segment.y, horizontalDist) * (180.0f / 3.14159265f);
                    float diffPitch = naveTargetInclination - naveInclination;
                    naveInclination += diffPitch * navePitchOffset * deltaTime;
                }
            }
        }

        // Descenso gradual de la nave
        if (naveIsDescending && !naveHasArrived)
        {
            navePosition.y -= naveDescentSpeed * deltaTime;
            if (navePosition.y <= 5.0f)
            {
                navePosition.y = 5.0f;
                naveHasArrived = true;
            }
        }

        // Animación de hélices y alas mientras la nave se mueve
        if (!naveIsDescending)
        {
            heliceRotation += heliceRotationSpeed * deltaTime * 60.0f;
            if (heliceRotation >= 360.0f) heliceRotation -= 360.0f;

            alaRotation += alaRotationSpeed * deltaTime * alaDirection;
            if (alaRotation >= 20.0f) alaDirection = -1.0f;
            if (alaRotation <= -20.0f) alaDirection = 1.0f;
        }

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformEyePosition = shaderList[0].GetEyePositionLocation();
        uniformColor = shaderList[0].getColorLocation();
        uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
        uniformShininess = shaderList[0].GetShininessLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition,
            camera.getCameraPosition().x,
            camera.getCameraPosition().y,
            camera.getCameraPosition().z);

        shaderList[0].SetDirectionalLight(&mainLight);
        shaderList[0].SetSpotLights(spotLights, spotLightCount);

        model = glm::mat4(1.0f);
        modelAux = glm::mat4(1.0f);
        modelRin = glm::mat4(1.0f);
        color = glm::vec3(1.0f, 1.0f, 1.0f);

        // Piso
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        pisoTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[0]->RenderMesh();

        // Pista
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        Pista.RenderModel();

        // Carro
        model = glm::mat4(1.0f);
        model = glm::translate(model, carPosition + glm::vec3(0.0f, carHeight, 0.0f));
        model = glm::rotate(model, glm::radians(carInclination), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(carYrotation - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelAux = model;

        // Base del carro
        color = glm::vec3(0.2f, 0.2f, 0.2f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelAux));
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        CarroBase.RenderModel();

        // Parte superior del carro
        model = modelAux;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroArriba.RenderModel();

        // Rueda 1 - delantera derecha
        model = modelAux;
        model = glm::translate(model, glm::vec3(2.5f, 0.8f, -1.35f));
        model = glm::rotate(model, glm::radians(-wheelRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelRin = model;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroRueda1.RenderModel();

        model = modelRin;
        color = glm::vec3(0.0f, 0.0f, 0.9f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroRin1.RenderModel();

        // Rueda 2 - delantera izquierda
        model = modelAux;
        model = glm::translate(model, glm::vec3(2.5f, 0.8f, 1.35f));
        model = glm::rotate(model, glm::radians(-wheelRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelRin = model;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroRueda2.RenderModel();

        model = modelRin;
        color = glm::vec3(0.0f, 0.0f, 0.9f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroRin2.RenderModel();

        // Rueda 3 - trasera derecha
        model = modelAux;
        model = glm::translate(model, glm::vec3(-2.5f, 0.8f, -1.5f));
        model = glm::rotate(model, glm::radians(-wheelRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelRin = model;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroRueda3.RenderModel();

        model = modelRin;
        color = glm::vec3(0.0f, 0.0f, 0.9f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroRin3.RenderModel();

        // Rueda 4 - trasera izquierda
        model = modelAux;
        model = glm::translate(model, glm::vec3(-2.5f, 0.8f, 1.5f));
        model = glm::rotate(model, glm::radians(-wheelRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        modelRin = model;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroRueda4.RenderModel();

        model = modelRin;
        color = glm::vec3(0.0f, 0.0f, 0.9f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        CarroRin4.RenderModel();

		// Nave
		model = glm::mat4(1.0f);
		model = glm::translate(model, navePosition);
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(naveInclination), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(naveYrotation + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelAux = model;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nave.RenderModel();

		spotLights[0].SetPos(modelAux);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// Ala 1
		model = modelAux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.25f));
		model = glm::rotate(model, glm::radians(alaRotation), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala.RenderModel();

		// Ala 2
		model = modelAux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-alaRotation), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala2.RenderModel();

		// Hélice 1
		model = modelAux;
		model = glm::translate(model, glm::vec3(-0.4f, -0.3f, -0.35f));
		model = glm::rotate(model, glm::radians(heliceRotation), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice1.RenderModel();

		// Hélice 2
		model = modelAux;
		model = glm::translate(model, glm::vec3(-0.4f, -0.3f, 0.35f));
		model = glm::rotate(model, glm::radians(heliceRotation), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice2.RenderModel();

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}