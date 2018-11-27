#include <stdio.h>
#include <math.h>

typedef unsigned char uchar;

void printBits(uchar c) {
    for (int i = 7; i >= 0; i--) {
        printf("%d ", i);
    }
    printf("\n");
    for (int i = 7; i >= 0; i--) {
        int x = (c >> i) & 1;
        printf("%d ", x);
    }
    printf("\n");
}

//These functions will help assue bitpacking happens correctly
uchar packBit(uchar c, int val, int location) {
    if (val == 1) c |= 1 << location;
    else {
        char d = 0xFF; //More efficient way for ding this?
        d ^= 1 << location;
        c &= d;
    }
    return c;
}

//Gets correct bit
int getBit(uchar c, int location) {
    int bit = (c >> location) & 1;
    printf("Bit:%d at Location:%d\n", bit, location);
    return bit;
}

void emptyChar(uchar val[2][16]) {
    for (int y = 0; y < 16; y++) for (int x = 0; x < 2; x++) val[x][y] = 0x00;
}

void testArray() {
    int IMWD = 16; //Test alternate method for accessing array without mod
    int IMHT = 16;
    uchar c[2][16];
    emptyChar(c);
    for (int y = 0; y < IMHT; y++) {
        for (int i = 0; i < (int)ceil(IMWD/8); i++) printBits(c[i][y]);
        printf("%d\n", y);
    }
    printf("Step\n");
    for (int y = 0; y < IMHT; y++) { //Works
        for (int i = 0; i < (int)ceil(IMWD/8); i++)
            for (int x = 7; x >= 0; x--) {
                //If is just to add some variation in test in implementation would look like:
                // c[i][y] = packBit(c[i][y], read, x); with read being the channel input 0 or 1
                if ((x % 2 == 0 && i % 2 == 0) || (x % 2 == 1 && i % 2 == 1)) c[i][y] = packBit(c[i][y], 1, x);
                else c[i][y] = packBit(c[i][y], 0, x);
            }
    }
    for (int y = 0; y < IMHT; y++) {
        for (int i = 0; i < (int)ceil(IMWD/8); i++) printBits(c[i][y]);
        printf("%d\n", y);
    }
}

int main(int n, char **argv) {
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
