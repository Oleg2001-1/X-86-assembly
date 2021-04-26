//Part of the program was taken from laboratory exercises by Rajmund Kożuszek
//Author Aleh Iotchanka, 308908
//Barcode EAN8 generation . Output file to output.bmp
//In line 249 we enter the 8-digit decimal code for encrypt
//In line 252, 253 we enter size of output BMP

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#pragma pack(push, 1)
typedef struct
{
    uint16_t bfType;
    uint32_t  bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t  bfOffBits;
    uint32_t  biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    int16_t biPlanes;
    int16_t biBitCount;
    uint32_t  biCompression;
    uint32_t  biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t  biClrUsed;
    uint32_t  biClrImportant;
    uint32_t  RGBQuad_0;
    uint32_t  RGBQuad_1;
} bmpHdr;
#pragma pack(pop)

typedef struct
{
    int width, height;        // image width and height
    unsigned char* pImg;    // an indication of the beginning of pixel data
    int cX, cY;                // current coordinates
    int col;                // current color
} imgInfo;

void* freeResources(FILE* pFile, void* pFirst, void* pSnd)
{
    if (pFile != 0)
        fclose(pFile);
    if (pFirst != 0)
        free(pFirst);
    if (pSnd !=0)
        free(pSnd);
    return 0;
}

imgInfo* readBMP(const char* fname)
{
    imgInfo* pInfo = 0;
    FILE* fbmp = 0;
    bmpHdr bmpHead;
    int lineBytes, y;
    unsigned long imageSize = 0;
    unsigned char* ptr;

    pInfo = 0;
    fbmp = fopen(fname, "rb");
    if (fbmp == 0)
        return 0;

    fread((void *) &bmpHead, sizeof(bmpHead), 1, fbmp);
    // some basic checks
    if (bmpHead.bfType != 0x4D42 || bmpHead.biPlanes != 1 ||
        bmpHead.biBitCount != 1 || bmpHead.biClrUsed != 2 ||
        (pInfo = (imgInfo *) malloc(sizeof(imgInfo))) == 0)
        return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

    pInfo->width = bmpHead.biWidth;
    pInfo->height = bmpHead.biHeight;
    imageSize = (((pInfo->width + 31) >> 5) << 2) * pInfo->height;

    if ((pInfo->pImg = (unsigned char*) malloc(imageSize)) == 0)
        return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

    // process height (it can be negative)
    ptr = pInfo->pImg;
    lineBytes = ((pInfo->width + 31) >> 5) << 2; // line size in bytes
    if (pInfo->height > 0)
    {
        // "upside down", bottom of the image first
        ptr += lineBytes * (pInfo->height - 1);
        lineBytes = -lineBytes;
    }
    else
        pInfo->height = -pInfo->height;

    // reading image
    // moving to the proper position in the file
    if (fseek(fbmp, bmpHead.bfOffBits, SEEK_SET) != 0)
        return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

    for (y=0; y<pInfo->height; ++y)
    {
        fread(ptr, 1, abs(lineBytes), fbmp);
        ptr += lineBytes;
    }
    fclose(fbmp);
    return pInfo;
}

int save_file(const imgInfo* pInfo, const char* fname)
{
    int lineBytes = ((pInfo->width + 31) >> 5)<<2;
    bmpHdr bmpHead =
    {
    0x4D42,                // unsigned short bfType;
    sizeof(bmpHdr),        // unsigned long  bfSize;
    0, 0,                // unsigned short bfReserved1, bfReserved2;
    sizeof(bmpHdr),        // unsigned long  bfOffBits;
    40,                    // unsigned long  biSize;
    pInfo->width,        // long  biWidth;
    pInfo->height,        // long  biHeight;
    1,                    // short biPlanes;
    1,                    // short biBitCount;
    0,                    // unsigned long  biCompression;
    lineBytes * pInfo->height,    // unsigned long  biSizeImage;
    11811,                // long biXPelsPerMeter; = 300 dpi
    11811,                // long biYPelsPerMeter;
    2,                    // unsigned long  biClrUsed;
    0,                    // unsigned long  biClrImportant;
    0x00000000,            // unsigned long  RGBQuad_0;
    0x00FFFFFF            // unsigned long  RGBQuad_1;
    };

    FILE * fbmp;
    unsigned char *ptr;
    int y;

    if ((fbmp = fopen(fname, "wb")) == 0)
        return -1;
    if (fwrite(&bmpHead, sizeof(bmpHdr), 1, fbmp) != 1)
    {
        fclose(fbmp);
        return -2;
    }

    ptr = pInfo->pImg + lineBytes * (pInfo->height - 1);
    for (y=pInfo->height; y > 0; --y, ptr -= lineBytes)
        if (fwrite(ptr, sizeof(unsigned char), lineBytes, fbmp) != lineBytes)
        {
            fclose(fbmp);
            return -3;
        }
    fclose(fbmp);
    return 0;
}

imgInfo* InitImage (int w, int h)
{
    imgInfo *pImg;
    if ( (pImg = (imgInfo *) malloc(sizeof(imgInfo))) == 0)
        return 0;
    pImg->height = h;
    pImg->width = w;
    pImg->pImg = (unsigned char*) malloc((((w + 31) >> 5) << 2) * h);
    if (pImg->pImg == 0)
    {
        free(pImg);
        return 0;
    }
    memset(pImg->pImg, 0xFF, (((w + 31) >> 5) << 2) * h);
    pImg->cX = 0;
    pImg->cY = 0;
    pImg->col = 0;
    return pImg;
}

void FreeImage(imgInfo* pInfo)
{
    if (pInfo && pInfo->pImg)
        free(pInfo->pImg);
    if (pInfo)
        free(pInfo);
}

imgInfo* SetColor(imgInfo* pImg, int col)
{
    pImg->col = col != 0;
    return pImg;
}

imgInfo* MoveTo(imgInfo* pImg, int x, int y)
{
    if (x >= 0 && x < pImg->width)
        pImg->cX = x;
    if (y >= 0 && y < pImg->height)
        pImg->cY = y;
    return pImg;
}

extern void encryptASM(char decimal[], char binary[]);

void encrypt(char decimal[], char binary[]) {
    encryptASM(decimal, binary);
}

extern void drawASM(unsigned char *pImg, unsigned short width_mod, char decimal[], unsigned short ofs);

void drawC(imgInfo *pInfo, unsigned short size_of_row, unsigned short ofs) {     
    int i;
    unsigned char *first = pInfo->pImg + ofs * size_of_row;
    unsigned char *current = first;
    unsigned long temp_height = pInfo->height - ofs;
    while (pInfo->cY < temp_height)
    {
        current = pInfo->pImg + pInfo->cY * size_of_row;
        for (i=0; i<size_of_row; ++i) {
            *current = *first;
            current++;
            first++;
        }
        pInfo->cY += 1;
        first = pInfo->pImg + ofs * size_of_row;
    }
}

void draw(imgInfo *pInfo, char text[]) {
    unsigned short width_mod = pInfo->width / 67;
    unsigned long size_of_row = ((pInfo->width + 31) >> 5) << 2;
    unsigned short ofs = (pInfo->width - (width_mod * 67)) / 2;
    SetColor(pInfo, 0);
    drawASM(pInfo->pImg+size_of_row*ofs, width_mod, text, ofs);
    MoveTo(pInfo, 0, ofs+1);
    drawC(pInfo, size_of_row, ofs);
}

int char_convert(char number[]) {
    int i = 0; int result = 0;
    while(number[i] != 0) {
        result *= 10;
        result += (number[i] - '0');
        i++;
    }
    return result;
}

int main(int argc, char* argv[])
{
    imgInfo* pInfo;
    char* decimal = "05032001";     //Here we enter the 8-digit decimal code for encrypt
    char binary[] = "101AAAAAAAAAAAAAAAAAAAAAAAAAAAA01010AAAAAAAAAAAAAAAAAAAAAAAAAAAA101";   
    //Here we change the values ​​of the output file (minimal 67x1)
    unsigned short height = 1024; 
    unsigned short width = 1024;
    if (argc == 2) {
        decimal = argv[1];
    }
    if (argc == 3) {
        width = char_convert(argv[1]);
        height = char_convert(argv[2]);
    }
    if (argc == 4) {
        decimal = argv[1];
        width = char_convert(argv[2]);
        height = char_convert(argv[3]);
    }
    if ((pInfo = InitImage (width, height)) == 0)
        return 2;
    encrypt(decimal, binary);
    draw(pInfo, binary);
    save_file(pInfo, "output.bmp");
    FreeImage(pInfo);
    return 0;
}

