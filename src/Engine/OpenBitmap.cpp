#include "Engine.h"
#include PLATFORM_HEADER

namespace MGE {

/*
Takes the location of a (.bmp) file loaded into memory and creates a re-formatted copy at the output location.

Parameter input_size according to whoever alocated the buffer, not the file format.

Returns sucess boolean
*/
bool DecodeBitmap(byte* input, int input_size, Bitmap* output, int output_size)
{
    // Byte offsets according to the specification of the BMP file format.
    short signature = *(short*)(input + 0x00); // expecting "BM"
    if (signature != ('M' << 8) + 'B') {return false;}
    if (input_size < 0x1E + 4) {return false;} // Size of header
    
    int file_size = *(int*)(input + 0x02);    
    int data_offset = *(int*)(input + 0x0A);
    
    int bitmap_header_size = *(int*)(input + 0x0E);
    //Print("header size = %d\n", bitmap_header_size);    
    
    int width = *(int*)(input + 0x12);
    int height = *(int*)(input + 0x16);
    short bytes_per_pixel = *(short*)(input + 0x1C) / BITSPERBYTE;
    //Print("bytes_per_pixel = %d\n", (int64)bytes_per_pixel);
    int compression_type = *(int*)(input + 0x1E);
    //Print("compression_type = %d\n", (int64)compression_type);
    
    byte* pixels = input + data_offset;
    int num_pixels = width * height;
    int data_size = num_pixels * bytes_per_pixel;

    if (compression_type != 0 && compression_type != 3) {return false;}
    if (file_size - data_offset != data_size) {return false;}
    if (output_size < data_size + sizeof(Bitmap)) {return false;}

    int stride = width * bytes_per_pixel;
    // Stride will round up to a multiple of 4. We can do that with a two's complement gimmick
    stride += (0 - stride & 0b11) & 0b11;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int* from = (int*)(input + data_offset + (x * bytes_per_pixel) + ((height - y) * stride));
            int* to = (int*)output->Pixels + x + (y * width);
            
            int pixel = *from;
            
            int red = (pixel >> 16) & 0xFF;
            int blue = pixel & 0xFF;
            pixel = pixel & 0xFF00FF00;
            pixel = pixel | (blue << 16) | red;
            
            if (bytes_per_pixel == 3)
            {
                // If image did not use an alpha channel we will assume opacity
                pixel = pixel | 0xFF000000;
            }
            
            *to = pixel;
        }
    }
    
    output->Width = width;
    output->Height = height;
    return true;
}

/*
param mem used as working memory only
Returns success boolean
*/
bool Engine::OpenBitmap(byte* mem, int mem_size, Bitmap* destination, int dest_size, char* path)
{
    uint32 bytes_read = PLATFORM::ReadEntireFile(mem, 10 * MEGABYTES, path);
    if (bytes_read == 0)
    {
        return false;
    }

    bool success = DecodeBitmap(mem, 10 * MEGABYTES, destination, 10 * MEGABYTES);

    return success;
}

}