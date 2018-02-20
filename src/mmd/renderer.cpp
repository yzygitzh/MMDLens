#include <iostream>
#include <linmath.h>
#include <boost/filesystem.hpp>

#include <mmd/renderer.hpp>

namespace fsys = boost::filesystem;

void PMXRenderer::loadShaders() {
    std::unique_ptr<char> vsSource, fsSource;

    // vertex shader
    std::ifstream vsFile(vsPath);
    if (vsFile.is_open()) {
        std::string vsText((std::istreambuf_iterator<char>(vsFile)), std::istreambuf_iterator<char>());
        vsFile.close();
        vs = glCreateShader(GL_VERTEX_SHADER);
        const char *vsTextPtr = vsText.c_str();
        glShaderSource(vs, 1, &vsTextPtr, NULL);
        glCompileShader(vs);
    } else {
        throw std::runtime_error("Unable to open vertex shader");
    }

    // fragment shader
    std::ifstream fsFile(fsPath);
    if (fsFile.is_open()) {
        std::string fsText((std::istreambuf_iterator<char>(fsFile)), std::istreambuf_iterator<char>());
        fsFile.close();
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        const char *fsTextPtr = fsText.c_str();
        glShaderSource(fs, 1, &fsTextPtr, NULL);
        glCompileShader(fs);
    } else {
        throw std::runtime_error("Unable to open fragment shader");
    }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
}

PMXRenderer::PMXRenderer(PMXModel &model_, char *progPath_): model(model_), progPath(progPath_) {
    // process model data
    std::vector<PMXVertex>& modelVertices = model.getVertices();
    std::vector<PMXSurface>& modelSurfaces = model.getSurfaces();

    // prepare vertices for rendering
    renderVertexNum = modelSurfaces.size() * 3;
    renderVertices = std::unique_ptr<PMXFloat3XYZ>(new PMXFloat3XYZ[renderVertexNum]);
    for (auto i = 0; i < modelSurfaces.size(); i++) {
        for (auto j = 0; j < 3; j++)
            renderVertices.get()[3 * i + j] = modelVertices[modelSurfaces[i].vertexIdx[j]].pos;
    }
    // prepare vertex texture

    // process OpenGL data structure
    // prepare vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PMXFloat3XYZ) * renderVertexNum, renderVertices.get(), GL_STATIC_DRAW);
    // load shaders
    vsPath = fsys::path(progPath).remove_filename().append(vsName).string();
    fsPath = fsys::path(progPath).remove_filename().append(fsName).string();
    loadShaders();
    // prepare vertex attribute
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "pos");
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
}

void PMXRenderer::render(GLFWwindow *window) {
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    mat4x4_identity(m);
    //mat4x4_rotate_X(m, m, (float) glfwGetTime());
    mat4x4_rotate_Y(m, m, (float) glfwGetTime());
    //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
    mat4x4_ortho(p, -ratio * 20, ratio * 20, -10.f, 30.f, 20.f, -20.f);
    mat4x4_mul(mvp, p, m);
    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glDrawArrays(GL_TRIANGLES, 0, renderVertexNum);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
