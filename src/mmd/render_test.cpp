#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <boost/program_options.hpp>

#include <mmd/parser.hpp>
#include <mmd/renderer.hpp>
#include <mmd/controller.hpp>

namespace po = boost::program_options;

static void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

int show(std::string modelPath, char *progPath)
{
    PMXModel testModel = PMXModel(modelPath);

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(Controller::DEFAULT_WINDOW_WIDTH,
                              Controller::DEFAULT_WINDOW_HEIGHT,
                              "MMD Lens", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // setup callbacks
    glfwSetWindowSizeCallback(window, Controller::windowSizeCallback);
    glfwSetKeyCallback(window, Controller::keyCallback);
    glfwSetMouseButtonCallback(window, Controller::mouseButtonCallback);
    glfwSetScrollCallback(window, Controller::scrollCallback);
    glfwSetCursorPosCallback(window, Controller::cursorPosCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    PMXRenderer renderer(testModel, progPath);

    while (!glfwWindowShouldClose(window))
    {
        Controller::updateCamera();
        renderer.setTrans(Controller::transMatrix);
        renderer.setMV(Controller::mvMatrix);
        renderer.setProj(Controller::projMatrix);
        renderer.render(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    po::options_description desc("MMD Parser Testing Program");
    desc.add_options()
        ("input-model,i", po::value<std::string>(), "input mmd model")
        ("help", "show help")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
    } else if (vm.count("input-model")) {
        std::string modelPath = vm["input-model"].as<std::string>();
        show(modelPath, argv[0]);
    } else {
        std::cout << "model path was not set." << std::endl;
    }
    return 0;
}
