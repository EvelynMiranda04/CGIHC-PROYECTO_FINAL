// ====================================================================================
// PROYECTO FINAL - MOTOR GRÁFICO BASE
// ====================================================================================

// ====================================================================================
// 1. LIBRERÍAS Y DEPENDENCIAS
// Incluimos STB para cargar imágenes, GLM para matemáticas de matrices y vectores,
// y las librerías estándar de OpenGL (GLEW/GLFW).
// ====================================================================================
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

// ====================================================================================
// 2. CLASES DEL MOTOR
// Cabeceras propias que encapsulan la complejidad de OpenGL.
// ====================================================================================
#include "Window.h"
#include "Camera.h"
#include "Shader_light.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"   
#include "Skybox.h" 
#include "Sphere.h"
#include "Model.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

const float toRadians = 3.14159265f / 180.0f;

// ====================================================================================
// 3. VARIABLES DE ANIMACIÓN Y CONTROL GLOBAL
// Variables que se actualizarán cada frame en el Game Loop (Offsets, rotaciones, etc).
// Compañeros: Agreguen aquí sus variables de físicas o contadores.
// ====================================================================================
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0f;
float angulovaria = 0.0f;

// Control de Tiempo (DeltaTime) para animaciones fluidas independientes de los FPS
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// ====================================================================================
// 4. DECLARACIÓN DE OBJETOS DE LA ESCENA
// Vectores, cámara, texturas, modelos y materiales que existirán en el entorno.
// ====================================================================================
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Skybox skybox;

// Materiales base
Material Material_brillante;
Material Material_opaco;

// Texturas y Modelos
Texture pisoTexture;
Texture OctaedroTexture;
Model Lampara_M;
Model Piso;
Model Prueba;
// MODELOS SIGUIENDO NUMEROS:
Model M01;	// VIAS
Model M02;	// ESTACION
Model M03;	// TREN
Model M04;	// ANDÉN
Model M05;	// EDIFICIO
Model M06;	// EDIFICIO
Model M07;	// --------
Model M08;	// EDIFICIO
Model M09;	// --------
Model M10;	// --------
Model M11;	// EDIFICIO
Model M12; // ESTRUCTURA
Model M13;	// --------
Model M14; // ESTRUCTURA
Model M15; // DECORACIÓN
Model M16; // NAVE
Model M17; // COCHE
Model M18; // DECORACIÓN
Model M19; // DECORACIÓN
Model M20; // DECORACIÓN
Model M21; // EDIFICIO
Model M22;	// ESTRUCTURA
Model M23;	// EDIFICIO
Model M24;	// RELOJ
Model M25; // LIBRERÍA
Model M26;	// VEGETACION
Model M27;
Model M28_1;
Model M28_2;
Model M28_3;
Model M28_4;
Model M29;
Model M30;
Model M31;
Model M32;

// ====================================================================================
// 5. SISTEMA DE ILUMINACIÓN
// Arreglos de luces que soporta el Shader.
// ====================================================================================
DirectionalLight mainLight; // El "Sol" del escenario
PointLight pointLights[MAX_POINT_LIGHTS]; // Focos omnidireccionales
SpotLight spotLights[MAX_SPOT_LIGHTS];    // Linternas/Faros direccionales

// Rutas de Shaders
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";


// ====================================================================================
// FUNCIONES DE UTILIDAD Y GEOMETRÍA
// ====================================================================================

// Calcula normales dinámicas para mallas creadas a mano (Sombreado Phong suave)
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
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

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


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


// ====================================================================================
// FUNCIÓN PRINCIPAL (MAIN)
// ====================================================================================
int main()
{
	// ====================================================================================
	// --- 1. Inicialización de Ventana y Cámara ---
	// ====================================================================================
	mainWindow = Window(1800, 900); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CreateShaders();
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	// ====================================================================================
	// --- 2. Carga de Assets (Texturas y Modelos 3D) ---
	// ====================================================================================
	pisoTexture = Texture("Textures/piso.tga");				pisoTexture.LoadTextureA();
	//Lampara_M = Model();			Lampara_M.LoadModel("Models/LamparaOBJ.obj");
	Piso = Model();				Piso.LoadModel("Models/PISO.obj");
	
	Prueba = Model();			Prueba.LoadModel("Models/26.obj");
	//Prueba = Model();			Prueba.LoadModel("Models/09.glb");
	//Prueba = Model();			Prueba.LoadModel("Models/a.fbx");
	// Modelos segun numeros
	M01 = Model();				M01.LoadModel("Models/01.obj");
	M02 = Model();				M02.LoadModel("Models/02.obj");
	M03 = Model();				M03.LoadModel("Models/03.obj");
	M04 = Model();				M04.LoadModel("Models/04.obj");
	M05 = Model();				M05.LoadModel("Models/05.obj");
	M06 = Model();				M06.LoadModel("Models/06.obj");
	//M07 = Model();				M07.LoadModel("Models/07.obj");
	M08 = Model();				M08.LoadModel("Models/08.obj");
	//M09 = Model();				M09.LoadModel("Models/09.obj");
	//M10 = Model();				M10.LoadModel("Models/10.obj");
	M11 = Model();				M11.LoadModel("Models/11.obj");
	M12 = Model();				M12.LoadModel("Models/12.obj");
	//M13 = Model();				M13.LoadModel("Models/13.obj");
	M14 = Model();				M14.LoadModel("Models/14.obj");
	M15 = Model();				M15.LoadModel("Models/15.obj");
	M16 = Model();				M16.LoadModel("Models/16.obj");
	M17 = Model();				M17.LoadModel("Models/17.obj");
	M18 = Model();				M18.LoadModel("Models/18.obj");
	M19 = Model();				M19.LoadModel("Models/19.obj");
	M20 = Model();				M20.LoadModel("Models/20.obj");
	M21 = Model();				M21.LoadModel("Models/21.obj");
	M22 = Model();				M22.LoadModel("Models/22.obj");
	M23 = Model();				M23.LoadModel("Models/23.obj");
	M24 = Model();				M24.LoadModel("Models/24.obj");
	M25 = Model();				M25.LoadModel("Models/25.obj");
	M26 = Model();				M26.LoadModel("Models/26.obj");
	M27 = Model();				M27.LoadModel("Models/27.obj");
	M28_1 = Model();			M28_1.LoadModel("Models/28-1.obj");
	M28_2 = Model();			M28_2.LoadModel("Models/28-2.obj");
	M28_3 = Model();			M28_3.LoadModel("Models/28-3.obj");
	M28_4 = Model();			M28_4.LoadModel("Models/28-4.obj");
	M29 = Model();				M29.LoadModel("Models/29.obj");
	M30 = Model();				M30.LoadModel("Models/30.obj");
	M31 = Model();				M31.LoadModel("Models/31.obj");
	M32 = Model();				M32.LoadModel("Models/32.obj");


	// ====================================================================================
	// --- 3. Carga de Skybox y Configuración de Materiales ---
	// ====================================================================================
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	// ====================================================================================
	// --- 4. Configuración Inicial de Iluminación ---
	// ====================================================================================
	// Luz Direccional (Global) -----------------------------------------------------------
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.4f, 0.4f,						// Bajamos intensidad
		0.0f, -1.0f, 0.0f);
	// 2. LUCES PUNTUALES (Point Lights) --------------------------------------------------
	unsigned int pointLightCount = 0;
	// Luz de la Lámpara Hextech (Luz puntual BLANCA)
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,	// Color Blanco (RGB)
		1.0f, 1.0f,						// Intensidad (alta para que ilumine bien)
		0.0f, 5.75f, -7.0f,				// Posición: Misma X, Y, Z
		1.0f, 0.09f, 0.032f);			// Atenuación para difuminado realista
	pointLightCount++;
	// LUZ DEL PEZ ABISAL (Índice 1)
	pointLights[1] = PointLight(0.0f, 0.0f, 1.0f,	// Color Azul Puro (RGB)
		1.0f, 1.0f,						// Intensidad
		0.0f, 0.0f, 0.0f,				// Posición (Se sobreescribe en el while)
		1.0f, 0.7f, 1.8f);				// Atenuación	
	pointLightCount++;
	// 3. LUCES SPOT (Linternas y Faros) --------------------------------------------------
	unsigned int spotLightCount = 0;
	// Índice 0: Faro AZUL (Coche) - Este será sobreescrito en el while
	spotLights[0] = SpotLight(0.035f, 0.318f, 0.600f,	// Color Azul (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, -0.2f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		90.0f);
	spotLightCount++;
	// Índice 1: Faro AMARILLO 1 (Nave) - Este será sobreescrito en el while 
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,		// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		30.0f);
	spotLightCount++;
	// Índice 2: Spotlight del Pez Abisal (Amarillo)
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f,	// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		40.0f);
	spotLightCount++;

	// ARREGLO AUXILIAR DE FAROS (Aqui ponemos distintas luces auxiliares para faros)
	SpotLight farosAuxiliares[8];
	// 0: Faro ROJO (Cono cerrado, llega lejos)
	farosAuxiliares[0] = SpotLight(1.0f, 0.0f, 0.0f,	// Color Rojo (RGB)
		1.0f, 1.0f,										// Intensidades
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,			// Posición y Dirección 
		1.0f, 0.045f, 0.0075f,							// Atenuación 
		30.0f);											// Ángulo de apertura 
	// 1: Faro AMARILLO (Cono medio)
	farosAuxiliares[1] = SpotLight(1.0f, 1.0f, 0.0f,	// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		45.0f);
	// 2: Faro VERDE (Cono amplio, se atenúa más rápido)
	farosAuxiliares[2] = SpotLight(0.0f, 1.0f, 0.0f,	// Color Verde (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.09f, 0.032f,
		60.0f);
	// 3: Faro CIAN 
	farosAuxiliares[3] = SpotLight(0.0f, 1.0f, 1.0f,	// Color Cian (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		75.0f);
	// 4: Faro AZUL (Original)
	farosAuxiliares[4] = SpotLight(0.0f, 0.0f, 1.0f,	// Color Azul (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		90.0f);
	// 5: Faro MAGENTA (Cono gigante)
	farosAuxiliares[5] = SpotLight(1.0f, 0.0f, 1.0f,	// Color Magenta (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.022f, 0.0019f,
		120.0f);
	// 6: HELICOPTERO FARO AMARILLO PERO HACIA ADELANTE
	farosAuxiliares[6] = SpotLight(1.0f, 1.0f, 0.0f,	// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,
		1.0f, 0.09f, 0.032f,
		30.0f);
	// 7: HELICOPTERO FARO AMARILLO PERO HACIA ATRAS
	farosAuxiliares[7] = SpotLight(1.0f, 1.0f, 0.0f,	// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		30.0f);

	// ====================================================================================
	// --- 5. Ubicaciones del Shader (Uniforms) ---
	// Estas variables se conectan directamente con la tarjeta gráfica para enviarle datos de matrices, posiciones y colores.
	// ====================================================================================
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);



	// ====================================================================================
	// --- 6. Declaraciones movidas fuera del ciclo ---
	// ====================================================================================

	// Inicialización de matrices y vectores de apoyo -------------------------------------
	GLfloat   now = 0.0f;
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 modelaux = glm::mat4(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);



	// ====================================================================================
	// GAME LOOP (Ciclo Principal de Renderizado)
	// ====================================================================================
	while (!mainWindow.getShouldClose())
	{

		// Control de cuadros por segundo (Cálculo del DeltaTime)
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		angulovaria += 0.5f * deltaTime;

		// ====================================================================================
		// FASE 0: FÍSICAS Y ANIMACIONES MATEMÁTICAS
		// Aquí deben ir todas las lógicas que modifiquen variables antes de enviarlas al modelo.
		// ====================================================================================
		/*
		// 1. Lógica Ping-Pong del Coche (Movimiento en X) ------------------------------------
		rotllanta += rotllantaOffset * deltaTime; // Rotación continua de las llantas
		if (avanza) {
			// Si avanza, RESTAMOS para que viaje hacia la dirección correcta en X
			movCoche -= movOffset * deltaTime;
			if (movCoche <= -90.0f) {     // Límite de 90 unidades (hacia los negativos)
				movCoche = -90.0f;        // Tope exacto
				avanza = false;           // Da la vuelta
			}
		}
		else {
			// Si retrocede, SUMAMOS para que regrese a 0
			movCoche += movOffset * deltaTime;
			if (movCoche >= 0.0f) {       // Rregresamos al punto de partida
				movCoche = 0.0f;          // Tope exacto
				avanza = true;            // Vuelve a avanzar dando la vuelta
			}
		}

		// 2. Lógica Ping-Pong de la Nave (Avanza hacia -20 en X) -----------------------------
		if (avanzaNave) {
			// Si avanza, RESTAMOS para que viaje hacia el -20
			movNave -= movOffsetNave * deltaTime;
			if (movNave <= -90.0f) {      // Llegó al límite negativo
				movNave = -90.0f;
				avanzaNave = false;       // Da la vuelta para regresar
			}
		}
		else {
			// Si regresa, SUMAMOS para que vuelva al origen (0)
			movNave += movOffsetNave * deltaTime;
			if (movNave >= 0.0f) {        // Llegó al inicio
				movNave = 0.0f;
				avanzaNave = true;        // Vuelve a avanzar hacia el -20
			}
		}

		// 3. Lógica ZIGZAG de la Nave (Sube y Baja en Y) ------------------------------------
		if (subeNave) {
			alturaNave += velocidadAlturaNave * deltaTime;
			if (alturaNave >= 2.0f) {     // Llegó al tope superior
				alturaNave = 2.0f;
				subeNave = false;         // Ahora baja
			}
		}
		else {
			alturaNave -= velocidadAlturaNave * deltaTime;
			if (alturaNave <= -2.0f) {    // Llegó al tope inferior
				alturaNave = -2.0f;
				subeNave = true;          // Ahora sube
			}
		}

		// 4. Lógica Ping-Pong del Aleteo (Rotación en Z) -------------------------------------
		if (aleteaSube) {
			rotAleteo += velAleteo * deltaTime;
			if (rotAleteo >= limiteAleteo) {  // Llegó al límite superior de rotación
				rotAleteo = limiteAleteo;
				aleteaSube = false;           // Ahora rota hacia abajo
			}
		}
		else {
			rotAleteo -= velAleteo * deltaTime;
			if (rotAleteo <= -limiteAleteo) { // Llegó al límite inferior de rotación
				rotAleteo = -limiteAleteo;
				aleteaSube = true;            // Ahora rota hacia arriba
			}
		}
		*/

		// Capturar teclas y ratón para la cámara
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Preparación del Buffer y dibujo del fondo
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		// Activación del Shader Principal y recuperación de sus Uniforms
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		// ====================================================================================
		// FASE 1: CÁLCULO DE JERARQUÍAS DE LUCES
		// Aquí enlazamos las posiciones de las luces direccionales (spots) a objetos dinámicos.
		// ====================================================================================



		// ====================================================================================
		// FASE 2: ENVÍO DE LUCES AL SHADER
		// Empaquetamos las luces calculadas y las enviamos a la GPU.
		// ====================================================================================
		shaderList[0].SetDirectionalLight(&mainLight);
		unsigned int lucesPuntualesActivas = 0;
		// (La lógica para encender/apagar focos iría aquí, agregando al arreglo 'pointLights')
		shaderList[0].SetPointLights(pointLights, lucesPuntualesActivas);
		shaderList[0].SetSpotLights(spotLights, 0); // Modifica el 0 por tu contador de Spots activos
		// Limpieza de offsets y color para evitar arrastrar residuos visuales a los modelos
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// ====================================================================================
		// FASE 3: RENDERIZADO DE MODELOS OPACOS
		// Aquí se dibujan todos los elementos sólidos de la escena.
		// Compañeros: Agreguen aquí sus modelos usando este formato: 
		// model = glm::translate... -> uniformMatrix -> Textura -> Material -> RenderModel()
		// ====================================================================================

		// PISO BASE GRIS
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[2]->RenderMesh();

		/*
		// PISTA ------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -2.1f, -1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Pista_M.RenderModel();

		// COCHE Y CARROCERÍA -----------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche, -1.0f, 0.0f));
		if (!avanza) model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CocheT.RenderModel();

		// Llantas del coche ------------------------------------------------------------------
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		// Trasera derecha
		model = glm::translate(modelaux, glm::vec3(4.3f, 0.4f, 3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT.RenderModel();
		// Trasera izquierda
		model = glm::translate(modelaux, glm::vec3(4.3f, 0.4f, -3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT.RenderModel();
		// Delantera derecha
		model = glm::translate(modelaux, glm::vec3(-6.0f, 0.4f, 3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT.RenderModel();
		// Delantera izquierda
		model = glm::translate(modelaux, glm::vec3(-6.0f, 0.4f, -3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT.RenderModel();

		// Cofre ------------------------------------------------------------------------------
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(modelaux, glm::vec3(-4.2f, 2.6f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getAperturaCofre()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CofreT.RenderModel();

		// NAVE Y SUS ALAS (Jerarquía Zigzag + Aleteo) ----------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movNave, 13.0f + alturaNave, 1.5f));
		if (!avanzaNave) {
			model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		NaveT.RenderModel();
		// ALA 1 (Costado Derecho)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f));
		model = glm::rotate(model, rotAleteo * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala1T.RenderModel();
		// ALA 2 (Costado Izquierdo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::rotate(model, -rotAleteo * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala2T.RenderModel();
		*/

		// LÁMPARA HEXTECH
		// ====================================================================================
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -2.0f, -7.0f));
		model = glm::scale(model, glm::vec3(0.10f, 0.10f, 0.10f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Lampara_M.RenderModel();

		// ESCENARIO BUENO
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);

		// PISO MODELO (.OBJ)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piso.RenderModel();

		// VIAS
		model = glm::mat4(1.0);
		//model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M01.RenderModel();

		// TRENES
		// Tren chico
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-6.5f, 0.0f, 7.26f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M03.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-55.0f, 0.0f, 7.26f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();

		// Tren largo
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(108.0f, 0.0f, -8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M03.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(157.0f, 0.0f, -8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(214.0f, 0.0f, -8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(270.0f, 0.0f, -8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();





		// Edificio Principañ
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M02.RenderModel();

		// Barandal Edificio Principal
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M14.RenderModel();

		// Barandal Biblioteca
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -50.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M14.RenderModel();



		// BIblioteca
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M23.RenderModel();

		// Librería Aire libre
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-60.0f, 0.0f, -25.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M25.RenderModel();

		// Nave estacionada
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(55.0f, 0.0f, -45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M16.RenderModel();

		// VEGETACIÓN -----------------------------------------------------------------------

		// Pino 1 (Cerca de la Nave)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 2 (Cerca de la Biblioteca)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 3 (Cerca del Edificio Principal)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, 70.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();

		// Pino 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-175.0f, 0.0f, -105.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 5
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-170.0f, 0.0f, -10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 6
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-35.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 7
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(165.0f, 0.0f, 45.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 8
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 0.0f, 35.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();



		
		// Cerezo 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-60.0f, 0.0f, 45.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(155.0f, 0.0f, -20.0f));
		model = glm::rotate(model, -75 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-90.0f, 0.0f, 20.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 5
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 0.0f, 75.0f));
		model = glm::rotate(model, 105 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 6
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-80.0f, 0.0f, -75.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 7
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 105 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();

		// -----------------------------------------------------------------------------------


		// Coche 1 (Estacionado cerca del Edificio Principal)
		model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 55.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		// Coche 2 (Estacionado cerca de la Biblioteca)
		model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 55.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		// Coche 3 (Estacionado cerca del Edificio Principal)
		model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 55.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();



		// Coche 4 (Estacionado cerca del Edificio Principal)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 55.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		// Coche 5 (Estacionado cerca del Edificio Principal)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 55.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		// Coche 6 (Estacionado cerca de la biblioteca)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		// Coche 7 (Estacionado cerca de la biblioteca)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();




		// Puesto Ziggs
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -30.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M22.RenderModel();

		// Estructura (Al lado del puesto Ziggs)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -30.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M12.RenderModel();

		// Estructura (Al lado de la nave)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, 0.0f, -30.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M11.RenderModel();

		// LINEA DE EDIFICIOS -----------------------------------------------------------------

		// Edificio No 3 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M05.RenderModel();
		// Edificio No 3 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M05.RenderModel();

		// Edificio No 5 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M06.RenderModel();
		// Edificio No 5 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M06.RenderModel();

		// Edificio No 7 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M08.RenderModel();
		// Edificio No 7 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M08.RenderModel();


		// Edificio No 1 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		// Edificio No 8 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		// Edificio No 1 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		// Edificio No 8 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();


		// Edificio No 2 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_1.RenderModel();
		// Edificio No 2 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, -125.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_1.RenderModel();


		// Edificio No 6 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_3.RenderModel();
		// Edificio No 6 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, -125.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_3.RenderModel();


		// Edificio No 6 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_2.RenderModel();
		// Edificio No 6 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -125.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_4.RenderModel();

		// ------------------------------------------------------------------------------------




		// ====================================================================================


		// ====================================================================================
		// FASE 4: ELEMENTOS CON TRANSPARENCIA (BLENDING) Y TEXTURAS ANIMADAS
		// El "Blending" debe ocurrir AL FINAL para que las transparencias calculen
		// correctamente la profundidad sobre los modelos opacos ya dibujados.
		// ====================================================================================
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		/*
		// 2. AGAVE (Transparencia Estática) --------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		*/

		glDisable(GL_BLEND); // Apagamos el blending al terminar esta fase
		// ====================================================================================

		// Desacoplamos el Shader para dejarlo limpio para el siguiente Frame
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}