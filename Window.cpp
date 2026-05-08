#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
	mouseFirstMoved = true;

	// Inicialización variables existentes
	RotLlantas = 0.0f;
	colorFaroCoche = 0;

	// Inicialización nuevas banderas
	accionF = false;
	accionG = false;
	accionH = false;
	accionJ = false;
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	mouseFirstMoved = true;

	// Inicialización variables existentes
	RotLlantas = 0.0f;
	colorFaroCoche = 0;

	// Inicialización nuevas banderas
	accionF = false;
	accionG = false;
	accionH = false;
	accionJ = false;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int Window::Initialise()
{
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Practica 9: Animacion Compleja", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);
	createCallbacks();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, bufferWidth, bufferHeight);
	glfwSetWindowUserPointer(mainWindow, this);

	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// Captura de banderas tipo "Trigger" (un solo pulso)
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_F) theWindow->accionF = true;
		if (key == GLFW_KEY_G) theWindow->accionG = true;
		if (key == GLFW_KEY_H) theWindow->accionH = true;
		if (key == GLFW_KEY_J) theWindow->accionJ = true;

		// Lógica existente del faro del coche
		if (key == GLFW_KEY_I) {
			theWindow->colorFaroCoche++;
			if (theWindow->colorFaroCoche > 5) theWindow->colorFaroCoche = 0;
		}
	}

	// Registro de estado general para teclas continuas
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) theWindow->keys[key] = true;
		else if (action == GLFW_RELEASE) theWindow->keys[key] = false;
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}