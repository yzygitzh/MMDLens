#include <codecvt>
#include <iostream>
#include <locale>
#include <boost/filesystem.hpp>

#include <mmd/parser.hpp>

namespace fsys = boost::filesystem;

void PMXModel::readFile() {
    std::fstream PMXFile(filePath, std::ios::in | std::ios::binary | std::ios::ate);
    if (PMXFile.is_open()) {
        fileSize = PMXFile.tellg();
#ifdef MODEL_PARSER_DEBUG
        std::cout << filePath << std::endl;
        std::cout << "PMX file size: " << fileSize << std::endl;
#endif
        memBlock = std::unique_ptr<char>(new char[fileSize]);
        PMXFile.seekg(0, std::ios::beg);
        PMXFile.read(memBlock.get(), fileSize);
        PMXFile.close();
    } else {
        throw std::runtime_error("Unable to open PMX file");
    }
    return;
}

PMXTextBuf PMXModel::readTextBuf(char *buf) {
    PMXTextBuf result;
    int textLen = *(int *)buf;
    result.originTextLen = textLen;
    if (globals.encoding == TEXT_ENCODING_UTF8) {
        // encoding=0, UTF8
        result.text = std::string(buf + sizeof(textLen), textLen);
    } else if (globals.encoding == TEXT_ENCODING_UTF16) {
        // convert UTF16 to UTF8
        std::u16string u16Str((char16_t *)(buf + sizeof(textLen)), textLen / sizeof(char16_t));
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
        result.text = cvt.to_bytes(u16Str);
    } else {
        throw std::runtime_error("Invalid text encoding");
    }
    return result;
}

#ifdef MODEL_PARSER_DEBUG
void PMXModel::printVertex(PMXVertex vertex) {
    std::cout << "pos: " << vertex.pos.x << " " << vertex.pos.y << " " << vertex.pos.z << std::endl;
    std::cout << "norm: " << vertex.norm.x << " " << vertex.norm.y << " " << vertex.norm.z << std::endl;
    std::cout << "UV: " << vertex.UV.u << " " << vertex.UV.v << std::endl;
    std::cout << "boneDeformMethod: " << (unsigned)vertex.boneDeformMethod << std::endl;
    if (vertex.boneDeformMethod == DEFORM_METHOD_SDEF) {
        std::cout << "bone1: " << vertex.SDEF.boneIdx[0] << std::endl;
        std::cout << "bone2: " << vertex.SDEF.boneIdx[1] << std::endl;
        std::cout << "weight: " << vertex.SDEF.weight << std::endl;
        std::cout << "C_x: " << vertex.SDEF.C.x
                  << " C_y: " << vertex.SDEF.C.y
                  << " C_z: " << vertex.SDEF.C.z << std::endl;
        std::cout << "R0_x: " << vertex.SDEF.R0.x
                  << " R0_y: " << vertex.SDEF.R0.y
                  << " R0_z: " << vertex.SDEF.R0.z << std::endl;
        std::cout << "R1_x: " << vertex.SDEF.R1.x
                  << " R1_y: " << vertex.SDEF.R1.y
                  << " R1_z: " << vertex.SDEF.R1.z << std::endl;
    }
    std::cout << "edgeSizeX: " << vertex.edgeSizeX << std::endl;
}
#endif

size_t PMXModel::readIdx(char *buf, size_t idxSize) {
    switch (idxSize) {
        case 1: return (size_t)*(unsigned char*)buf;
        case 2: return (size_t)*(unsigned short*)buf;
        case 4: return (size_t)*(unsigned *)buf;
        default:
            throw std::runtime_error("Invalid index size");
    }
}

void PMXModel::readVertices(char *buf) {
    size_t bufIdx = 0;
    vertexNum = *(int *)(buf);
    bufIdx += sizeof(vertexNum);
    vertices.resize(vertexNum);
    for (auto i = 0; i < vertexNum; i++) {
        PMXVertex currVertex;

        currVertex.pos = *(PMXFloat3XYZ *)(buf + bufIdx);
        bufIdx += sizeof(currVertex.pos);
        currVertex.norm = *(PMXFloat3XYZ *)(buf + bufIdx);
        bufIdx += sizeof(currVertex.norm);
        currVertex.UV = *(PMXFloat2UV *)(buf + bufIdx);
        bufIdx += sizeof(currVertex.UV);

        for (auto j = 0; j < globals.additionalUVNum; j++) {
            currVertex.additionalUV[j] = *(PMXFloat4XYZW *)(buf + bufIdx);
            bufIdx += sizeof(currVertex.additionalUV[j]);
        }

        currVertex.boneDeformMethod = *(unsigned char *)(buf + bufIdx);
        bufIdx += sizeof(currVertex.boneDeformMethod);

        switch (currVertex.boneDeformMethod) {
            case DEFORM_METHOD_BDEF1:
                currVertex.BDEF1.boneIdx = readIdx(buf + bufIdx, globals.boneIdxSize);
                bufIdx += globals.boneIdxSize;
                break;
            case DEFORM_METHOD_BDEF2:
                for (auto j = 0; j < 2; j++) {
                    currVertex.BDEF2.boneIdx[j] = readIdx(buf + bufIdx, globals.boneIdxSize);
                    bufIdx += globals.boneIdxSize;
                }
                currVertex.BDEF2.weight = *(float *)(buf + bufIdx);
                bufIdx += sizeof(currVertex.BDEF2.weight);
                break;
            case DEFORM_METHOD_BDEF4:
                for (auto j = 0; j < 4; j++) {
                    currVertex.BDEF4.boneIdx[j] = readIdx(buf + bufIdx, globals.boneIdxSize);
                    bufIdx += globals.boneIdxSize;
                }
                for (auto j = 0; j < 4; j++) {
                    currVertex.BDEF4.weight[j] = *(float *)(buf + bufIdx);
                    bufIdx += sizeof(currVertex.BDEF4.weight[j]);
                }
                break;
            case DEFORM_METHOD_SDEF:
                for (auto j = 0; j < 2; j++) {
                    currVertex.SDEF.boneIdx[j] = readIdx(buf + bufIdx, globals.boneIdxSize);
                    bufIdx += globals.boneIdxSize;
                }
                currVertex.SDEF.weight = *(float *)(buf + bufIdx);
                bufIdx += sizeof(currVertex.SDEF.weight);
                currVertex.SDEF.C = *(PMXFloat3XYZ *)(buf + bufIdx);
                bufIdx += sizeof(currVertex.SDEF.C);
                currVertex.SDEF.R0 = *(PMXFloat3XYZ *)(buf + bufIdx);
                bufIdx += sizeof(currVertex.SDEF.R0);
                currVertex.SDEF.R1 = *(PMXFloat3XYZ *)(buf + bufIdx);
                bufIdx += sizeof(currVertex.SDEF.R1);
                break;
            default:
                throw std::runtime_error("Invalid deform method");
        }

        currVertex.edgeSizeX = *(float *)(buf + bufIdx);
        bufIdx += sizeof(currVertex.edgeSizeX);

        vertices[i] = currVertex;
    }
    vertexRegionSize = bufIdx;
}

void PMXModel::readSurfaces(char *buf) {
    size_t bufIdx = 0;
    surfaceNum = *(int *)(buf) / 3;
    bufIdx += sizeof(surfaceNum);
    surfaces.resize(surfaceNum);
    for (auto i = 0; i < surfaceNum; i++) {
        PMXSurface currSurface;
        for (auto j = 0; j < 3; j++) {
            currSurface.vertexIdx[j] = readIdx(buf + bufIdx, globals.vertexIdxSize);
            bufIdx += globals.vertexIdxSize;
        }
        surfaces[i] = currSurface;
    }
    surfaceRegionSize = bufIdx;
}

void PMXModel::readTextures(char *buf) {
    size_t bufIdx = 0;
    textureNum = *(int *)(buf);
    bufIdx += sizeof(textureNum);
    textures.resize(textureNum);
    for (auto i = 0; i < textureNum; i++) {
        PMXTexture currTexture;
        PMXTextBuf name = readTextBuf(buf + bufIdx);
        bufIdx += sizeof(name.originTextLen) + name.originTextLen;
        textures[i].name = name;
        // read in image file, assume relative path
        std::string path = fsys::path(filePath).remove_filename().append(name.text).string();
        std::cout << path << std::endl;
        textures[i].image.load(path.c_str());
        textures[i].image.convertToRGBAF();
    }
    textureRegionSize = bufIdx;
}

void PMXModel::parseFile() {
    size_t bufIdx = 0;
    /*
     * PMX header
     * - PMX magic
     * - PMX version
     * - PMX globals, including text encoding & uv & data field byte size
     */

    // check PMX magic
    if (*(int *)(memBlock.get() + bufIdx) != MAGIC) {
        throw std::runtime_error("Invalid PMX magic");
    } else {
        bufIdx += sizeof(MAGIC);
    }
    // check PMX version
    ver = *(float *)(memBlock.get() + bufIdx);
    if (ver != 2.0 && ver != 2.1) {
        throw std::runtime_error("Unsupported PMX version");
    } else {
        bufIdx += sizeof(ver);
    }
    // PMX globals, number should be fixed at 8
    unsigned char globalNum = *(unsigned char *)(memBlock.get() + bufIdx);
    bufIdx += sizeof(globalNum);
    globals = *(PMXGlobalStruct *)(memBlock.get() + bufIdx);
    bufIdx += sizeof(globals);

    /*
     * PMX model info
     * - model name (??, en)
     * - model comment (??, en)
     * Store text encoded as UTF8.
     */
    PMXTextBuf modelNameBuf = readTextBuf(memBlock.get() + bufIdx);
    modelName = modelNameBuf.text;
    bufIdx += sizeof(modelNameBuf.originTextLen) + modelNameBuf.originTextLen;

    PMXTextBuf modelNameEnBuf = readTextBuf(memBlock.get() + bufIdx);
    modelNameEn = modelNameEnBuf.text;
    bufIdx += sizeof(modelNameEnBuf.originTextLen) + modelNameEnBuf.originTextLen;

    PMXTextBuf modelCommentBuf = readTextBuf(memBlock.get() + bufIdx);
    modelComment = modelCommentBuf.text;
    bufIdx += sizeof(modelCommentBuf.originTextLen) + modelCommentBuf.originTextLen;

    PMXTextBuf modelCommentEnBuf = readTextBuf(memBlock.get() + bufIdx);
    modelCommentEn = modelCommentEnBuf.text;
    bufIdx += sizeof(modelCommentEnBuf.originTextLen) + modelCommentEnBuf.originTextLen;

    /*
     * PMX vertices
     */
    readVertices(memBlock.get() + bufIdx);
    bufIdx += vertexRegionSize;

    /*
     * PMX surfaces
     */
    readSurfaces(memBlock.get() + bufIdx);
    bufIdx += surfaceRegionSize;

    /*
     * PMX textures
     */
    readTextures(memBlock.get() + bufIdx);
    bufIdx += textureRegionSize;

#ifdef MODEL_PARSER_DEBUG
        std::cout << "PMX version: " << ver << std::endl;
        std::cout << "PMX encoding: " << (unsigned)globals.encoding << std::endl;
        std::cout << "PMX additionalUVNum: " << (unsigned)globals.additionalUVNum << std::endl;
        std::cout << "PMX vertexIdxSize: " << (unsigned)globals.vertexIdxSize << std::endl;
        std::cout << "PMX textureIdxSize: " << (unsigned)globals.textureIdxSize << std::endl;
        std::cout << "PMX boneIdxSize: " << (unsigned)globals.boneIdxSize << std::endl;
        std::cout << "PMX morphIdxSize: " << (unsigned)globals.morphIdxSize << std::endl;
        std::cout << "PMX rigidIdxSize: " << (unsigned)globals.rigidIdxSize << std::endl;
        std::cout << "PMX modelName:" << std::endl << modelName << std::endl;
        std::cout << "PMX modelNameEn:" << std::endl << modelNameEn << std::endl;
        std::cout << "PMX modelComment:" << std::endl << modelComment << std::endl;
        std::cout << "PMX modelCommentEn:" << std::endl << modelCommentEn << std::endl;
        std::cout << "PMX vertexNum: " << vertexNum << std::endl;
        std::cout << "PMX vertexRegionSize: " << vertexRegionSize << std::endl;
        /*std::cout << "PMX vertex #1: " << std::endl;
        printVertex(vertices[0]);
        std::cout << "PMX vertex #2: " << std::endl;
        printVertex(vertices[1]);*/
        std::cout << "PMX surfaceNum: " << surfaceNum << std::endl;
        std::cout << "PMX surfaceRegionSize: " << surfaceRegionSize << std::endl;
        std::cout << "PMX textureNum: " << textureNum << std::endl;
        std::cout << "PMX textureRegionSize: " << textureRegionSize << std::endl;
        for (auto i = 0; i < textureNum; i++)
            std::cout << textures[i].name.text << std::endl;
#endif
}

PMXModel::PMXModel(std::string filePath_): filePath(filePath_) {
    // read PMX file content into memory
    readFile();
    // load PMX contents into class fields
    parseFile();
}

std::vector<PMXVertex>& PMXModel::getVertices() {
    return vertices;
}

std::vector<PMXSurface>& PMXModel::getSurfaces() {
    return surfaces;
}
