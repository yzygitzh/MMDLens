#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <linmath.h>

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
    mat4x4 transMatrix, mvMatrix, projMatrix;
    GLint transLocation, mvLocation, projLocation;
    GLint posLocation, normLocation, UVLocation;

    void loadShaders();
public:
    PMXRenderer(PMXModel &model_, char *progPath_);
    void render(GLFWwindow *window);
    void setTrans(mat4x4 newTrans);
    void setMV(mat4x4 newMV);
    void setProj(mat4x4 newProj);
};

#endif
