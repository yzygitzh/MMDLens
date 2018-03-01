#ifndef MODEL_CONTROLLER_H
#define MODEL_CONTROLLER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <linmath.h>

class Controller {
    static constexpr float DEFAULT_DISTANCE = 40.0f;
    static constexpr float DEFAULT_CENTER_Y = -10.0f;
    static double viewTheta, viewPhi, viewDis;
	static float centerX, centerY, centerZ;
    static int windowWidth, windowHeight;
    static double xPos, yPos;
    static bool mouseAngle, mouseOffset;
public:
    static const int DEFAULT_WINDOW_WIDTH = 640, DEFAULT_WINDOW_HEIGHT = 480;
    static mat4x4 transMatrix, mvMatrix, projMatrix;
    static void updateCamera();

    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
};

#endif
