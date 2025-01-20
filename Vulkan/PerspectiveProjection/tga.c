#include "tga.h"
#include <stdio.h>
#include <stdlib.h>

TGAImage* loadTGA(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return NULL;
    }

    TGAHeader header;
    fread(&header, sizeof(TGAHeader), 1, file);

    if (header.bitsPerPixel != 32) {
        fprintf(stderr, "Error: Only 32-bit TGA images are supported\n");
        fclose(file);
        return NULL;
    }

    uint32_t imageSize = header.width * header.height * 4;
    uint32_t* imageData = (uint32_t*)malloc(imageSize);

    fread(imageData, imageSize, 1, file);
    fclose(file);

    TGAImage* image = (TGAImage*)malloc(sizeof(TGAImage));
    image->width = header.width;
    image->height = header.height;
    image->data = imageData;

    return image;
}

void freeTGA(TGAImage* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}
