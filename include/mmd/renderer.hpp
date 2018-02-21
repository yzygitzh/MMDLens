#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <mmd/parser.hpp>

struct PMXRendererVertex {
    PMXFloat3XYZ pos;
    PMXFloat3XYZ norm;
    PMXFloat2UV UV;
};

class PMXRenderer {
    char *progPath;

    // model data
    PMXModel &model;
    size_t renderVertexNum;
    std::unique_ptr<PMXRendererVertex> renderVertices;

    // OpenGL data
    const char *vsName = "vs.glsl", *fsName = "fs.glsl";
    std::string vsPath, fsPath;
    GLuint vao, vertexBuffer, vs, fs, program;
    std::unique_ptr<GLuint> textures;

    // variables in shaders
    GLint mvpLocation, posLocation, normLocation, UVLocation;

    void loadShaders();
public:
    PMXRenderer(PMXModel &model_, char *progPath_);
    void render(GLFWwindow *window);
};

#endif
