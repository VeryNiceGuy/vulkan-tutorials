#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

BMPImage* loadBMP(const char* filename) {
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

    BMPInfoHeader infoHeader;
    fread(&infoHeader, sizeof(BMPInfoHeader), 1, file);
    fseek(file, fileHeader.bfOffBits, SEEK_SET);

    uint8_t* data = malloc(infoHeader.biSizeImage);
    fread(data, infoHeader.biSizeImage, 1, file);
    fclose(file);

    BMPImage* image = (BMPImage*)malloc(sizeof(BMPImage));
    image->width = infoHeader.biWidth;
    image->height = infoHeader.biHeight;
    image->data = data;

    return image;
}

void freeBMP(BMPImage* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}
