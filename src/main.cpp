/*
*   This program/source code is licensed under the MIT License:
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
*/

// information about how PSX saves are made was taken from the PS Devs wiki (under the GNU free docs license) <3
// https://www.psdevwiki.com/ps3/PS1_Savedata#Virtual_Memory_Card_PS1_.28.VM1.29

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

unsigned char buffer[8192]; // this is the bytedata that gets output to a file

void init_buffer() {
    // zeroes out the buffer to prevent any memory kerfluffles

    printf("Initializing file buffer...\n");

    for (int i = 0; i < sizeof buffer; i++) {
        char zero[] = "\00";
        memmove(buffer + i, zero, sizeof zero);
    }
    return;
}

void write_to_file() {
    // write the buffer's contents to file

    printf("Saving file...\n");

    FILE *write_ptr;
    write_ptr = fopen("BASLUS-15249EVELYN","wb");
    fwrite(buffer,sizeof(buffer),1,write_ptr);
    return;
}

void checkerboard() {
    // silly test pattern as a placeholder :3
    for (int i = 0; i < 128; i++) {
        int offset = sizeof(char) * i;
        char data[2];

        if (i%16 >= 8) {strcpy(data, "\x0F");} else {strcpy(data, "\xF0");}

        memmove(buffer + (sizeof(char)*128) + offset, data, sizeof(char));
    }

    return;
}

void allcolors() {
    // silly test pattern as a placeholder :3
    for (int i = 0; i < 16; i++) {
        int offset = sizeof(char) * (i*8);
        char data[] = "\x10\x32\x54\x76\x98\xBA\xDC\xFE";

        memmove(buffer + (sizeof(char)*128) + offset, data, sizeof(data));
    }

    return;
}

void png_to_bytes(char* path) {
    // converts a given PNG file to a 16x16 4bpp indexed image
    // currently ONLY supports black and white due to the palette shenanagains listed in main()

    // initialize SDL, we only need it for loading images
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[!] %s\n", SDL_GetError());
        return;
    }

    IMG_Init(IMG_INIT_PNG);

    printf("Initialized SDL!\n");

    // create a dummy renderer, this won't actually create a window, its just required for scaling images
    printf("Loading image (%s)...\n", "test.png");

    SDL_Renderer* renderer = SDL_CreateRenderer(NULL, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_Surface* tmp = IMG_Load(path);
    SDL_Surface* texture = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 0, 0, 0);

    // checks to see if the image has been loaded
    if (tmp == NULL) {
        printf("[!] Couldn't load the image!\n");
        return;
    }

    // scale down the texture to 16x16, the size of an icon
    printf("Resizing image to 16x16...\n");
    SDL_BlitScaled(tmp, NULL, texture, NULL);

    printf("Reading image pixels...\n");

    int index = 0;
    int index2 = 0;
    uint8_t nibble_a;
    uint8_t nibble_b;

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            // stores colors here for convenience
            // note the alpha channel, we never actually NEED it but hey, its provided by the function, so why not
            SDL_Color rgb = {0, 0, 0, 255};

            // formula for reading a given X and Y coordinate in an image
            uint32_t p = *((uint32_t*)texture->pixels + x * texture->w + y);
            SDL_GetRGBA(p, texture->format, &rgb.r, &rgb.g, &rgb.b, &rgb.a);

            // this is just for debugging pixel reads, you can ignore this unless you're changing the above couple lines
            //printf("%i %i: %i %i %i\n", x, y, rgb.r, rgb.g, rgb.b);

            // convert the RGB value to either black or white
            int gray = (rgb.r + rgb.g + rgb.b) / 3;

            // okay... here comes some clunk
            // what we need to do is write each of our pixels as one nibble, except C doesn't have a native nibble type
            // so this dumb solution i came up with maintains two nibbles, A and B, and then adds them together
            // and then pushes that result into memory. horrible. terrible. but apparently it works almost
            if (gray >= 127) {
                if (index%2 == 0) {nibble_a = 0x0F;} else {nibble_b = 0x0F;}
            } else {
                if (index%2 == 0) {nibble_a = 0x00;} else {nibble_b = 0x00;}
            }

            // every other instance of this forloop, we add the two nibbles together here
            if (index%2==1) {
                uint8_t byte = (nibble_a << 4) | nibble_b;

                // more debugging stuff dont worry about this
                //printf("%02x", byte);
                //if (index%16 == 15) {printf("\n");}

                memmove(buffer + (sizeof(char)*128) + index2, &byte, sizeof(uint8_t));
                nibble_a = 0;
                nibble_b = 0;
                index2++;
            }

            index++;
        }
    }

    // clean up SDL stuff
    printf("Cleaning up SDL...\n");
    SDL_FreeSurface(tmp);
    SDL_FreeSurface(texture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

int main(int argv, char** args) {
    // check if there's actually any arguments
    if (argv < 2) {
        printf("Please specify a PNG file. Run with -h for syntax & other info!");
        return 0;
    }

    if (argv >= 2) {
        // only check the first argument, ignore everything else
        if (strncmp(args[1], "-h", 2) == 0) {
            // prints help
            printf("\nEVELYN written by SuperFromND. (c) 2023, open-source under MIT <3"
            "\nAccepted parameters are:\n\n"
            "-h         - Print this message\n"
            "[file].png - Input file (preferrably 16x16 monochrome)\n");
            return 0;
        }
    }

    init_buffer();

    // these first several memwrites lay down someheader data that all PS saves absolutely need
    memmove(buffer, "\x53\x43", sizeof(char)*2); // magic number
    memmove(buffer + sizeof(char)*2, "\x11", sizeof(char)); // frame count (00 for none, 11 for 1 frame, 12 for two, 13 for three)
    memmove(buffer + sizeof(char)*3, "\x01", sizeof(char)); // # of blocks used, not *super* important for us i think

    // now we write a savedata name!
    // the name is encoded in shift-JIS, possible future todo is adding the ability to encode into that format for custom names
    char name[64] = {0};
    strcpy(name, "\x44\x41\x54\x41\x20\x47\x45\x4e\x27\x44\x20\x42\x59\x20\x45\x56\x45\x4c\x59\x4e\x20\x2f\x20\x50\x52\x4f\x47\x20\x42\x59\x20\x53\x55\x50\x45\x52\x46\x52\x4f\x4d\x4e\x44");
    memmove(buffer + sizeof(char)*4, name, sizeof(name));

    // color table defined here
    // believe me when i say i tried for HOURS to make actual color palettes work
    // but screw it; just using black and white for my own sanity, i'll leave making actual palettes as an exercise for the reader
    memmove(buffer + sizeof(char)*96, "\x00\x00\xFF\xFF\x00\x00\xFF\xFF\x00\x00\xFF\xFF\x00\x00\xFF\xFF\x00\x00\xFF\xFF\x00\x00\xFF\xFF\x00\x00\xFF\xFF\x00\x00\xFF\xFF", sizeof(char) * 32);

    // the next 128 bytes (or 256/384 if you have multiple icons) are the actual icon data
    // resolution of an icon is 16x16, with 1 byte representing two pixels (4bpp)

    png_to_bytes(args[1]);

    write_to_file();
    return 0;
}
