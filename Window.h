#pragma once
#include <stdio.h>
#include <glew.h>
#include <glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return (GLfloat)bufferWidth; }
	GLfloat getBufferHeight() { return (GLfloat)bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();

	// --- VARIABLES EXISTENTES ---
	GLfloat getRotacionLlantas() { return RotLlantas; }
	int getColorFaroCoche() { return colorFaroCoche; }

	// --- NUEVAS BANDERAS BOOLEANAS (F, G, H, J) ---
	bool getAccionF() { return accionF; }
	void apagarAccionF() { accionF = false; }

	bool getAccionG() { return accionG; }
	void apagarAccionG() { accionG = false; }

	bool getAccionH() { return accionH; }
	void apagarAccionH() { accionH = false; }

	bool getAccionJ() { return accionJ; }
	void apagarAccionJ() { accionJ = false; }

	bool getShouldClose() { return  glfwWindowShouldClose(mainWindow); }
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;

	// Variables de animación existentes
	GLfloat RotLlantas;
	int colorFaroCoche;

	// Nuevas variables booleanas
	bool accionF;
	bool accionG;
	bool accionH;
	bool accionJ;

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};