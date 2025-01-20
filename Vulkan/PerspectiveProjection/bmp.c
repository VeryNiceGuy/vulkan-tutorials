#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

unsigned char* loadBMP(const char* filename, BMPInfoHeader* infoHeader) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return NULL;
    }

    BMPFileHeader fileHeader;
    fread(&fileHeader, sizeof(BMPFileHeader), 1, file);

    if (fileHeader.bfType != 0x4D42) {
        fprintf(stderr, "Error: Not a valid BMP file\n");
        fclose(file);
        return NULL;
    }

    fread(infoHeader, sizeof(BMPInfoHeader), 1, file);
    fseek(file, fileHeader.bfOffBits, SEEK_SET);

    unsigned char* data = malloc(infoHeader->biSizeImage);
    fread(data, infoHeader->biSizeImage, 1, file);

    fclose(file);
    return data;
}
