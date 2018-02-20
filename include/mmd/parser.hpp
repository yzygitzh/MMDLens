#ifndef MODEL_PARSER_H
#define MODEL_PARSER_H

#include <string>
#include <exception>
#include <fstream>
#include <memory>
#include <vector>
#include <FreeImagePlus.h>

struct PMXTextBuf {
    std::string text;
    int originTextLen;
};

struct PMXFloat2XY {float x, y;};
struct PMXFloat2UV {float u, v;};
struct PMXFloat3XYZ {float x, y, z;};
struct PMXFloat3RGB {float r, g, b;};
struct PMXFloat4XYZW {float x, y, z, w;};
struct PMXFloat4RGBA {float r, g, b, a;};

struct PMXVertex {
    PMXFloat3XYZ pos;
    PMXFloat3XYZ norm;
    PMXFloat2UV UV;
    PMXFloat4XYZW additionalUV[4];
    unsigned char boneDeformMethod;
    struct {
        size_t boneIdx;
    } BDEF1;
    struct {
        size_t boneIdx[2];
        float weight;
    } BDEF2;
    struct {
        size_t boneIdx[4];
        float weight[4];
    } BDEF4;
    struct {
        size_t boneIdx[2];
        float weight;
        PMXFloat3XYZ C;
        PMXFloat3XYZ R0;
        PMXFloat3XYZ R1;
    } SDEF;
    float edgeSizeX;
};

struct PMXSurface {
    size_t vertexIdx[3];
};

struct PMXTexture {
    PMXTextBuf name;
    fipImage image;
};

class PMXModel {
    static const int MAGIC = 0x20584d50; // "PMX "
    static const int TEXT_ENCODING_UTF16 = 0;
    static const int TEXT_ENCODING_UTF8 = 1;
    static const int DEFORM_METHOD_BDEF1 = 0;
    static const int DEFORM_METHOD_BDEF2 = 1;
    static const int DEFORM_METHOD_BDEF4 = 2;
    static const int DEFORM_METHOD_SDEF = 3;

    std::string filePath;
    std::unique_ptr<char> memBlock;
    size_t fileSize;

    // PMX header
    float ver;
    struct PMXGlobalStruct {
        unsigned char encoding;
        unsigned char additionalUVNum;
        unsigned char vertexIdxSize;
        unsigned char textureIdxSize;
        unsigned char materialIdxSize;
        unsigned char boneIdxSize;
        unsigned char morphIdxSize;
        unsigned char rigidIdxSize;
    } globals;

    // model info
    std::string modelName;
    std::string modelNameEn;
    std::string modelComment;
    std::string modelCommentEn;

    // vertex
    int vertexNum;
    size_t vertexRegionSize;
    std::vector<PMXVertex> vertices;

    // surface
    int surfaceNum;
    size_t surfaceRegionSize;
    std::vector<PMXSurface> surfaces;

    // texture
    int textureNum;
    size_t textureRegionSize;
    std::vector<PMXTexture> textures;

    void readFile();
    void parseFile();
    size_t readIdx(char *buf, size_t idxSize);
    PMXTextBuf readTextBuf(char *buf);
    void readVertices(char *buf);
    void readSurfaces(char *buf);
    void readTextures(char *buf);

#ifdef MODEL_PARSER_DEBUG
    void printVertex(PMXVertex vertex);
#endif

public:
    PMXModel(std::string filePath);
    std::vector<PMXVertex>& getVertices();
    std::vector<PMXSurface>& getSurfaces();
};

#endif
