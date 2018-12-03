#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int IMWD = 16;
int IMHT = 16;

typedef unsigned char uchar;

// typedef struct compress_row_data
// {
//     int length;
//     int patternCount[];
// } data;

typedef struct compress_row
{
    int length;
    int *patternCount;
    uchar pattern[];
} row;

uchar packBit(uchar c, int val, int location);
int getBit(uchar c, int location);

row * compress(uchar c[IMWD/8])
{
    int patternLength = 0; //This will be calculated
    int lastBit = 2;
    int bitCount[IMWD]; //Worst case, find a way around?
    for (int i = 0; i < IMWD; i++) bitCount[i] = 0;
    int patternLoc = IMWD-1;
    int patternX = 0;
    int get;
    for (int x = 0; x < IMWD / 8; x++)
    {
        for (int i = 7; i >= 0; i--)
        {
            get = getBit(c[x], i);
            if(get != lastBit) {
                // printf("%d\n", get);
                // printf("%d\n", patternLength);
                patternLength++;
                // printf("%d\n\n", patternLength);
                lastBit = get;
            }
            bitCount[patternLength-2]++;
        }
    }
    row *comp = malloc(sizeof(row) + ceil(patternLength/8));
    comp->length = patternLength;
    comp->patternCount = malloc(sizeof(int) * patternLength);
    for (int i = 0; i < patternLength; i++) comp->patternCount[i] = bitCount[i];
    lastBit = 2;
    for (int x = 0; x < IMWD / 8; x++)
    {
        for (int i = 7; i >= 0; i--)
        {
            get = getBit(c[x], i);
            if (get != lastBit) {
                if (lastBit != 2) {
                    // printf("%d\n", lastBit);
                    packBit(comp->pattern[patternX], lastBit, patternLoc % 8);
                    patternLoc--;
                    if (patternLoc % 8 == 7) {
                        patternX++;
                    }
                } //packBit(comp->pattern[x], lastBit, patternLoc % 8);
                lastBit = get;
            }
        }
    }
    // printf("%d\n", get);
    if (get != lastBit) packBit(comp->pattern[patternX], lastBit, patternLoc % 8);
    return comp;
}

void printBits(uchar c)
{
    // for (int i = 7; i >= 0; i--)
    // {
    //     printf("%d ", i);
    // }
    // printf("\n");
    for (int i = 7; i >= 0; i--)
    {
        int x = (c >> i) & 1;
        printf("%d ", x);
    }
    // printf("\n");
}

//These functions will help assue bitpacking happens correctly
uchar packBit(uchar c, int val, int location)
{
    if (val == 1)
        c |= 1 << location;
    else
    {
        char d = 0xFF; //More efficient way for ding this?
        d ^= 1 << location;
        c &= d;
    }
    return c;
}

//Gets correct bit
int getBit(uchar c, int location)
{
    int bit = (c >> location) & 1;
    // printf("Bit:%d at Location:%d\n", bit, location);
    return bit;
}

void emptyChar(uchar val[16][2])
{
    for (int y = 0; y < 16; y++)
        for (int x = 0; x < 2; x++)
            val[x][y] = 0x00;
}

void testArray()
{
    // int IMWD = 16; //Test alternate method for accessing array without mod
    // int IMHT = 16;
    uchar c[16][2];
    emptyChar(c);
    // for (int y = 0; y < IMHT; y++)
    // {
    //     for (int i = 0; i < (int)ceil(IMWD / 8); i++)
    //         printBits(c[y][i]);
    //     printf("%d\n", y);
    // }
    // printf("Step\n");
    for (int y = 0; y < IMHT; y++)
    { //Works
        for (int i = 0; i < (int)ceil(IMWD / 8); i++)
            for (int x = 7; x >= 0; x--)
            {
                //If is just to add some variation in test in implementation would look like:
                // c[i][y] = packBit(c[i][y], read, x); with read being the channel input 0 or 1
                if ((x % 2 == 0 && i % 2 == 0) || (x % 2 == 1 && i % 2 == 1))
                    c[y][i] = packBit(c[y][i], 1, x);
                else
                    c[y][i] = packBit(c[y][i], 0, x);
            }
    }
    // for (int y = 0; y < IMHT; y++)
    // {
    //     for (int i = 0; i < (int)ceil(IMWD / 8); i++)
    //         printBits(c[y][i]);
    //     printf("%d\n", y);
    // }
    for (int i = 0; i < (int)ceil(IMWD / 8); i++)
            printBits(c[0][i]);
    printf("\n");
    row *comp;
    comp = compress(c[0]);
    for (int i = 0; i < 2; i++) printBits(comp->pattern[i]);
    printf("\n");
    for (int i = 0; i <= comp->length; i++) printf("%d ",comp->patternCount[i]);
    printf("\n");
    printf("%d\n", comp->length);
}

int main(int n, char **argv)
{
    testArray();
    // uchar c = 0; //Function tests
    // c = packBit(c, 1, 1);
    // getBit(c, 0);
    // getBit(c, 1);
    // getBit(c, 2);
    // printBits(c);
    // c = packBit(c, 1, 7);
    // getBit(c, 7);
    // printBits(c);
    // c = packBit(c, 1, 0);
    // getBit(c, 0);
    // printBits(c);
    // c = 0xFF;
    // printBits(c);
    // getBit(c, 0);
    // getBit(c, 1);
    // getBit(c, 2);
    // c = packBit(c, 0, 0);
    // getBit(c, 0);
    // printBits(c);
    // c = packBit(c, 0, 7);
    // getBit(c, 7);
    // printBits(c);
    // c = packBit(c, 0, 5);
    // getBit(c, 5);
    // printBits(c);
}

// for (int y = 0; y < IMHT; y++) {
//       for (int x = 0; x < IMWD; x++) {
//           c_in :> read;
//           val[(int)floor(x/8)][y] |= read << (mod((8 - (x + 1)), 8)); //Loads bits into character array
//       }
// }
//Possible alternate implementaation using functions from above
// for (int y = 0; y < IMHT; y++) {
//     for (int i = 0; i < (int)ceil(IMWD/8); i++)
//         for (int x = 7; x >= 0; x--) {
//             val[i][y] = packBit(val[i][y], read, x);
//         }
// }
