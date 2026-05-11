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
// ====================================================================================

// Control de Tiempo (DeltaTime) para animaciones fluidas independientes de los FPS
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// ==========================================
// 3.1 - VARIABLES AVATAR (Juan Pablo)
// ==========================================
float tiempoAnimacion = 0.0f;
float velocidadAnimacion = 0.15f;
float rotacionAvatar = 0.0f;
float amplitudArticulacion = 35.0f;
// Variables de movimiento para el recorrido
float distanciaRecorrida = 0.0f;
float cuerpoPosX = 120.0f;
float cuerpoPosZ = -30.0f;
float cuerpoRotY = 0.0f; // Empezamos en 0 porque las piezas ya tienen el 180 interno
bool estaMoviendo = true;
float cronometroEspera = 0.0f;
float velocidadTrayecto = 0.10f;
float rotacionGlobal;
float radRot;
bool isMoving;
float velocity;

// ==========================================
// 3.2 - VARIABLES CICLO DYN (Evelyn)
// ==========================================
float temporizadorSkybox = 0.0f;
int indiceSkyboxActual = 0;
float duracionEtapaSkybox = 250.0f;	// VELOCIDAD DEL CICLO DÍA-NOCHE

// ==========================================
// 3.3 - MÁQUINA DE ESTADOS An2: (Evelyn)
// ==========================================
// --- TREN CHICO ---
int subEtapaTrenChico = 1;			// [1-5]
float temporizadorTrenChico = 0.0f; // Reloj individual tren ch
float distanciaTrenChico = 0.0f;	// Progreso q llevamos en las vías1
float trenChicoPosY = 0.0f;
float trenChicoPosZ = 7.26f;
float trenChicoRotY = 0.0f;
// --- TREN LARGO ---
int subEtapaTrenLargo = 6;			// [6-10]
float temporizadorTrenLargo = 0.0f; // Reloj individual tren lr
float distanciaTrenLargo = 0.0f;	//  Progreso q llevamos en las vías2
// --- HUMO: ANIMACIÓN CÍCLICA ---
float temporizadorCicloHumo = 0.0f; // 200.0f unidades por ciclo
float offsetYHumo = 0.0f;			// -3Y o +3Y
float escalaHumoAnimada = 0.0f;		// Escala final después de cálculos
glm::mat4 matrizHumoChico = glm::mat4(1.0f);	// Guardado de posición de la cabina 
glm::mat4 matrizHumoLargo = glm::mat4(1.0f);	// Guardado de posición de la cabina 

float offsetY;
float offsetZ;
int gatilloB;
float progreso;
float pSuave;
float escalaBase;
float deltaX;
float deltaY;
float cx, cz, cRotY;
float lx, lz, lRotY;
float posX, posZ, rotY;
int i;

// ====================================================================================
// 4. DECLARACIÓN DE OBJETOS DE LA ESCENA
// Vectores, cámara, texturas, modelos y materiales que existirán en el entorno.
// ====================================================================================
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;
Skybox arregloSkyboxes[8];	// Cambiamos enttre 8 variantes
Material Material_brillante;
Material Material_opaco;

// ==========================================
// 4.1 - TEXTURAS Y MODELOS (Evelyn)
// ==========================================
// === ESCENARIO ===
Model Piso;
Model M01, M02, M03, M04, M14;			// VIAS - ESTACION - CABINA - ANDEN - REJAS
Model M05, M06, M08, M21,M22;			// EDIFICIOS STEAMPUNK
Model M12, M23;							// ESTRUCTURAS (Edificios no tan steampunk)
Model M28_1, M28_2, M28_3, M28_4;		// EDIFICIOS "Steampunk" Editados en blender
// --- DECORACIÓN 1
Model M15, M16, M17, M18, M19;			// FUENTE - NAVE - COCHE 1 - BASURA - BANCA		
Model M25, M26, M31, M32, M33_1;		// LIBRERÍA - PINO - COCHE 2 - CEREZO - SEÑAL F1
Model M33_2, M34;							// SEÑAL F2, CARTEL
// --- DESCARTADOS
Model M27, M28, M29, M30;				// Usados dentro de blender
Model M07, M09, M10, M11, M13, M20;		// Por su no uso
// === RUNATERRA (League of Legends) ===
// --- NPC's 
Model LOL_00;							// ZIGGS
Model LOL_01, LOL_02, LOL_04, LOL_05;	// PORO - ANIVIA - PENGU - CRAB
Model LOL_12, LOL_13, LOL_14, LOL_15;	// XAYAH - ANNIE - TRISTANA - SEJUANI
// --- DECORACIÓN 2
Model LOL_03, LOL_06, LOL_07, LOL_08;	// TORRETA - WARD - MAESTRÍA - MAZO JAYCE
Model LOL_09, LOL_10, LOL_11, LOL_16;	// AHRI - BASE - LUMINARIA - MAZO POPPY
Model M24;								// RELOJ?
Texture texturaHumo, texturaPoro;		// HUMO - PORO BLUSH

// ==========================================
// 4.2 - TEXTURAS Y MODELOS (Juan Pablo)
// ==========================================
// AVATAR
Model hw_cuerpo;
Model hw_cabeza;
Model hw_espada;
Model hw_PiernaDerecha;
Model hw_PiernaIzquierda;
Model hw_BrazoDerecho;
Model hw_BrazoIzquierdo;
// BANCA HW
Model hw_banca;

// ==========================================
// 4.3 - TEXTURAS Y MODELOS (Tony)
// ==========================================

// ==========================================
// 4.4 - TEXTURAS Y MODELOS (Ruben)
// ==========================================


// ====================================================================================
// 5. SISTEMA DE ILUMINACIÓN
// Arreglos de luces que soporta el Shader.
// ====================================================================================
DirectionalLight arregloLucesDireccionales[8];	// Luces sincronizadas con el Skybox
PointLight pointLights[MAX_POINT_LIGHTS];		// Focos puntuales
SpotLight spotLights[MAX_SPOT_LIGHTS];			// Linternas/Faros direccionales
// Rutas de Shaders
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";


// ====================================================================================
// 6. FUNCIONES DE UTILIDAD Y GEOMETRÍA
// ====================================================================================

// ==========================================
// 6.1 - Normales promedio
// ==========================================
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

// ==========================================
// 6.2 - Crear objetos
// ==========================================
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

// ==========================================
// 6.3 - Crear Shaders
// ==========================================
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// ==========================================
// 6.4 - Evaluar ruta tren chico (Evelyn)
// ==========================================
void CalcularPosicionRutaTren(float distancia, float& outX, float& outZ, float& outRot) {
	if (distancia <= 317.5f) { // Etapa 1 y 2 (-319 a -1.5)
		outX = -319.0f + distancia;
		outZ = 7.26f;
		outRot = 0.0f;
	}
	else if (distancia <= 437.0f) { // Etapa 3 (-1.5 a 118)
		outX = -1.5f + (distancia - 317.5f);
		outZ = 7.26f;
		outRot = 0.0f;
	}
	else if (distancia <= 451.0f) { // Etapa 4 - Curva Mini 1 (118 a 132)
		float factor = (distancia - 437.0f) / 14.0f;
		outX = 118.0f + factor * (132.0f - 118.0f);
		outZ = 7.26f;
		outRot = factor * (-10.0f);
	}
	else if (distancia <= 464.88f) { // Etapa 4 - Curva Mini 2 (132 a 144.6)
		float factor = (distancia - 451.0f) / 13.88f;
		outX = 132.0f + factor * (144.6f - 132.0f);
		outZ = 7.26f + factor * (13.1f - 7.26f);
		outRot = -10.0f + factor * (-23.6f); // Llega a -33.6
	}
	else if (distancia <= 482.93f) { // Etapa 4 - Curva Mini 3 (144.6 a 150.1)
		float factor = (distancia - 464.88f) / 18.05f;
		outX = 144.6f + factor * (150.1f - 144.6f);
		outZ = 13.1f + factor * (30.3f - 13.1f);
		outRot = -33.6f + factor * (-36.4f); // Llega a -70
	}
	else if (distancia <= 494.88f) { // Etapa 4 - Curva Mini 4 (150.1 a 151.1)
		float factor = (distancia - 482.93f) / 11.95f;
		outX = 150.1f + factor * (151.1f - 150.1f);
		outZ = 30.3f + factor * (42.25f - 30.3f);
		outRot = -70.0f + factor * (-20.0f); // Llega a -90
	}
	else { // Etapa 5 (Avance por Z)
		float avance = distancia - 494.88f;
		outX = 151.1f;
		outZ = 42.25f + avance; // El avance ahora llegará hasta 350.0f
		outRot = -90.0f;
	}
}

// ==========================================
// 6.5 - Evaluar ruta tren largo (Evelyn)
// ==========================================
void CalcularPosicionRutaTrenLargo(float distancia, float& outX, float& outZ, float& outRot) {
	if (distancia <= 316.0f) { // Etapa 6 y 7 (330 a 14)
		outX = 330.0f - distancia; // Va hacia los negativos en X
		outZ = -8.0f;
		outRot = 180.0f;
	}
	else if (distancia <= 451.0f) { // Etapa 8 (14 a -121)
		outX = 14.0f - (distancia - 316.0f);
		outZ = -8.0f;
		outRot = 180.0f;
	}
	else if (distancia <= 464.1f) { // Etapa 9 - Curva Mini 1 (-121 a -134.1)
		float factor = (distancia - 451.0f) / 13.1f;
		outX = -121.0f - factor * 13.1f;
		outZ = -8.0f;
		outRot = 180.0f + factor * (-10.0f); // Gira de 180° a 170°
	}
	else if (distancia <= 477.6f) { // Etapa 9 - Curva Mini 2
		float factor = (distancia - 464.1f) / 13.5f;
		outX = -134.1f - factor * 11.8f; // Llega a -145.9
		outZ = -8.0f - factor * 6.6f;    // Llega a -14.6
		outRot = 170.0f + factor * (-23.6f); // Gira de 170° a 146.4°
	}
	else if (distancia <= 497.7f) { // Etapa 9 - Curva Mini 3
		float factor = (distancia - 477.6f) / 20.1f;
		outX = -145.9f - factor * 5.1f;  // Llega a -151.0
		outZ = -14.6f - factor * 19.5f;  // Llega a -34.1
		outRot = 146.4f + factor * (-36.4f); // Gira a 110°
	}
	else if (distancia <= 511.3f) { // Etapa 9 - Curva Mini 4
		float factor = (distancia - 497.7f) / 13.6f;
		outX = -151.0f - factor * 1.0f;  // Llega a -152.0
		outZ = -34.1f - factor * 13.55f; // Llega a -47.65
		outRot = 110.0f + factor * (-20.0f); // Gira a 90° exactos
	}
	else { // Etapa 10 (Avance por Z hacia -500)
		float avance = distancia - 511.3f;
		outX = -152.0f;
		outZ = -47.65f - avance;
		outRot = 90.0f;
	}
}



// ====================================================================================
// 7. FUNCIÓN PRINCIPAL (MAIN)
// ====================================================================================
int main()
{

	// =========================================================
	// --- 7.1. Inicialización de Ventana y Cámara ---
	// =========================================================
	mainWindow = Window(1800, 900); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CreateShaders();
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);


	// =========================================================
	// --- 7.2. Carga de Assets (Texturas y Modelos 3D) ---
	// =========================================================
	// -----------------------> 7.2.1 - Evelyn
	Piso = Model();				Piso.LoadModel("Models/PISO.obj");
	M01 = Model();				M01.LoadModel("Models/01.obj");
	M02 = Model();				M02.LoadModel("Models/02.obj");
	M03 = Model();				M03.LoadModel("Models/03.obj");
	M04 = Model();				M04.LoadModel("Models/04.obj");
	M05 = Model();				M05.LoadModel("Models/05.obj");
	M06 = Model();				M06.LoadModel("Models/06.obj");
	M08 = Model();				M08.LoadModel("Models/08.obj");
	M12 = Model();				M12.LoadModel("Models/12.obj");
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
	M33_1 = Model();			M33_1.LoadModel("Models/33-1.obj");
	M33_2 = Model();			M33_2.LoadModel("Models/33-2.obj");
	M34 = Model();				M34.LoadModel("Models/34.obj");
	LOL_00 = Model();			LOL_00.LoadModel("Models/LOL_00.obj");
	LOL_01 = Model();			LOL_01.LoadModel("Models/LOL_01.obj");
	LOL_02 = Model();			LOL_02.LoadModel("Models/LOL_02.obj");
	LOL_03 = Model();			LOL_03.LoadModel("Models/LOL_03.obj");
	LOL_04 = Model();			LOL_04.LoadModel("Models/LOL_04.obj");
	LOL_05 = Model();			LOL_05.LoadModel("Models/LOL_05.obj");
	LOL_06 = Model();			LOL_06.LoadModel("Models/LOL_06.obj");
	LOL_07 = Model();			LOL_07.LoadModel("Models/LOL_07.obj");
	LOL_08 = Model();			LOL_08.LoadModel("Models/LOL_08.obj");
	LOL_09 = Model();			LOL_09.LoadModel("Models/LOL_09.obj");
	LOL_10 = Model();			LOL_10.LoadModel("Models/LOL_10.obj");
	LOL_11 = Model();			LOL_11.LoadModel("Models/LOL_11.obj");
	LOL_12 = Model();			LOL_12.LoadModel("Models/LOL_12.obj");
	LOL_13 = Model();			LOL_13.LoadModel("Models/LOL_13.obj");
	LOL_14 = Model();			LOL_14.LoadModel("Models/LOL_14.obj");
	LOL_15 = Model();			LOL_15.LoadModel("Models/LOL_15.obj");
	LOL_16 = Model();			LOL_16.LoadModel("Models/LOL_16.obj");
	texturaHumo = Texture("Textures/HUMO.tga");		texturaHumo.LoadTextureA();
	texturaPoro = Texture("Textures/35_1.png");	texturaPoro.LoadTextureA();

	// -----------------------> 7.2.2 - Juan Pablo
	hw_cuerpo = Model();			hw_cuerpo.LoadModel("Models/hw_cuerpo.obj");
	hw_cabeza = Model();			hw_cabeza.LoadModel("Models/hw_cabeza.obj");
	hw_espada = Model();			hw_espada.LoadModel("Models/hw_espada.obj");
	hw_PiernaDerecha = Model();		hw_PiernaDerecha.LoadModel("Models/hw_PiernaDerecha.obj");
	hw_PiernaIzquierda = Model();	hw_PiernaIzquierda.LoadModel("Models/hw_PiernaIzquierda.obj");
	hw_BrazoDerecho = Model();		hw_BrazoDerecho.LoadModel("Models/hw_BrazoDerecho.obj");
	hw_BrazoIzquierdo = Model();	hw_BrazoIzquierdo.LoadModel("Models/hw_BrazoIzquierdo.obj");
	hw_banca.LoadModel("Models/hw_banca.obj");
	// -----------------------> 7.2.3 - Tony

	// -----------------------> 7.2.4 - Ruben

	// =========================================================
	// --- 7.3. Carga de Skybox (Evelyn) --- y Materiales
	// =========================================================
	std::string prefijosSkybox[8] = {
		"11Amanecer", "12Amanecer", "21Dia", "22Dia",
		"31Atardecer", "32Atardecer", "41Noche", "42Noche"
	};
	for (i = 0; i < 8; i++) {
		std::vector<std::string> skyboxFaces;
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_px.png"); // +X Derecha
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_nx.png"); // -X Izquierda
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_ny.png"); // -Y Abajo (PRIMERO)
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_py.png"); // +Y Arriba (DESPUÉS)
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_pz.png"); // +Z Atrás
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_nz.png"); // -Z Frente
		arregloSkyboxes[i] = Skybox(skyboxFaces);
	}
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// =========================================================
	// --- 7.4. Configuración Inicial de Iluminación (Evelyn) --
	// =========================================================

	// ----- 7.4.1 - Arreglos auxiliares de posiciones (X, Y, Z)
	glm::vec3 posicionesPostes[8] = {
		glm::vec3(-86.0f, 0.0f, -85.0f),  // Poste 1
		glm::vec3(84.0f, 0.0f, -85.0f),   // Poste 2
		glm::vec3(84.0f, 0.0f, 84.0f),    // Poste 3
		glm::vec3(-86.0f, 0.0f, 84.0f),   // Poste 4
		glm::vec3(-86.0f, 0.0f, 0.0f),    // Poste 5
		glm::vec3(84.0f, 0.0f, 0.0f),     // Poste 6
		glm::vec3(0.0f, 0.0f, 75.0f),     // Poste 7
		glm::vec3(0.0f, 0.0f, -75.0f)     // Poste 8
	};
	glm::vec3 posicionesEdificiosM28[6] = {
		glm::vec3(75.0f, 0.0f, 125.0f),   // M28_1 (Espejeado)
		glm::vec3(-75.0f, 0.0f, -125.0f), // M28_1 (NO Espejeado)
		glm::vec3(-45.0f, 0.0f, 125.0f),  // M28_3 (Espejeado)
		glm::vec3(45.0f, 0.0f, -125.0f),  // M28_3 (NO Espejeado)
		glm::vec3(15.0f, 0.0f, 125.0f),   // M28_2 (Espejeado)
		glm::vec3(-15.0f, 0.0f, -125.0f)  // M28_4 (NO Espejeado)
	};

	// ----- 7.4.2: LUZ DIRECCIONAL GLOBAL (Sincronizada con los Skyboxes)
	// 0: 11Amanecer (Amarillo claro)
	arregloLucesDireccionales[0] = DirectionalLight(1.0f, 0.9f, 0.6f, 0.1f, 0.1f, 0.0f, -1.0f, 0.0f);
	// 1: 12Amanecer (Casi blanco)
	arregloLucesDireccionales[1] = DirectionalLight(1.0f, 0.95f, 0.9f, 0.22f, 0.28f, 0.0f, -1.0f, 0.0f);
	// 2: 21Dia (Casi blanco)
	arregloLucesDireccionales[2] = DirectionalLight(1.0f, 0.95f, 0.9f, 0.44f, 0.44f, 0.0f, -1.0f, 0.0f);
	// 3: 22Dia (Blanco)
	arregloLucesDireccionales[3] = DirectionalLight(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.0f, -1.0f, 0.0f);
	// 4: 31Atardecer (Naranja Claro)
	arregloLucesDireccionales[4] = DirectionalLight(1.0f, 0.7f, 0.4f, 0.44f, 0.44f, 0.0f, -1.0f, 0.0f);
	// 5: 32Atardecer (Naranja)
	arregloLucesDireccionales[5] = DirectionalLight(1.0f, 0.5f, 0.1f, 0.22f, 0.28f, 0.0f, -1.0f, 0.0f);
	// 6: 41Noche (Azul oscuro)
	arregloLucesDireccionales[6] = DirectionalLight(0.1f, 0.1f, 0.5f, 0.1f, 0.1f, 0.0f, -1.0f, 0.0f);
	// 7: 42Noche (Morado oscuro)
	arregloLucesDireccionales[7] = DirectionalLight(0.3f, 0.1f, 0.4f, 0.05f, 0.05f, 0.0f, -1.0f, 0.0f);

	// ----- 7.4.3: LUCES PUNTUALES (Point Lights)
	unsigned int pointLightCount = 0;
	for (i = 0; i < 8; i++) {
		pointLights[i] = PointLight(1.0f, 0.9f, 0.5f,	// Color amarillento (1.0, 0.9, 0.5)
			0.5f, 1.0f,									// Intensidad ambiental y difusa
			posicionesPostes[i].x, posicionesPostes[i].y + 15.0f, posicionesPostes[i].z,	//+15Y
			1.0f, 0.05f, 0.01f);						// Atenuación suave
	}
	pointLightCount = 8;

	

	// ----- 7.4.4: LUCES SPOT (Linternas y Faros) 
	unsigned int spotLightCount = 0;
	// Edificios M28 (Índices 0 a 5) - Luces fijas con dirección hacia el piso
	for (i = 0; i < 6; i++) {
		offsetY = 33.0f;							// +33Y
		// Para Z: +125 restamos 14  // -125 sumamos 14
		offsetZ = (posicionesEdificiosM28[i].z > 0) ? -14.0f : 14.0f;
		// Calculamos posicion de la luz con jerarquía (Edificio + Offset)
		glm::vec3 posicionLuz = posicionesEdificiosM28[i] + glm::vec3(0.0f, offsetY, offsetZ);
		spotLights[i] = SpotLight(1.0f, 0.9f, 0.5f,  // Color amarillo claro
			1.0f, 1.0f,                              // Intensidades (Ambiental, Difusa)
			posicionLuz.x, posicionLuz.y, posicionLuz.z, // Posición
			0.0f, -1.0f, 0.0f,                       // Dirección: Apuntando hacia piso
			1.0f, 0.045f, 0.0075f,                   // Atenuación 
			45.0f);                                  // Ángulo de apertura del cono
	}
	// Faros móviles (Índices 6 y 7) - Luces con dirección hacia adelante (Z negativo)
	spotLights[6] = SpotLight(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.045f, 0.0075f, 30.0f);
	spotLights[7] = SpotLight(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.045f, 0.0075f, 30.0f);
	spotLightCount = 8;



	// =========================================================
	// --- 7.5. Ubicaciones del Shader (Uniforms)
	// =========================================================
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// ----- 7.5.1: Inicialización de matrices y vectores de apoyo
	GLfloat   now = 0.0f;
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 modelaux = glm::mat4(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);
	
	// ----- 7.5.2:  VARIABLE DE CONTROL DE CÁMARAS (Juan Pablo)
	// 0 = F (1ra Persona), 1 = G (3ra Persona), 2 = H (Aérea), 3 = J (Libre)
	int tipoCamara = 3; // Iniciamos en 3 (Libre) por default


	// ====================================================================================
	// 8. GAME LOOP (Ciclo Principal de Renderizado)
	// ====================================================================================
	lastTime = glfwGetTime();
	while (!mainWindow.getShouldClose())
	{
		// Control de cuadros por segundo (Cálculo del DeltaTime)
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// --- 8.1 FÍSICAS Y ANIMACIONES MATEMÁTICAS
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.1: LÓGICA DE CAMBIO DE SKYBOX (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		temporizadorSkybox += deltaTime;	// Tiempo transcurrido entre cada frame
		// Al superar uan etapa (250f)
		if (temporizadorSkybox >= duracionEtapaSkybox) {
			temporizadorSkybox -= duracionEtapaSkybox;
			indiceSkyboxActual++;			// Pasamos al proximo skybox
			if (indiceSkyboxActual >= 8) {	// Reiniciamos al Amanecer (0)
				indiceSkyboxActual = 0;
			}
		}

		


		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.2: LÓGICA DE ANIMACION B1 - Reloj (Tony)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


		
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.3: LÓGICA DE ANIMACION B2 - Locomotora (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		gatilloB = mainWindow.getContadorTeclaB();	// ¿Cuantas veces hemos apretado B?

		// RESET AUTOMÁTICO AL VOLVER A 0
		if (gatilloB == 0) {
			// Reseteamos tren chico y largo
			subEtapaTrenChico = 1;
			temporizadorTrenChico = 0.0f;
			distanciaTrenChico = 0.0f;
			subEtapaTrenLargo = 6;
			temporizadorTrenLargo = 0.0f;
			distanciaTrenLargo = 0.0f;
		}
		// Maquina del tren chico (Índices 1 a 5)
		if (gatilloB >= 1) {
			// --- ETAPA 1: Avanzar y frenar suavemente ---
			if (subEtapaTrenChico == 1) {
				temporizadorTrenChico += deltaTime;
				progreso = temporizadorTrenChico / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				// Función seno:Para frenado suave
				pSuave = sin(progreso * 1.570796f);	// 1.570796f es PI/2
				distanciaTrenChico = 317.5f * pSuave;		// 317.5 dist_total a recorrer en esta etapa
				if (progreso >= 1.0f && gatilloB >= 2) {	// ¿B se apreto ya dos veces y llegamos a la meta?
					subEtapaTrenChico = 2;
					temporizadorTrenChico = 0.0f;			// Reset del reloj para nueva etapa
				}
			}
			// --- ETAPA 2: Espera en el andén ---
			else if (subEtapaTrenChico == 2) {
				temporizadorTrenChico += deltaTime;
				// Solo espera inactivo durante la mitad del tiempo de un skybox
				if (temporizadorTrenChico >= (duracionEtapaSkybox / 2.0f)) {
					subEtapaTrenChico = 3;
					temporizadorTrenChico = 0.0f;
				}
			}
			// --- ETAPA 3: Arranque lento (Ease-In) en X ---
			else if (subEtapaTrenChico == 3) {
				temporizadorTrenChico += deltaTime;
				progreso = temporizadorTrenChico / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				// Función cuadrática: Inicia lento (cerca de 0) y acelera progresivamente
				pSuave = progreso * progreso;			// Curva cuadrática para arranque lento
				distanciaTrenChico = 317.5f + (119.5f * pSuave); // 119.5 es la distancia extra a recorrer
				// Transición automática al llegar a la curva
				if (progreso >= 1.0f) {
					subEtapaTrenChico = 4;
					temporizadorTrenChico = 0.0f;
				}
			}
			// --- ETAPA 4: Curva ---
			else if (subEtapaTrenChico == 4) {
				temporizadorTrenChico += deltaTime;
				progreso = temporizadorTrenChico / (duracionEtapaSkybox / 2.0f);
				if (progreso >= 1.0f) progreso = 1.0f;
				distanciaTrenChico = 437.0f + (57.88f * progreso); // 57.88 es la distancia extra a recorrer
				if (progreso >= 1.0f) {
					subEtapaTrenChico = 5;
					temporizadorTrenChico = 0.0f;
				}
			}
			// --- ETAPA 5: Avance en Z ---
			else if (subEtapaTrenChico == 5) {
				temporizadorTrenChico += deltaTime;
				progreso = temporizadorTrenChico / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				distanciaTrenChico = 494.88f + (307.75f * progreso); // 307.75 es la distancia extra a recorrer
			}
		}

		// Maquina del tren largo (Índices 6 a 10)
		if (gatilloB >= 3) {
			// --- ETAPA 6: Avanzar y frenar suavemente ---
			if (subEtapaTrenLargo == 6) {
				temporizadorTrenLargo += deltaTime;
				progreso = temporizadorTrenLargo / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				pSuave = sin(progreso * 1.570796f);
				distanciaTrenLargo = 316.0f * pSuave; // 316.0 es la distancia de 330 a 14
				// Transición a Etapa 7 SOLO si ya apretamos la B por cuarta vez
				if (progreso >= 1.0f && gatilloB >= 4) {
					subEtapaTrenLargo = 7;
					temporizadorTrenLargo = 0.0f;
				}
			}
			// --- ETAPA 7: Espera en el andén ---
			else if (subEtapaTrenLargo == 7) {
				temporizadorTrenLargo += deltaTime;
				if (temporizadorTrenLargo >= (duracionEtapaSkybox / 2.0f)) {
					subEtapaTrenLargo = 8;
					temporizadorTrenLargo = 0.0f;
				}
			}
			// --- ETAPA 8: Arranque lento en X ---
			else if (subEtapaTrenLargo == 8) {
				temporizadorTrenLargo += deltaTime;
				progreso = temporizadorTrenLargo / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				pSuave = progreso * progreso;
				distanciaTrenLargo = 316.0f + (135.0f * pSuave); // 135 es la distancia de 14 a -121
				if (progreso >= 1.0f) {
					subEtapaTrenLargo = 9;
					temporizadorTrenLargo = 0.0f;
				}
			}
			// --- ETAPA 9: La Curva de 90° (Derecha) ---
			else if (subEtapaTrenLargo == 9) {
				temporizadorTrenLargo += deltaTime;
				progreso = temporizadorTrenLargo / (duracionEtapaSkybox / 2.0f);
				if (progreso >= 1.0f) progreso = 1.0f;
				distanciaTrenLargo = 451.0f + (60.3f * progreso); // 60.3 es la distancia extra a recorrer
				if (progreso >= 1.0f) {
					subEtapaTrenLargo = 10;
					temporizadorTrenLargo = 0.0f;
				}
			}
			// --- ETAPA 10: Avance recto en Z ---
			else if (subEtapaTrenLargo == 10) {
				temporizadorTrenLargo += deltaTime;
				progreso = temporizadorTrenLargo / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				distanciaTrenLargo = 511.3f + (452.35f * progreso);	// 452.35 es la distancia extra a recorrer
			}
		}




		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.4: LÓGICA DE ANIMACION B3 - Árbol (Ruben)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>





		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.2: LÓGICA DE ANIMACION A1 - Coche (Ruben)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>





		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.3: LÓGICA DE ANIMACION A2 - Humo (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		temporizadorCicloHumo += deltaTime;
		if (temporizadorCicloHumo >= 200.0f) {	// El ciclo completo dura 200.0f
			temporizadorCicloHumo -= 200.0f;	
		}
		escalaBase = 0.0f;
		// ETAPA 1 y 2 (Primeros 50 frames): De -3Y a 0Y expandiendo escala de 0 a 5.
		if (temporizadorCicloHumo <= 50.0f) {
			progreso = temporizadorCicloHumo / 50.0f; // Normalizamos de 0.0 a 1.0
			offsetYHumo = -3.0f + (progreso * 3.0f);        // Sube de -3 a 0
			escalaBase = progreso * 5.0f;                   // Crece de 0 a 5
		}
		// ETAPA 3 (Siguientes 100 frames): Mantenemos 0Y y escala 5
		else if (temporizadorCicloHumo <= 150.0f) {
			offsetYHumo = 0.0f;
			escalaBase = 5.0f;
		}
		// ETAPA 4 (Últimos 50 frames): De 0Y a +3Y encogiendo escala de 5 a 0
		else {
			progreso = (temporizadorCicloHumo - 150.0f) / 50.0f; // Normalizamos de 0.0 a 1.0
			offsetYHumo = progreso * 3.0f;               // Sube de 0 a +3
			escalaBase = 5.0f - (progreso * 5.0f);       // Se encoge de 5 a 0
		}
		// Agregamos seno encima para ver el humo "vibrando"
		escalaHumoAnimada = escalaBase + (sin(glfwGetTime() * 3.0f) * 1.5f * (escalaBase / 5.0f));
		// Prevenir escalas negativas
		if (escalaHumoAnimada < 0.0f) escalaHumoAnimada = 0.0f;




		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.4: LÓGICA DE ANIMACION A3 - Disparo (Ruben)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// --- 8.2 LOGICA DE CONTROL DE CÁMARAS Y MOVIMIENTO DEL AVATAR (Juan Pablo)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	
		// Capturar eventos de la ventana
		glfwPollEvents();
		// --- 1. ACTUALIZAR ESTADO DE CÁMARA ---
		if (mainWindow.getAccionF()) { tipoCamara = 0; mainWindow.apagarAccionF(); }
		if (mainWindow.getAccionG()) { tipoCamara = 1; mainWindow.apagarAccionG(); }
		if (mainWindow.getAccionH()) { tipoCamara = 2; mainWindow.apagarAccionH(); }
		if (mainWindow.getAccionJ()) { tipoCamara = 3; mainWindow.apagarAccionJ(); }

		// Leer el ratón una sola vez
		deltaX = mainWindow.getXChange();
		deltaY = mainWindow.getYChange();

		// --- 2. LÓGICA DEL RATÓN ---
		if (tipoCamara == 3) {
			// [J] CÁMARA LIBRE: Controla su propio giro
			camera.mouseControl(deltaX, deltaY);
		}
		else {
			// CÁMARAS ANCLADAS: El ratón gira la cintura/cuerpo del personaje
			// Ajusta el 0.3f si quieres que el ratón gire al personaje más rápido o más lento
			cuerpoRotY -= deltaX * 0.3f;
		}
		
		// --- 3. CÁLCULO DE VECTORES DIRECCIONALES DEL AVATAR ---
		glm::vec3 posAvatar(cuerpoPosX, 1.0f, cuerpoPosZ);
		rotacionGlobal = 180.0f + cuerpoRotY;
		radRot = rotacionGlobal * toRadians;

		// Vector que apunta hacia el frente del personaje
		glm::vec3 forwardAvatar(sin(radRot), 0.0f, cos(radRot));
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		// Vector que apunta hacia la derecha del personaje
		glm::vec3 rightAvatar = glm::normalize(glm::cross(up, forwardAvatar));

		// --- 4. CONTROL DE MOVIMIENTO (A, W, S, D) Y ANIMACIÓN ---
		isMoving = false;
		velocity = 0.15f * deltaTime; //la velocidad de caminar

		if (tipoCamara == 3) {
			// Si es cámara libre, el WASD mueve a la cámara por el aire, no al avatar
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
		}
		else {
			// Si es cámara de jugador, el WASD mueve al avatar
			if (mainWindow.getsKeys()[GLFW_KEY_W]) {
				cuerpoPosX += forwardAvatar.x * velocity;
				cuerpoPosZ += forwardAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_S]) {
				cuerpoPosX -= forwardAvatar.x * velocity;
				cuerpoPosZ -= forwardAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_D]) {
				cuerpoPosX -= rightAvatar.x * velocity;
				cuerpoPosZ -= rightAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_A]) {
				cuerpoPosX += rightAvatar.x * velocity; 
				cuerpoPosZ += rightAvatar.z * velocity;
				isMoving = true;
			}
		}

		// --- 5. ANIMACIÓN CONDICIONADA A MOVIMIENTO ---
		if (isMoving) {
			// Usamos una función seno basada en el tiempo para oscilar brazos/piernas de forma natural
			// 10.0f = Qué tan rápido mueve los brazos. 45.0f = Cuántos grados se levantan.
			rotacionAvatar = sin(glfwGetTime() * 10.0f) * 45.0f;
		}
		else {
			rotacionAvatar = 0.0f; // Personaje quieto con extremidades rectas
		}

		// --- 6. CÁLCULO FINAL DE LAS MATRICES DE VISTA ---
		glm::mat4 viewMatrix;
		glm::vec3 posCamara;
		glm::vec3 targetCamara;

		if (tipoCamara == 3) {
			viewMatrix = camera.calculateViewMatrix();
			posCamara = camera.getCameraPosition();
		}
		else {
			// Actualizamos la posición base del avatar por si se movió
			posAvatar = glm::vec3(cuerpoPosX, 1.0f, cuerpoPosZ);

			if (tipoCamara == 0) { // F (Primera Persona)
				posCamara = posAvatar + glm::vec3(0.0f, 1.2f, 0.0f) + forwardAvatar * 1.2f;
				targetCamara = posCamara + forwardAvatar;
			}
			else if (tipoCamara == 1) { // G (Hombro / Tercera Persona)
				posCamara = posAvatar + glm::vec3(0.0f, 1.8f, 0.0f) - forwardAvatar * 5.0f + rightAvatar * 1.0f;
				targetCamara = posAvatar + glm::vec3(0.0f, 1.2f, 0.0f) + forwardAvatar * 5.0f;
			}
			else if (tipoCamara == 2) { // H (Aérea)
				posCamara = posAvatar + glm::vec3(0.0f, 50.0f, -0.1f);
				targetCamara = posAvatar;
			}
			viewMatrix = glm::lookAt(posCamara, targetCamara, up);
		}


		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// --- 8.3 PREPARACIÓN DE SHADERS, ENVÍO DE UNIFORMS Y DIBUJADO DE OBJETOS
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// Preparación del Buffer y dibujo del fondo
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		arregloSkyboxes[indiceSkyboxActual].DrawSkybox(camera.calculateViewMatrix(), projection);
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
		// Usamos viewMatrix y posCamara
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition, posCamara.x, posCamara.y, posCamara.z);


		// ===============================================================================
		// --- 8.4: CÁLCULO DE JERARQUÍAS DE LUCES
		// ===============================================================================
		// 1. FARO DEL TREN CHICO (Tecla 7 -> Índice 6)
		
		CalcularPosicionRutaTren(distanciaTrenChico, cx, cz, cRotY);
		// Creamos la matriz del "padre" (el tren) para usarla como ancla de los "hijos" (luz y humo)
		glm::mat4 trenChicoMat = glm::mat4(1.0f);
		trenChicoMat = glm::translate(trenChicoMat, glm::vec3(cx, 0.0f, cz));
		trenChicoMat = glm::rotate(trenChicoMat, cRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 posFaroChico = glm::vec3(trenChicoMat * glm::vec4(18.0f, 9.5f, 0.0f, 1.0f));
		glm::vec3 dirFaroChico = glm::normalize(glm::vec3(trenChicoMat * glm::vec4(1.0f, -0.2f, 0.0f, 0.0f)));
		// Sobrescribimos el SpotLight 6 con las nuevas coordenadas y direcciones calculadas
		spotLights[6] = SpotLight(1.0f, 1.0f, 1.0f,   // Color Blanco
			1.0f, 1.0f,                               // Intensidades
			posFaroChico.x, posFaroChico.y, posFaroChico.z, // Posición acoplada
			dirFaroChico.x, dirFaroChico.y, dirFaroChico.z, // Dirección de enfoque
			1.0f, 0.022f, 0.0019f,                    // Atenuación (Valores bajos = llega más lejos)
			35.0f);                                   // Ángulo de apertura del cono de luz de 35°
		// 2. FARO DEL TREN LARGO (Tecla 8 -> Índice 7)
		
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo, lx, lz, lRotY);
		// Creamos la matriz del "padre" (el tren) para usarla como ancla de los "hijos" (luz y humo)
		glm::mat4 trenLargoMat = glm::mat4(1.0f);
		trenLargoMat = glm::translate(trenLargoMat, glm::vec3(lx, 0.0f, lz));
		trenLargoMat = glm::rotate(trenLargoMat, lRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 posFaroLargo = glm::vec3(trenLargoMat * glm::vec4(18.0f, 9.5f, 0.0f, 1.0f));
		glm::vec3 dirFaroLargo = glm::normalize(glm::vec3(trenLargoMat * glm::vec4(1.0f, -0.2f, 0.0f, 0.0f)));
		// Sobrescribimos el SpotLight 7 con las nuevas coordenadas y direcciones calculadas
		spotLights[7] = SpotLight(1.0f, 1.0f, 1.0f,   // Color Blanco
			1.0f, 1.0f,                               // Intensidades
			posFaroLargo.x, posFaroLargo.y, posFaroLargo.z,
			dirFaroLargo.x, dirFaroLargo.y, dirFaroLargo.z,
			1.0f, 0.022f, 0.0019f,                    // Atenuación para llegar mucho más lejos
			35.0f);                                   // Ángulo de apertura de 35°
		matrizHumoChico = trenChicoMat;
		matrizHumoLargo = trenLargoMat;

		

		// ===============================================================================
		// --- 8.5: ENVÍO DE LUCES AL SHADER
		// ===============================================================================
		
		// ENVIO de luz DIRECCIONAL (Siempre 1, cambia según el skybox)
		shaderList[0].SetDirectionalLight(&arregloLucesDireccionales[indiceSkyboxActual]);

		// ENVIO de luces PUNTUALES (Siempre 8, pero solo algunas activas según el skybox)
		unsigned int lucesPuntualesActivas = 0;
		if (indiceSkyboxActual == 0 || indiceSkyboxActual == 5 ||
			indiceSkyboxActual == 6 || indiceSkyboxActual == 7)
		{
			lucesPuntualesActivas = 8;
		}
		shaderList[0].SetPointLights(pointLights, lucesPuntualesActivas);

		// ENVIO de luces SPOT (Variable, según teclas 1 a 8)
		bool* estadosSpot = mainWindow.getStatusLucesSpot();
		SpotLight lucesSpotActivas[8];			// ATemp solo para las luces que estén encendidas
		unsigned int contadorSpotActivas = 0;	// Lleva la cuenta real de cuántas enviaremos
		for (i = 0; i < 8; i++) {			// Recoremos catalogo de las 8 SpotLights
			if (estadosSpot[i]) {
				lucesSpotActivas[contadorSpotActivas] = spotLights[i];
				contadorSpotActivas++;
			}
		}
		shaderList[0].SetSpotLights(lucesSpotActivas, contadorSpotActivas);

		// Limpieza de offsets de textura y color
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));



		// ===============================================================================
		// --- 8.6: RENDERIZADO DE MODELOS OPACOS
		// ===============================================================================
		

		// >>>>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - EVELYN >>>>>>>>>>>>>>>>>>>>>>>>>
		// === ESCENARIO ===
		// PISO BASE
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piso.RenderModel();
		// VIAS
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M01.RenderModel();
		// ESTACIÓN (Edificio Principal)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M02.RenderModel();

		// === TREN CHICO ===
		// 1. DIBUJAR CABINA
		CalcularPosicionRutaTren(distanciaTrenChico, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model; // Guardamos para el Poro (El Poro SÍ va montado en la cabina)
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M03.RenderModel();
		// 2. DIBUJAR PORO (Jerarquía Mantenida)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-12.63f, 9.28f, -0.13f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// 3. DIBUJAR ANDEN/VAGON (Jerarquía Rota: Sigue la misma ruta con retraso de 48.5 unidades)
		CalcularPosicionRutaTren(distanciaTrenChico - 48.5f, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();

		// === TREN LARGO ===
		// 1. DIBUJAR CABINA
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M03.RenderModel();
		// 2. DIBUJAR VAGON 1 (49 unidades detrás)
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo - 49.0f, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		// 3. DIBUJAR VAGON 2 (106 unidades detrás)
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo - 106.0f, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		// 4. DIBUJAR VAGON 3 (162 unidades detrás)
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo - 162.0f, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();

		// === ESCENARIO ===
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

		// === EDIFICIOS STEAMPUNK ===
		// Edificio No 3 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M05.RenderModel();
		// Edificio No 3 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, -120.0f));
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
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M08.RenderModel();
		// Edificio No 1 y 8 (Espejeado / No Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, 0.0f, -120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		// Puesto Ziggs
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M22.RenderModel();

		// === EDIFICIOS No tan steampunk ===
		// Estructura (Al lado del puesto Ziggs)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M12.RenderModel();
		// Biblioteca
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M23.RenderModel();

		// === EDIFICIOS STEAMPUNK MODIFICADOS EN BLENDER ===
		// Edificio No 2 (Espejeado / No Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, -125.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_1.RenderModel();
		// Edificio No 6 (Espejeado / No Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_3.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, -125.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_3.RenderModel();
		// Edificio No 6 Variantes
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_2.RenderModel(); // M28_2 Espejeado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -125.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_4.RenderModel(); // M28_4 NO Espejeado


		// === DECORACION 1 ===
		// Mini fuente
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-120.0f, 0.0f, -34.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M15.RenderModel();
		// Nave estacionada
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(55.0f, 0.0f, -45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M16.RenderModel();
		// Coches 1 (M17)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 55.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		// Botes de basura
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.09f, 0.0f, -72.72f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-87.15f, 0.0f, 19.36f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.06f, 0.0f, -18.43f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(87.51f, 0.0f, 73.59f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		// Bancas (1 a 18)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-89.0f, 0.0f, -22.65f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-89.0f, 0.0f, -40.17f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-41.42f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-68.03f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.45f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.84f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(67.01f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.96f, 0.0f, -69.64f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.96f, 0.0f, -51.81f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(88.41f, 0.0f, 22.38f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(88.41f, 0.0f, 40.21f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.05f, 0.0f, 69.43f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.05f, 0.0f, 51.91f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(41.35f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(67.95f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-40.54f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.93f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-67.09f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();

		// === DECORACION ===
		// Librería Aire libre
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-60.0f, 0.0f, -25.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M25.RenderModel();
		// Pinos (M26)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, 70.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-175.0f, 0.0f, -105.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-170.0f, 0.0f, -10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-35.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(165.0f, 0.0f, 45.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 0.0f, 35.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Coches 2 (M31)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 55.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		// Cerezos (M32)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-60.0f, 0.0f, 45.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(155.0f, 0.0f, -20.0f));
		model = glm::rotate(model, -75 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-90.0f, 0.0f, 20.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 0.0f, 75.0f));
		model = glm::rotate(model, 105 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-80.0f, 0.0f, -75.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 105 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Señalizacion (M33_1)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(110.0f, 0.0f, 15.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(130.0f, 0.0f, 15.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -16.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-110.0f, 0.0f, -16.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_1.RenderModel();
		// Señalizacion (M33_2)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-140.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_2.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_2.RenderModel();
		// Carteles
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, 0.0f, -75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M34.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 25.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M34.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M34.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(27.0f, 0.0f, -83.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M34.RenderModel();

		// === RUNATERRA NPC's ===
		// Ziggs (LOL_00)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-111.0, 0.0f, -34.19f));
		model = glm::rotate(model, 137 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_00.RenderModel();
		// Poros (LOL_01)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-89.01f, 0.96f, -41.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-49.56f, 0.91f, -33.35f));
		model = glm::rotate(model, 13 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(85.9f, 2.37f, -18.68f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poros Ahri -------------
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-135.0f, 0.0f, -49.0f));
		model = glm::rotate(model, -25 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-132.0f, 0.0f, -50.0f));
		model = glm::rotate(model, -15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-134.0f, 0.0f, -52.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-132.0f, 0.0f, -54.0f));
		model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-135.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 25 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Anivia (LOL_02)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-26.0f, 22.0f, -42.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_02.RenderModel();
		// Pinguim (LOL_04)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-90.0f, 0.0f, -76.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-7.73f, 0.0f, -82.43f));
		model = glm::rotate(model, -211 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-142.85f, 0.0f, -100.48f));
		model = glm::rotate(model, -33 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -45.0f));
		model = glm::rotate(model, 18 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -50.0f));
		model = glm::rotate(model, -13 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		// Escurridizo (LOL_05)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-120.0f, 0.0f, -29.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_05.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-92.0f, 0.0f, -112.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_05.RenderModel();
		// Xayah (LOL_11)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-65.0f, 0.0f, -75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_11.RenderModel();
		// Annie (LOL_13)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-70.0f, 0.0f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_13.RenderModel();
		// Tristana (LOL_14)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-125.0f, 0.0f, -75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_14.RenderModel();
		// Montura Sejuani (LOL_15)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-125.0f, 0.0f, -90.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_15.RenderModel();

		// === DECORACION 2 RUNATERRA ===
		// Torreta (LOL_03)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-138.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_03.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-100.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_03.RenderModel();
		// Ward (LOL_06)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-89.0f, 0.0f, -47.0f));
		model = glm::rotate(model, 61 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_06.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 0.0f, -19.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_06.RenderModel();
		// Maestría (LOL_07)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-104.42f, 9.65f, -34.16f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-107.71f, 9.65f, -30.91f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-104.42f, 9.65f, -27.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-101.35f, 9.65f, -30.91f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		// Martillo Jayce (LOL_08)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-114.0f, 0.0f, -25.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_08.RenderModel();
		// Estatua Ahri (LOL_09)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 1.15f, -52.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_09.RenderModel();
		// Base Ahri (LOL_10)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 0.0f, -52.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_10.RenderModel();
		// Mazo Poppy (LOL_16)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-114.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_16.RenderModel();

		// === LUMINARIAS ===
		// Luminarias Hextech (LOL_12 - Jerarquía de arreglo)
		for (int i = 0; i < 8; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posicionesPostes[i]); // Usa la posición del arreglo
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			LOL_12.RenderModel();
		}



		// >>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - JUAN PABLO >>>>>>>>>>>>>>>>>>>>>>>
		
		// === INSTANCIA HW - CUERPO (EL PADRE) ===
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(cuerpoPosX, 1.0f, cuerpoPosZ));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, cuerpoRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelCuerpo = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hw_cuerpo.RenderModel();

		// === INSTANCIA HW - CABEZA (HIJO DEL CUERPO) ===
		model = modelCuerpo; // Partimos de la base del cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_cabeza.RenderModel();

		// === INSTANCIA HW - ESPADA (HIJO DEL CUERPO) ===
		model = modelCuerpo; // Empezamos desde la posición del cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -0.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_espada.RenderModel();

		// === PIERNA DERECHA (Avanza 15°) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.18f, -0.6f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Tu rotación base
		model = glm::rotate(model, rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // +35°
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_PiernaDerecha.RenderModel();

		// === PIERNA IZQUIERDA (Retrocede -15°) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-0.18f, -0.6f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // -35°
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_PiernaIzquierda.RenderModel();

		// === BRAZO DERECHO (Retrocede con pierna derecha) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.37f, 0.2f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // -35°
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_BrazoDerecho.RenderModel();

		// === BRAZO IZQUIERDO (Avanza con pierna derecha) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-0.37f, 0.2f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // +35°
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_BrazoIzquierdo.RenderModel();

		// === INSTANCIAS DE HW ===
		// Banca
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(120.0f, 0.0f, -25.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_banca.RenderModel();




		// >>>>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - TONY >>>>>>>>>>>>>>>>>>>>>>>>>>>
		// >>>>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - RUBEN >>>>>>>>>>>>>>>>>>>>>>>>>>



		// ====================================================================================
		// FASE 4: ELEMENTOS CON TRANSPARENCIA (BLENDING) Y TEXTURAS ANIMADAS
		// ====================================================================================
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
		// === HUMO DE LOCOMOTORAS===
		glm::vec3 colorHumo = glm::vec3(2.0f, 2.0f, 2.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorHumo));
		texturaHumo.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		// 1. HUMO TREN CHICO
		model = matrizHumoChico;
		model = glm::translate(model, glm::vec3(15.75f, 16.0f + offsetYHumo, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHumoAnimada, escalaHumoAnimada, escalaHumoAnimada));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMesh();
		// 2. HUMO TREN LARGO
		model = matrizHumoLargo;
		model = glm::translate(model, glm::vec3(15.75f, 16.0f + offsetYHumo, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHumoAnimada, escalaHumoAnimada, escalaHumoAnimada));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMesh();
		// LIMPIEZA
		glm::vec3 colorOriginal = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorOriginal));


		// === POROS CORAZONES ===
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-134.0f, 7.0f + offsetYHumo, -52.0f));
		model = glm::scale(model, glm::vec3(escalaHumoAnimada, escalaHumoAnimada, escalaHumoAnimada));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		texturaPoro.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		

		glDisable(GL_BLEND);
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}