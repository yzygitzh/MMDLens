#define _USE_MATH_DEFINES

#include <cmath>
#include <linmath.h>
#include <iostream>

#include <mmd/controller.hpp>

double Controller::viewTheta = M_PI_2;
double Controller::viewPhi = -M_PI_2;
double Controller::viewDis = Controller::DEFAULT_DISTANCE;
float Controller::centerX = 0.0f;
float Controller::centerY = Controller::DEFAULT_CENTER_Y;
float Controller::centerZ = 0.0f;

int Controller::windowWidth = Controller::DEFAULT_WINDOW_WIDTH;
int Controller::windowHeight = Controller::DEFAULT_WINDOW_HEIGHT;
double Controller::xPos = -1.0f;
double Controller::yPos = -1.0f;
bool Controller::mouseAngle = false;
bool Controller::mouseOffset = false;

mat4x4 Controller::transMatrix, Controller::mvMatrix, Controller::projMatrix;

void Controller::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        viewTheta = M_PI_2;
        viewPhi = -M_PI_2;
        viewDis = Controller::DEFAULT_DISTANCE;
        centerX = 0.0f;
        centerY = -10.0f;
    }
}

void Controller::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseAngle = true;
        } else if (action == GLFW_RELEASE) {
            mouseAngle = false;
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT ||
               button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            mouseOffset = true;
        } else if (action == GLFW_RELEASE) {
            mouseOffset = false;
        }
    }
}

void Controller::cursorPosCallback(GLFWwindow* window, double xPos_, double yPos_) {
    if (mouseAngle || mouseOffset) {
        if (xPos == -1.0f) {
            xPos = xPos_;
            yPos = yPos_;
        }
    } else {
        xPos = yPos = -1.0f;
    }

    if (mouseAngle) {
        viewTheta -= M_PI / 1000.0 * (xPos_ - xPos);
        if (viewPhi <= M_PI_2 && viewPhi + 0.01 * (yPos_ - yPos) > M_PI_2)
            viewPhi = M_PI_2;
        else if (viewPhi >= -M_PI_2 && viewPhi + 0.01 * (yPos_ - yPos) < -M_PI_2)
            viewPhi = -M_PI_2;
        else
            viewPhi += 0.01 * (yPos_ - yPos);
    } else if (mouseOffset) {
        centerX -= 0.1 * (xPos_ - xPos);
		centerY -= 0.1 * (yPos_ - yPos);
    }

    if (mouseAngle || mouseOffset) {
        xPos = xPos_;
	    yPos = yPos_;
    }

	return;
}

void Controller::scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    viewDis -= yOffset;
}

void Controller::windowSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void Controller::updateCamera() {
	float cameraX = std::cos(viewTheta) * std::cos(viewPhi) * viewDis;
	float cameraY = std::sin(viewTheta) * std::cos(viewPhi) * viewDis;
	float cameraZ = std::sin(viewPhi) * viewDis;

    vec3 eye = {cameraX, cameraY, cameraZ};
    vec3 center = {0.0f, 0.0f, 0.0f};
    vec3 up = {0.0f, 0.0f, 1.0f};

    mat4x4_translate(transMatrix, centerX, centerY, centerZ);
    mat4x4_look_at(mvMatrix, eye, center, up);
    mat4x4_perspective(projMatrix, M_PI / 4, float(windowWidth) / windowHeight, 0.1f, 1000.0f);
}
