#include <cstdio>
#include <string>
#include "model_parser.h"

using namespace std;

PMXModel::PMXModel(string fileName): fileName(move(fileName)) {
    printf("MMD file: %s\n", this->fileName.c_str());
}
