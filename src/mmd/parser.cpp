#include <cstdio>
#include <string>

#include <mmd/parser.h>

using namespace std;

PMXModel::PMXModel(string fileName): fileName(move(fileName)) {
    printf("MMD file: %s\n", this->fileName.c_str());
}
