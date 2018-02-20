#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <mmd/parser.hpp>

class PMXRenderer {
    char *progPath;

    // model data
    PMXModel &model;
    size_t renderVertexNum;
    std::unique_ptr<PMXFloat3XYZ> renderVertices;

    // OpenGL data
    const char *vsName = "vs.glsl", *fsName = "fs.glsl";
    std::string vsPath, fsPath;
    GLuint vertexBuffer, vs, fs, program;

    // variables in shaders
    GLint mvp_location, vpos_location;

    void loadShaders();
public:
    PMXRenderer(PMXModel &model_, char *progPath_);
    void render(GLFWwindow *window);
};

#endif
