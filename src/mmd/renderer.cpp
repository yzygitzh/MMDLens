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
    std::vector<PMXTexture>& modelTextures = model.getTextures();

    // prepare vertices for rendering
    renderVertexNum = modelSurfaces.size() * 3;
    renderVertices = std::unique_ptr<PMXRendererVertex>(new PMXRendererVertex[renderVertexNum]);
    for (auto i = 0; i < modelSurfaces.size(); i++) {
        for (auto j = 0; j < 3; j++) {
            renderVertices.get()[3 * i + j].pos = modelVertices[modelSurfaces[i].vertexIdx[j]].pos;
            renderVertices.get()[3 * i + j].norm = modelVertices[modelSurfaces[i].vertexIdx[j]].norm;
            renderVertices.get()[3 * i + j].UV = modelVertices[modelSurfaces[i].vertexIdx[j]].UV;
        }
    }
    // process OpenGL data structure
    // prepare vertex array object
    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
    // prepare vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PMXRendererVertex) * renderVertexNum, renderVertices.get(), GL_STATIC_DRAW);
    // load shaders
    vsPath = fsys::path(progPath).remove_filename().append(vsName).string();
    fsPath = fsys::path(progPath).remove_filename().append(fsName).string();
    loadShaders();

    // prepare vertex uniforms
    mvpLocation = glGetUniformLocation(program, "MVP");

    // prepare vertex attributes
    posLocation = glGetAttribLocation(program, "pos");
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(PMXRendererVertex), (void*) 0);
    normLocation = glGetAttribLocation(program, "norm");
    glEnableVertexAttribArray(normLocation);
    glVertexAttribPointer(normLocation, 3, GL_FLOAT, GL_FALSE, sizeof(PMXRendererVertex), (void*) 12);
    UVLocation = glGetAttribLocation(program, "UV");
    glEnableVertexAttribArray(UVLocation);
    glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(PMXRendererVertex), (void*) 24);

    // prepare vertex texture
    textures = std::unique_ptr<GLuint>(new GLuint[modelTextures.size()]);
    glGenTextures(modelTextures.size(), textures.get());
    glActiveTexture(GL_TEXTURE0); // tex_color
    for (auto i = 0; i < modelTextures.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, textures.get()[i]);
        glTexStorage2D(GL_TEXTURE_2D,  // 2D texture
            1,                         // 8 mipmap levels
            GL_RGBA32F,                // 32-bit floating-point RGBA data
            modelTextures[i].image.getWidth(), modelTextures[i].image.getHeight());  // 256 x 256 texels
        glTexSubImage2D(GL_TEXTURE_2D, // 2D texture
            0,                         // Level 0
            0, 0,                      // Offset 0, 0
            modelTextures[i].image.getWidth(), modelTextures[i].image.getHeight(),   // 256 x 256 texels, replace entire image
            GL_RGBA,                   // Four channel data
            GL_FLOAT,                  // Floating point data%
            modelTextures[i].image.accessPixels());                                  // Pointer to data
    }
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
    mat4x4_rotate_Y(m, m, (float) glfwGetTime());
    mat4x4_ortho(p, -ratio *15, ratio * 15, -5.f, 25.f, 15.f, -15.f);
    mat4x4_mul(mvp, p, m);
    glUseProgram(program);
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*) mvp);

    glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::vector<PMXMaterial>& modelMaterials = model.getMaterials();
    int vertexOffset = 0;
    for (auto i = 0; i < modelMaterials.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, textures.get()[modelMaterials[i].textureIdx]);
        glDrawArrays(GL_TRIANGLES, vertexOffset, modelMaterials[i].surfaceNum * 3);
        vertexOffset += modelMaterials[i].surfaceNum * 3;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}
