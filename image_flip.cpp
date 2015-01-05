#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstdlib>
#include <cassert>

using namespace std;


// Class for the chosen bmp image
class BMPImage
{
    public:
        BMPImage(const char *name);
        long get_width() const;
        long get_height() const;
        void swap_pixel(int x, int y, int w);
        
        string name;
        FILE *f;
        long offset;
        long width;
        long height;
        long row_length;
        unsigned char *header;
};

// Class constructor
BMPImage::BMPImage(const char *pName) : name(pName)
{
    /* By moving all of the calculations that need only be done once
       from member functions to the ctor, the functions can be called
       multiple times without wasting memory or calc time.
       */

    f = fopen(pName, "rb+"); // pName is used as a pointer for fopen 
    header = (unsigned char *) malloc(54);
    fread(header, sizeof(unsigned char), 54, f);

    // this method works for big and little endian storage
    // This just puts the bytes together with an offset of 8 bits
    // each time.
    width = (header[21] << 24)
                   + (header[20] << 16)
                   + (header[19] << 8)
                   + header[18];

    height = (header[25] << 24)
                   + (header[24] << 16)
                   + (header[23] << 8)
                   + header[22];

    // Get the start of the pixel data from the header
    offset = (header[13] << 24)
                + (header[12] << 16)
                + (header[11] << 8)
                + header[10];

    // free the malloc for the header
    free(header);

    // & is the bitwise and operator and ~ is the bitwise not
    // so 1100 & 0101 = 0100 and ~3 = ~0011 = 1100
    row_length = (width*3 + 3) & (~3);
}


// Function to return the width of the image from the header
long BMPImage::get_width() const
{
    return width;
}

// Similarly for image height
long BMPImage::get_height() const
{
    return height;
}

// Take coordinates for a pixel and swap its rgb values with
// those reflected in the vertical bisector of the row
void BMPImage::swap_pixel(int x, int y, int w)
{
    // declare variable to hold rgb values
    unsigned char pix1[3];
    // seek to the start of the pixel data
    fseek(f, offset, SEEK_SET);
    // seek to the needed pixel
    fseek(f, row_length*y + 3*x, SEEK_CUR);
    // read the rgb values
    fread(pix1, sizeof(unsigned char), 3, f);

    unsigned char pix2[3];
    fseek(f, offset, SEEK_SET);
    fseek(f, row_length*y + 3*(w - x - 1), SEEK_CUR);
    fread(pix2, sizeof(unsigned char), 3, f);

    fseek(f, offset, SEEK_SET);
    fseek(f, row_length*y + 3*x, SEEK_CUR);
    // write the rgb values from the second pixel to the first
    fwrite(pix2, sizeof(unsigned char), 3, f);

    fseek(f, offset, SEEK_SET);
    fseek(f, row_length*y + 3*(w - x - 1), SEEK_CUR);
    // write the rgb values from the first pixel to the second
    fwrite(pix1, sizeof(unsigned char), 3, f);
}



// Main Method
int main(int argc, char *argv[])
{
    // Create a class for our chosen image and read the header
    BMPImage pic(argv[1]);

    long w = pic.get_width();
    long h = pic.get_height();

    // loop over the pixels by row and column to swap every pixel
    int i, j;
    for(i=0; i < h; i++) {
        for(j=0; j < w/2; j++) {
            pic.swap_pixel(j, i, w);
        }
    }

    // free the memory allocated to the open file
    fclose(pic.f);
}
