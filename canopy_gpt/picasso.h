#ifndef PICASSO_H
#define PICASSO_H

#include <stdlib.h>
#include <string.h>

// Define BMP file header structures
#pragma pack(push,1) //https://www.ibm.com/docs/no/zos/2.4.0?topic=descriptions-pragma-pack
typedef struct {
    uint16_t file_type;                 // File type always BM which is 0x4D42
    uint32_t file_size;                 // Size of the file (in bytes)
    uint16_t reserved1;                 // Reserved, always 0
    uint16_t reserved2;                 // Reserved, always 0
    uint32_t offset_data;               // Start position of pixel data (bytes from the beginning of the file)
} BMPFileHeader;

typedef struct {
    uint32_t size;                      // Size of this header (in bytes)
    int32_t width;                      // width of bitmap in pixels
    int32_t height;                     // width of bitmap in pixels
    uint16_t planes;                    // No. of planes for the target device, this is always 1
    uint16_t bit_count;                 // No. of bits per pixel
    uint32_t compression;               // 0 or 3 - uncompressed (BI_RGB)
    uint32_t size_image;                // 0 - for uncompressed images
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bitCount
    uint32_t colors_important;          // No. of colors used for displaying the bitmap. If 0 all colors are required
} BMPInfoHeader;

typedef struct {
        BMPFileHeader fh;
        BMPInfoHeader ih;
        uint8_t *image;
}BMP;
#pragma pack(pop)

uint8_t *load_bmp(const char *filename, BMP *bmp) {
       BMPFileHeader *fileHeader = &bmp->fh;
       BMPInfoHeader *infoHeader = &bmp->ih;

        FILE *file = fopen(filename, "rb");
        if (!file) {
                perror("Unable to open file");
                return NULL;
        }

        // Read file header
        fread(fileHeader, sizeof(BMPFileHeader), 1, file);
        if (fileHeader->file_type != 0x4D42) {
                printf("Not a valid BMP file\n");
                fclose(file);
                return NULL;
        }

        // Read info header
        fread(infoHeader, sizeof(BMPInfoHeader), 1, file);

        // Move file pointer to the beginning of bitmap data
        fseek(file, fileHeader->offset_data, SEEK_SET);

        // Allocate memory for the bitmap data
        uint8_t *data = (uint8_t *)malloc(infoHeader->size_image);
        if (!data) {
                printf("Memory allocation failed\n");
                fclose(file);
                return NULL;
        }

        // Read the bitmap data
        fread(data, infoHeader->size_image, 1, file);

        fclose(file);

        // Swap color channels since mac uses BGRA, not RGBA
        for (int i = 0; i < infoHeader->width * infoHeader->height; ++i)
        {
                uint8_t temp        = data[i * 4 + 0];  // Blue
                data[i * 4 + 0]     = data[i * 4 + 2];  // Swap Red and Blue
                data[i * 4 + 2]     = temp;             // Assign Red to the original Blue
        }
        return data;
}
void flip_buffer_vertical(uint8_t *buffer, int width, int height) {
    int bytes_per_pixel = 4; // RGBA
    int row_size = width * bytes_per_pixel;

    uint8_t temp_row[row_size]; // Temporary row buffer

    for (int y = 0; y < height / 2; y++) {
        int top_index = y * row_size;
        int bottom_index = (height - y - 1) * row_size;

        // Swap the rows
        memcpy(temp_row, &buffer[top_index], row_size);
        memcpy(&buffer[top_index], &buffer[bottom_index], row_size);
        memcpy(&buffer[bottom_index], temp_row, row_size);
    }
}

#endif // PICASSO_H
