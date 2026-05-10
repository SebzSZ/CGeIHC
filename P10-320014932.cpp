#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#include "AudioManager.h"
#include "Camera.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "InputManager.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Skybox.h"
#include "SpotLight.h"
#include "Texture.h"
#include "Window.h"
#include "GameObject.h"

// 
// TU SISTEMA ORIGINAL DE ANIMACIÓN POR KEYFRAMES DE LA NAVE
// 
float reproduciranimacion = 0, habilitaranimacion = 0, guardoFrame = 0, reinicioFrame = 0, ciclo = 0, ciclo2 = 0;
float ciclo3 = 0, ciclo4 = 0;
float ciclo5 = 0, ciclo6 = 0;
float ciclo7 = 0, ciclo8 = 0;
float cicloN = 0, cicloM = 0;

float posXavion = 0.0f, posYavion = 5.0f, posZavion = 5.0f;
float movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0.0f;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 6;
typedef struct _frame
{
    float movAvion_x;
    float movAvion_y;
    float movAvion_xInc;
    float movAvion_yInc;
    float giroAvion;
    float giroAvionInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 6;
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
    printf("frameindex %d\n", FrameIndex);
    KeyFrame[FrameIndex].movAvion_x = movAvion_x;
    KeyFrame[FrameIndex].movAvion_y = movAvion_y;
    KeyFrame[FrameIndex].giroAvion = giroAvion;
    FrameIndex++;
}

void resetElements(void)
{
    movAvion_x = KeyFrame[0].movAvion_x;
    movAvion_y = KeyFrame[0].movAvion_y;
    giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
    KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
    KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
    KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
}

void animate(void)
{
    if (play)
    {
        if (i_curr_steps >= i_max_steps)
        {
            playIndex++;
            printf("playindex : %d\n", playIndex);
            if (playIndex > FrameIndex - 2)
            {
                printf("Frame index= %d\n", FrameIndex);
                printf("termina anim\n");
                playIndex = 0;
                play = false;
            }
            else
            {
                i_curr_steps = 0;
                interpolation();
            }
        }
        else
        {
            movAvion_x += KeyFrame[playIndex].movAvion_xInc;
            movAvion_y += KeyFrame[playIndex].movAvion_yInc;
            giroAvion += KeyFrame[playIndex].giroAvionInc;
            i_curr_steps++;
        }
    }
}

// NUEVO: Funciones para Archivo TXT
void GuardarKeyframesEnArchivo(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << FrameIndex << "\n";
        for (int i = 0; i < FrameIndex; ++i) {
            file << KeyFrame[i].movAvion_x << " " << KeyFrame[i].movAvion_y << " " << KeyFrame[i].giroAvion << "\n";
        }
        file.close();
        printf("\n[Exito] Animacion GUARDADA en el disco duro (Archivo: %s)\n", filename.c_str());
    }
}

void CargarKeyframesDeArchivo(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> FrameIndex;
        for (int i = 0; i < FrameIndex && i < MAX_FRAMES; ++i) {
            file >> KeyFrame[i].movAvion_x >> KeyFrame[i].movAvion_y >> KeyFrame[i].giroAvion;
        }
        file.close();
        printf("\n[Exito] Animacion CARGADA del disco duro con %d frames.\n", FrameIndex);
    }
}

// 
// TU FUNCIÓN ORIGINAL DE INPUTS (Adaptada a tu InputManager actual)
// 
void inputKeyframes()
{
    InputManager& input = InputManager::getInstance();

    if (input.isKeyDown(GLFW_KEY_SPACE))
    {
        if (reproduciranimacion < 1)
        {
            if (play == false && (FrameIndex > 1))
            {
                resetElements();
                interpolation();
                play = true;
                playIndex = 0;
                i_curr_steps = 0;
                reproduciranimacion++;
                printf("presiona 0 para habilitar reproducir de nuevo la animación'\n");
                habilitaranimacion = 0;
            }
            else
            {
                play = false;
            }
        }
    }
    if (input.isKeyDown(GLFW_KEY_0))
    {
        if (habilitaranimacion < 1)
        {
            reproduciranimacion = 0;
            habilitaranimacion = 1;
            printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
        }
    }

    if (input.isKeyDown(GLFW_KEY_L))
    {
        if (guardoFrame < 1)
        {
            saveFrame();
            printf("presiona P para habilitar guardar otro frame'\n");
            guardoFrame = 1;
            reinicioFrame = 0;
        }
    }
    if (input.isKeyDown(GLFW_KEY_P))
    {
        if (reinicioFrame < 1)
        {
            guardoFrame = 0;
            reinicioFrame = 1;
            printf("Ya puedes guardar otro frame presionando la tecla L'\n");
        }
    }

    // Tecla NUEVA para guardar al TXT del disco duro
    if (input.isKeyDown(GLFW_KEY_G))
    {
        GuardarKeyframesEnArchivo("animacion_nave.txt");
    }

    if (input.isKeyDown(GLFW_KEY_1))
    {
        if (ciclo < 1)
        {
            movAvion_x += 1.0f;
            printf("movAvion_x es: %f\n", movAvion_x);
            ciclo = 1;
            ciclo2 = 0;
            printf("Presiona 2 para habilitar\n");
        }
    }
    if (input.isKeyDown(GLFW_KEY_2))
    {
        if (ciclo2 < 1)
        {
            ciclo = 0;
            ciclo2 = 1;
            printf("Presionando 1 está habilitada\n");
        }
    }

    if (input.isKeyDown(GLFW_KEY_3))
    {
        if (ciclo3 < 1)
        {
            movAvion_x -= 1.0f;
            printf("movAvion_x es: %f\n", movAvion_x);
            ciclo3 = 1;
            ciclo4 = 0;
            printf("Presiona 4 para habilitar\n");
        }
    }
    if (input.isKeyDown(GLFW_KEY_4))
    {
        if (ciclo4 < 1)
        {
            ciclo3 = 0;
            ciclo4 = 1;
            printf("Presionando 3 está habilitada\n");
        }
    }

    if (input.isKeyDown(GLFW_KEY_5))
    {
        if (ciclo5 < 1)
        {
            movAvion_y += 1.0f;
            printf("movAvion_y es: %f\n", movAvion_y);
            ciclo5 = 1;
            ciclo6 = 0;
            printf("Presiona 6 para habilitar\n");
        }
    }
    if (input.isKeyDown(GLFW_KEY_6))
    {
        if (ciclo6 < 1)
        {
            ciclo5 = 0;
            ciclo6 = 1;
            printf("Presionando 5 está habilitada\n");
        }
    }

    if (input.isKeyDown(GLFW_KEY_7))
    {
        if (ciclo7 < 1)
        {
            movAvion_y -= 1.0f;
            printf("movAvion_y es: %f\n", movAvion_y);
            ciclo7 = 1;
            ciclo8 = 0;
            printf("Presiona 8 para habilitar\n");
        }
    }
    if (input.isKeyDown(GLFW_KEY_8))
    {
        if (ciclo8 < 1)
        {
            ciclo7 = 0;
            ciclo8 = 1;
            printf("Presionando 7 está habilitada\n");
        }
    }

    if (input.isKeyDown(GLFW_KEY_N))
    {
        if (cicloN < 1)
        {
            giroAvion += 180.0f;
            if (giroAvion >= 360.0f)
                giroAvion -= 360.0f;
            printf("giroAvion es: %f\n", giroAvion);
            cicloN = 1;
            cicloM = 0;
            printf("Presiona M para habilitar\n");
        }
    }
    if (input.isKeyDown(GLFW_KEY_M))
    {
        if (cicloM < 1)
        {
            cicloN = 0;
            cicloM = 1;
            printf("Presionando N está habilitada\n");
        }
    }
}


// 
// Shaders, Piso y Creación del Castillo
// 
static const char* VERT_SHADER = "shaders/shader.vert";
static const char* FRAG_SHADER = "shaders/shader.frag";
static const char* SKYBOX_VERT = "shaders/skybox.vert";
static const char* SKYBOX_FRAG = "shaders/skybox.frag";

static GLfloat FLOOR_VERTS[] = {
       -1.0f,  0.0f, -1.0f,  0.0f, 10.0f,  0.0f, 1.0f, 0.0f,
        1.0f,  0.0f, -1.0f, 10.0f, 10.0f,  0.0f, 1.0f, 0.0f,
        1.0f,  0.0f,  1.0f, 10.0f,  0.0f,  0.0f, 1.0f, 0.0f,
       -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,
};
static GLuint FLOOR_IDX[] = { 0, 1, 2,  0, 2, 3 };

std::shared_ptr<GameObject> CreateCastle(Material& matOpaco, Model& floor, Model& mainRoom, Model& bigTower, Model& wall, Model& midTower)
{
    std::shared_ptr<GameObject> castleObj = std::make_shared<GameObject>("Castle Container", GameObjectType::MODEL);

    std::shared_ptr<GameObject> mainRoomObj = std::make_shared<GameObject>("Main Room", GameObjectType::MODEL);
    mainRoomObj->setModel(&mainRoom);
    mainRoomObj->setMaterial(&matOpaco);
    mainRoomObj->transform.setPosition(20.0f, 0.0f, 1.64f);
    mainRoomObj->transform.setRotation(0.0f, 180.0f, 0.0f);
    mainRoomObj->transform.setScale(120.0f);
    castleObj->addChild(mainRoomObj);

    std::shared_ptr<GameObject> wall1Obj = std::make_shared<GameObject>("Wall 1", GameObjectType::MODEL);
    wall1Obj->setModel(&wall);
    wall1Obj->setMaterial(&matOpaco);
    wall1Obj->transform.setPosition(6.9f, 0.0f, 0.0f);
    wall1Obj->transform.setRotation(0.0f, -90.0f, 0.0f);
    wall1Obj->transform.setScale(60.0f);
    castleObj->addChild(wall1Obj);

    std::shared_ptr<GameObject> wall2Obj = std::make_shared<GameObject>("Wall 2", GameObjectType::MODEL);
    wall2Obj->setModel(&wall);
    wall2Obj->setMaterial(&matOpaco);
    wall2Obj->transform.setPosition(33.21f, 0.0f, 0.0f);
    wall2Obj->transform.setRotation(0.0f, -90.0f, 0.0f);
    wall2Obj->transform.setScale(60.0f);
    castleObj->addChild(wall2Obj);

    std::shared_ptr<GameObject> wall3Obj = std::make_shared<GameObject>("Wall 3", GameObjectType::MODEL);
    wall3Obj->setModel(&wall);
    wall3Obj->setMaterial(&matOpaco);
    wall3Obj->transform.setPosition(40.9f, 0.0f, -7.55f);
    wall3Obj->transform.setRotation(0.0f, 0.0f, 0.0f);
    wall3Obj->transform.setScale(60.0f);
    castleObj->addChild(wall3Obj);

    std::shared_ptr<GameObject> wall4Obj = std::make_shared<GameObject>("Wall 4", GameObjectType::MODEL);
    wall4Obj->setModel(&wall);
    wall4Obj->setMaterial(&matOpaco);
    wall4Obj->transform.setPosition(20.27f, 0.0f, -15.21f);
    wall4Obj->transform.setRotation(0.0f, 90.0f, 0.0f);
    wall4Obj->transform.setScale(glm::vec3(60.0f, 60.0f, 165.0f));
    castleObj->addChild(wall4Obj);

    std::shared_ptr<GameObject> wall5Obj = std::make_shared<GameObject>("Wall 5", GameObjectType::MODEL);
    wall5Obj->setModel(&wall);
    wall5Obj->setMaterial(&matOpaco);
    wall5Obj->transform.setPosition(-0.73f, 0.0f, -7.66f);
    wall5Obj->transform.setRotation(0.0f, 180.0f, 0.0f);
    wall5Obj->transform.setScale(60.0f);
    castleObj->addChild(wall5Obj);

    std::shared_ptr<GameObject> wall6Obj = std::make_shared<GameObject>("Wall 6", GameObjectType::MODEL);
    wall6Obj->setModel(&wall);
    wall6Obj->setMaterial(&matOpaco);
    wall6Obj->transform.setPosition(20.0f, 3.96f, -6.58f);
    wall6Obj->transform.setRotation(0.0f, -90.0f, 0.0f);
    wall6Obj->transform.setScale(60.0f);
    castleObj->addChild(wall6Obj);

    std::shared_ptr<GameObject> wall7Obj = std::make_shared<GameObject>("Wall 7", GameObjectType::MODEL);
    wall7Obj->setModel(&wall);
    wall7Obj->setMaterial(&matOpaco);
    wall7Obj->transform.setPosition(20.1f, 3.96f, -12.91f);
    wall7Obj->transform.setRotation(0.0f, 90.0f, 0.0f);
    wall7Obj->transform.setScale(60.0f);
    castleObj->addChild(wall7Obj);

    std::shared_ptr<GameObject> wall8Obj = std::make_shared<GameObject>("Wall 8", GameObjectType::MODEL);
    wall8Obj->setModel(&wall);
    wall8Obj->setMaterial(&matOpaco);
    wall8Obj->transform.setPosition(27.7f, 3.96f, -9.8f);
    wall8Obj->transform.setRotation(0.0f, 0.0f, 0.0f);
    wall8Obj->transform.setScale(glm::vec3(60.0f, 60.0f, 24.89f));
    castleObj->addChild(wall8Obj);

    std::shared_ptr<GameObject> wall9Obj = std::make_shared<GameObject>("Wall 9", GameObjectType::MODEL);
    wall9Obj->setModel(&wall);
    wall9Obj->setMaterial(&matOpaco);
    wall9Obj->transform.setPosition(12.4f, 3.96f, -9.8f);
    wall9Obj->transform.setRotation(0.0f, 180.0f, 0.0f);
    wall9Obj->transform.setScale(glm::vec3(60.0f, 60.0f, 24.89f));
    castleObj->addChild(wall9Obj);

    std::shared_ptr<GameObject> bigTower1Obj = std::make_shared<GameObject>("Big Tower 1", GameObjectType::MODEL);
    bigTower1Obj->setModel(&bigTower);
    bigTower1Obj->setMaterial(&matOpaco);
    bigTower1Obj->transform.setPosition(16.84f, 8.92f, -9.0f);
    bigTower1Obj->transform.setScale(60.0f);
    castleObj->addChild(bigTower1Obj);

    std::shared_ptr<GameObject> bigTower2Obj = std::make_shared<GameObject>("Big Tower 2", GameObjectType::MODEL);
    bigTower2Obj->setModel(&bigTower);
    bigTower2Obj->setMaterial(&matOpaco);
    bigTower2Obj->transform.setPosition(24.65f, 0.53f, -9.47f);
    bigTower2Obj->transform.setScale(60.0f);
    castleObj->addChild(bigTower2Obj);

    std::shared_ptr<GameObject> midTower1Obj = std::make_shared<GameObject>("Mid Tower 1", GameObjectType::MODEL);
    midTower1Obj->setModel(&midTower);
    midTower1Obj->setMaterial(&matOpaco);
    midTower1Obj->transform.setPosition(36.09f, 5.44f, -3.64f);
    midTower1Obj->transform.setRotation(0.0f, -90.0f, 0.0f);
    midTower1Obj->transform.setScale(50.0f);
    castleObj->addChild(midTower1Obj);

    std::shared_ptr<GameObject> midTower2Obj = std::make_shared<GameObject>("Mid Tower 2", GameObjectType::MODEL);
    midTower2Obj->setModel(&midTower);
    midTower2Obj->setMaterial(&matOpaco);
    midTower2Obj->transform.setPosition(36.09f, 5.44f, -11.0f);
    midTower2Obj->transform.setRotation(0.0f, -90.0f, 0.0f);
    midTower2Obj->transform.setScale(50.0f);
    castleObj->addChild(midTower2Obj);

    std::shared_ptr<GameObject> midTower3Obj = std::make_shared<GameObject>("Mid Tower 3", GameObjectType::MODEL);
    midTower3Obj->setModel(&midTower);
    midTower3Obj->setMaterial(&matOpaco);
    midTower3Obj->transform.setPosition(3.69f, 5.44f, -3.64f);
    midTower3Obj->transform.setRotation(0.0f, -90.0f, 0.0f);
    midTower3Obj->transform.setScale(50.0f);
    castleObj->addChild(midTower3Obj);

    std::shared_ptr<GameObject> midTower4Obj = std::make_shared<GameObject>("Mid Tower 4", GameObjectType::MODEL);
    midTower4Obj->setModel(&midTower);
    midTower4Obj->setMaterial(&matOpaco);
    midTower4Obj->transform.setPosition(3.69f, 5.44f, -11.0f);
    midTower4Obj->transform.setRotation(0.0f, -90.0f, 0.0f);
    midTower4Obj->transform.setScale(50.0f);
    castleObj->addChild(midTower4Obj);

    std::shared_ptr<GameObject> floor1Obj = std::make_shared<GameObject>("Floor 1", GameObjectType::MODEL);
    floor1Obj->setModel(&floor);
    floor1Obj->setMaterial(&matOpaco);
    floor1Obj->transform.setPosition(33.59f, 5.28f, -8.34f);
    floor1Obj->transform.setScale(58.3f, 1.0f, 76.2f);
    castleObj->addChild(floor1Obj);

    std::shared_ptr<GameObject> floor2Obj = std::make_shared<GameObject>("Floor 2", GameObjectType::MODEL);
    floor2Obj->setModel(&floor);
    floor2Obj->setMaterial(&matOpaco);
    floor2Obj->transform.setPosition(6.43f, 5.28f, -8.34f);
    floor2Obj->transform.setScale(58.3f, 1.0f, 76.2f);
    castleObj->addChild(floor2Obj);

    std::shared_ptr<GameObject> floor3Obj = std::make_shared<GameObject>("Floor 3", GameObjectType::MODEL);
    floor3Obj->setModel(&floor);
    floor3Obj->setMaterial(&matOpaco);
    floor3Obj->transform.setPosition(20.0f, 5.28f, -10.2f);
    floor3Obj->transform.setScale(50.1f, 1.0f, 55.15f);
    castleObj->addChild(floor3Obj);

    std::shared_ptr<GameObject> floor4Obj = std::make_shared<GameObject>("Floor 4", GameObjectType::MODEL);
    floor4Obj->setModel(&floor);
    floor4Obj->setMaterial(&matOpaco);
    floor4Obj->transform.setPosition(19.75f, 9.28f, -10.0f);
    floor4Obj->transform.setScale(60.0f, 1.0f, 31.1f);
    castleObj->addChild(floor4Obj);

    return castleObj;
}

// 
// MAIN PRINCIPAL
// 
int main()
{
    Window mainWindow("Proyecto Final - CGeIHC");
    if (mainWindow.Initialize() != 0) return -1;

    AudioManager& audioManager = AudioManager::getInstance();
    if (!audioManager.Initialize()) return -1;

    audioManager.loadMP3("bgMusic", "Sounds/bgMusic.mp3");
    audioManager.play("bgMusic", true, 0.5f);

    Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.1f);
    camera.setCameraMode(CameraMode::THIRD_PERSON);

    Shader shader;
    shader.createFromFiles(VERT_SHADER, FRAG_SHADER);

    Skybox skybox;
    skybox.create(
        { "Textures/Skybox/jettelly_space_nebulas_blue_LEFT.png", "Textures/Skybox/jettelly_space_nebulas_blue_RIGHT.png", "Textures/Skybox/jettelly_space_nebulas_blue_UP.png", "Textures/Skybox/jettelly_space_nebulas_blue_DOWN.png", "Textures/Skybox/jettelly_space_nebulas_blue_FRONT.png", "Textures/Skybox/jettelly_space_nebulas_blue_BACK.png" },
        { "Textures/Skybox/jettelly_space_nebulas_black_LEFT.png", "Textures/Skybox/jettelly_space_nebulas_black_RIGHT.png", "Textures/Skybox/jettelly_space_nebulas_black_UP.png", "Textures/Skybox/jettelly_space_nebulas_black_DOWN.png", "Textures/Skybox/jettelly_space_nebulas_black_FRONT.png", "Textures/Skybox/jettelly_space_nebulas_black_BACK.png" },
        SKYBOX_VERT, SKYBOX_FRAG
    );
    skybox.setDayNightCycleDuration(30.0f);

    Texture floorTexture("Textures/naka_yuka_01_52D00-DXT1.png");
    floorTexture.loadWithAlpha();

    Material matOpaco(0.2f, 4.0f);
    Material matBrillante(1.0f, 32.0f);

    Model rubyModel;    if (!rubyModel.load("Models/RubyCuerpo.obj"))      return -1;
    Model rubyLeftArm;  if (!rubyLeftArm.load("Models/RubyBrazoIzq.obj"))  return -1;
    Model rubyRightArm; if (!rubyRightArm.load("Models/RubyBrazoDer.obj")) return -1;
    Model rubyLeftLeg;  if (!rubyLeftLeg.load("Models/RubyPiernaIzq.obj")) return -1;
    Model rubyRightLeg; if (!rubyRightLeg.load("Models/RubyPiernaDer.obj")) return -1;
    Model rubyCape;     if (!rubyCape.load("Models/RubyCapa.obj"))         return -1;

    Model mainRoom;  if (!mainRoom.load("Models/HyruleCastle_MainRoom.obj")) return -1;
    Model bigTower;  if (!bigTower.load("Models/HyruleCastle_BigTower.obj")) return -1;
    Model wall;      if (!wall.load("Models/HyruleCastle_Wall.obj"))         return -1;
    Model midTower;  if (!midTower.load("Models/HyruleCastle_MidTower.obj")) return -1;
    Model floor;     if (!floor.load("Models/HyruleCastle_Floor.obj"))       return -1;

    Model nave_M;    if (!nave_M.load("Models/nave.obj")) return -1;
    Model ala_M;     if (!ala_M.load("Models/ala.obj")) return -1;
    Model ala2_M;    if (!ala2_M.load("Models/ala2.obj")) return -1;
    Model helice1_M; if (!helice1_M.load("Models/helice1.obj")) return -1;
    Model helice2_M; if (!helice2_M.load("Models/helice2.obj")) return -1;

    std::shared_ptr<GameObject> floorObj = std::make_shared<GameObject>("Floor", GameObjectType::MESH);
    MeshData floorData;
    floorData.vertices = std::vector<GLfloat>(std::begin(FLOOR_VERTS), std::end(FLOOR_VERTS));
    floorData.indices = std::vector<GLuint>(std::begin(FLOOR_IDX), std::end(FLOOR_IDX));
    floorObj->loadMesh(floorData);
    floorObj->setTextureID(floorTexture.getID());
    floorObj->setMaterial(&matOpaco);
    floorObj->transform.setScale(50.0f, 1.0f, 50.0f);

    std::shared_ptr<GameObject> castleObj = CreateCastle(matOpaco, floor, mainRoom, bigTower, wall, midTower);

    std::shared_ptr<GameObject> rubyObj = std::make_shared<GameObject>("Ruby", GameObjectType::MODEL);
    rubyObj->setModel(&rubyModel);
    rubyObj->setMaterial(&matOpaco);
    rubyObj->transform.setPosition(0.0f, 1.0f, 0.0f);
    rubyObj->transform.setScale(5.0f);

    std::shared_ptr<GameObject> leftArm = std::make_shared<GameObject>("LeftArm", GameObjectType::MODEL);
    leftArm->setModel(&rubyLeftArm);
    leftArm->transform.setPosition(0.025f, 0.0611f, 0.0f);
    leftArm->transform.setRotation(0.0f, 0.0f, -65.0f);
    rubyObj->addChild(leftArm);

    std::shared_ptr<GameObject> rightArm = std::make_shared<GameObject>("RightArm", GameObjectType::MODEL);
    rightArm->setModel(&rubyRightArm);
    rightArm->transform.setPosition(-0.025f, 0.0611f, 0.0f);
    rightArm->transform.setRotation(0.0f, 0.0f, 65.0f);
    rubyObj->addChild(rightArm);

    std::shared_ptr<GameObject> leftLeg = std::make_shared<GameObject>("LeftLeg", GameObjectType::MODEL);
    leftLeg->setModel(&rubyLeftLeg);
    rubyObj->addChild(leftLeg);

    std::shared_ptr<GameObject> rightLeg = std::make_shared<GameObject>("RightLeg", GameObjectType::MODEL);
    rightLeg->setModel(&rubyRightLeg);
    rubyObj->addChild(rightLeg);

    std::shared_ptr<GameObject> cape = std::make_shared<GameObject>("Cape", GameObjectType::MODEL);
    cape->setModel(&rubyCape);
    cape->transform.setPosition(0.0f, 0.065f, -0.025f);
    rubyObj->addChild(cape);

    float walkTime = 0.0f;
    float walkSpeed = 10.0f;
    float walkAmplitude = 35.0f;
    float angleThigh = 0.0f;

        std::shared_ptr<GameObject> naveObj = std::make_shared<GameObject>("Nave", GameObjectType::MODEL);
        naveObj->setModel(&nave_M);
        naveObj->setMaterial(&matBrillante);

        std::shared_ptr<GameObject> ala1Obj = std::make_shared<GameObject>("Ala1", GameObjectType::MODEL);
        ala1Obj->setModel(&ala_M);
        ala1Obj->setMaterial(&matBrillante);
        ala1Obj->transform.setPosition(0.0f, 0.0f, -0.25f);
        naveObj->addChild(ala1Obj);

        std::shared_ptr<GameObject> ala2Obj = std::make_shared<GameObject>("Ala2", GameObjectType::MODEL);
        ala2Obj->setModel(&ala2_M);
        ala2Obj->setMaterial(&matBrillante);
        ala2Obj->transform.setPosition(0.0f, 0.0f, 0.0f);
        naveObj->addChild(ala2Obj);

        std::shared_ptr<GameObject> helice1Obj = std::make_shared<GameObject>("Helice1", GameObjectType::MODEL);
        helice1Obj->setModel(&helice1_M);
        helice1Obj->setMaterial(&matBrillante);
        helice1Obj->transform.setPosition(-0.4f, -0.3f, -0.35f);
        naveObj->addChild(helice1Obj);

        std::shared_ptr<GameObject> helice2Obj = std::make_shared<GameObject>("Helice2", GameObjectType::MODEL);
        helice2Obj->setModel(&helice2_M);
        helice2Obj->setMaterial(&matBrillante);
        helice2Obj->transform.setPosition(-0.4f, -0.3f, 0.35f);
        naveObj->addChild(helice2Obj);

    float alaRotation = 0.0f;
    float alaDirection = 1.0f;
    float alaRotationSpeed = 20.0f;
    float heliceRotation = 0.0f;
    float heliceRotationSpeed = 300.0f;

    //  Carga inicial de Keyframes 
    // Cargamos tu ruta inicial por default (los 6 frames originales)
    KeyFrame[0].movAvion_x = 0.0f; KeyFrame[0].movAvion_y = 0.0f; KeyFrame[0].giroAvion = 0;
    KeyFrame[1].movAvion_x = 1.0f; KeyFrame[1].movAvion_y = 2.0f; KeyFrame[1].giroAvion = 0;
    KeyFrame[2].movAvion_x = 2.0f; KeyFrame[2].movAvion_y = 0.0f; KeyFrame[2].giroAvion = 0;
    KeyFrame[3].movAvion_x = 3.0f; KeyFrame[3].movAvion_y = -2.0f; KeyFrame[3].giroAvion = 0;
    KeyFrame[4].movAvion_x = 3.0f; KeyFrame[4].movAvion_y = -2.0f; KeyFrame[4].giroAvion = 180.0f;
    KeyFrame[5].movAvion_x = 0.0f; KeyFrame[5].movAvion_y = 0.0f; KeyFrame[5].giroAvion = 180.0f;
    FrameIndex = 6;

    // Intentar sobrescribir con el TXT si existe
    CargarKeyframesDeArchivo("animacion_nave.txt");

    DirectionalLight directionalLight(
        1.0f, 0.95f, 0.8f,
        0.3f, 0.8f,
        0.0f, -1.0f, -0.5f
    );

    SpotLight spotLights[MAX_SPOT_LIGHTS];
    unsigned int spotLightCount = 0;
    spotLights[0] = SpotLight(
        1.0f, 1.0f, 1.0f,
        0.4f, 1.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 0.005f, 0.0f,
        15.0f
    );
    spotLightCount++;

    glm::vec3 rubyPos = rubyObj->transform.getPosition();
    camera.addInterestPoint(rubyPos + glm::vec3(0.0f, 2.0f, 5.0f), rubyPos + glm::vec3(0.0f, 1.0f, 0.0f));
    camera.addInterestPoint(glm::vec3(0.0f, 25.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    camera.addInterestPoint(rubyPos + glm::vec3(8.0f, 3.0f, 0.0f), rubyPos + glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)mainWindow.getBufferWidth() / (float)mainWindow.getBufferHeight(), 0.1f, 500.0f);
    glm::mat4 model(1.0f);

    GLfloat lastTime = (GLfloat)glfwGetTime();

    printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion\n");
    printf("3.-Presiona L para guardar frame temporal\n4.-Presiona P para habilitar guardar nuevo frame\n");
    printf("5.-Presiona G para guardar todos los frames en un archivo TXT\n");
    printf("\nControles de movimiento del avion:\n1 (y 2) - Mover en X positivo\n3 (y 4) - Mover en X negativo\n");
    printf("5 (y 6) - Mover en Y positivo\n7 (y 8) - Mover en Y negativo\nN (y M) - Rotar 180 grados\n\n");

    // 
    // Loop principal de render
    // 
    while (!mainWindow.shouldClose())
    {
        GLfloat now = (GLfloat)glfwGetTime();
        GLfloat deltaTime = now - lastTime;
        lastTime = now;

        skybox.updateDayNightCycle(deltaTime);

        glfwPollEvents();
        InputManager& input = InputManager::getInstance();
        input.beginFrame();

        //  Llamamos a tu función idéntica de inputs 
        inputKeyframes();
        animate(); // Animamos a la nave usando tus funciones originales

        //  Input Cámara (TUS TECLAS ORIGINALES) 
        if (input.isKeyPressed(GLFW_KEY_1)) camera.setCameraMode(CameraMode::THIRD_PERSON);
        if (input.isKeyPressed(GLFW_KEY_2)) camera.setCameraMode(CameraMode::AERIAL);
        if (input.isKeyPressed(GLFW_KEY_3)) camera.setCameraMode(CameraMode::INTEREST_POINT);

        switch (camera.getCameraMode()) {
        case CameraMode::THIRD_PERSON:
            camera.mouseControl(input.getMouseDeltaX() * 0.5f, input.getMouseDeltaY() * 0.5f);
            camera.updateThirdPersonCamera(rubyObj->transform.getPosition(), deltaTime);
            break;
        default: break;
        }

        //  Input Personaje Ruby (TUS TECLAS ORIGINALES) 
        float rubyMoveSpeed = 8.0f;
        glm::vec3 cameraDirection = camera.getDirection();
        glm::vec3 moveDirection = glm::normalize(glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
        glm::vec3 rightDirection = glm::normalize(glm::cross(moveDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        bool isWalking = false;

        if (input.isKeyDown(GLFW_KEY_W)) { glm::vec3 movement = moveDirection * rubyMoveSpeed * deltaTime; rubyObj->transform.translate(movement.x, 0.0f, movement.z); isWalking = true; }
        if (input.isKeyDown(GLFW_KEY_S)) { glm::vec3 movement = moveDirection * rubyMoveSpeed * deltaTime; rubyObj->transform.translate(-movement.x, 0.0f, -movement.z); isWalking = true; }
        if (input.isKeyDown(GLFW_KEY_A)) { glm::vec3 movement = rightDirection * rubyMoveSpeed * deltaTime; rubyObj->transform.translate(-movement.x, 0.0f, -movement.z); isWalking = true; }
        if (input.isKeyDown(GLFW_KEY_D)) { glm::vec3 movement = rightDirection * rubyMoveSpeed * deltaTime; rubyObj->transform.translate(movement.x, 0.0f, movement.z); isWalking = true; }

        if (isWalking) {
            walkTime += deltaTime * walkSpeed;
            angleThigh = walkAmplitude * sin(walkTime);
        }
        else {
            walkTime = 0.0f;
            angleThigh += (0.0f - angleThigh) * 8.0f * deltaTime;
        }

        leftArm->transform.setRotation(angleThigh, 0.0f, -65.0f);
        rightArm->transform.setRotation(-angleThigh, 0.0f, 65.0f);
        leftLeg->transform.setRotation(-angleThigh, 0.0f, 0.0f);
        rightLeg->transform.setRotation(angleThigh, 0.0f, 0.0f);
        cape->transform.setRotation((-angleThigh * 0.5f) + 45.0f, 0.0f, 0.0f);

        glm::vec3 rubyDirection = glm::normalize(glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
        float angleY = atan2(rubyDirection.x, rubyDirection.z);
        rubyObj->transform.setRotation(0.0f, glm::degrees(angleY), 0.0f);

        //  Actualizar Transformación de la Nave 
        naveObj->transform.setPosition(posXavion + movAvion_x, posYavion + movAvion_y, posZavion);
        naveObj->transform.setRotation(0.0f, giroAvion + 180.0f, 0.0f);

        heliceRotation += heliceRotationSpeed * deltaTime;
        if (heliceRotation >= 360.0f) heliceRotation -= 360.0f;

        alaRotation += alaRotationSpeed * deltaTime * alaDirection;
        if (alaRotation >= 20.0f) { alaRotation = 20.0f; alaDirection = -1.0f; }
        if (alaRotation <= -20.0f) { alaRotation = -20.0f; alaDirection = 1.0f; }

        ala1Obj->transform.setRotation(0.0f, alaRotation, 0.0f);
        ala2Obj->transform.setRotation(0.0f, -alaRotation, 0.0f);
        helice1Obj->transform.setRotation(heliceRotation, 0.0f, 0.0f);
        helice2Obj->transform.setRotation(heliceRotation, 0.0f, 0.0f);

        //  Luces y Renderización 
        spotLights[0].setFlash(camera.getPosition(), camera.getDirection());

        float timeProgress = skybox.getTimeProgress();
        float dayIntensity, nightIntensity;

        if (timeProgress < 0.5f) {
            float t = timeProgress * 2.0f;
            dayIntensity = 1.0f - t;
            nightIntensity = t;
        }
        else {
            float t = (timeProgress - 0.5f) * 2.0f;
            dayIntensity = t;
            nightIntensity = 1.0f - t;
        }

        glm::vec3 dayColor(1.0f, 0.95f, 0.8f);
        glm::vec3 nightColor(0.4f, 0.4f, 0.6f);
        glm::vec3 currentColor = glm::mix(nightColor, dayColor, dayIntensity);

        directionalLight.setColor(currentColor);
        directionalLight.setAmbientIntensity(0.3f * (0.5f + dayIntensity * 0.5f));
        directionalLight.setDiffuseIntensity(0.8f * dayIntensity);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox.draw(camera.calculateViewMatrix(), projection);

        shader.use();
        glm::mat4 view = camera.calculateViewMatrix();
        glUniformMatrix4fv(shader.getProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(shader.getViewLocation(), 1, GL_FALSE, glm::value_ptr(view));
        glm::vec3 eye = camera.getPosition();
        glUniform3f(shader.getEyePositionLocation(), eye.x, eye.y, eye.z);
        glUniform3f(shader.getColorLocation(), 1.0f, 1.0f, 1.0f);
        glUniform2f(shader.getTextureOffsetLocation(), 0.0f, 0.0f);

        shader.setDirectionalLight(&directionalLight);
        shader.setSpotLights(spotLights, spotLightCount);

        floorObj->draw(shader);
        castleObj->draw(shader);
        naveObj->draw(shader);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rubyObj->draw(shader);
        glDisable(GL_BLEND);

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    audioManager.shutdown();
    return 0;
}