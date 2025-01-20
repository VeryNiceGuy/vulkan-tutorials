#pragma once
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint8_t idLength;
    uint8_t colorMapType;
    uint8_t dataTypeCode;
    uint16_t colorMapOrigin;
    uint16_t colorMapLength;
    uint8_t colorMapDepth;
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t width;
    uint16_t height;
    uint8_t bitsPerPixel;
    uint8_t imageDescriptor;
} TGAHeader;
#pragma pack(pop)

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t* data;
} TGAImage;

TGAImage* loadTGA(const char* filename);
void freeTGA(TGAImage* image);