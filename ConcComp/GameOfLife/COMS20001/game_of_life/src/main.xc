// COMS20001 - Cellular Automaton Farm - Initial Code Skeleton
// (using the XMOS i2c accelerometer demo code)

#include <platform.h>
#include <xs1.h>
#include <math.h>
#include <stdio.h>
#include "pgmIO.h"
#include "i2c.h"

#define  IMHT 16                  //image height
#define  IMWD 16                  //image width

typedef unsigned char uchar;      //using uchar as shorthand

port p_scl = XS1_PORT_1E;         //interface ports to orientation
port p_sda = XS1_PORT_1F;

#define FXOS8700EQ_I2C_ADDR 0x1E  //register addresses for orientation
#define FXOS8700EQ_XYZ_DATA_CFG_REG 0x0E
#define FXOS8700EQ_CTRL_REG_1 0x2A
#define FXOS8700EQ_DR_STATUS 0x0
#define FXOS8700EQ_OUT_X_MSB 0x1
#define FXOS8700EQ_OUT_X_LSB 0x2
#define FXOS8700EQ_OUT_Y_MSB 0x3
#define FXOS8700EQ_OUT_Y_LSB 0x4
#define FXOS8700EQ_OUT_Z_MSB 0x5
#define FXOS8700EQ_OUT_Z_LSB 0x6

/////////////////////////////////////////////////////////////////////////////////////////
//
// Read Image from PGM file from path infname[] to channel c_out
//
/////////////////////////////////////////////////////////////////////////////////////////
void DataInStream(char infname[], chanend c_out)
{
  int res;
  uchar line[ IMWD ];
  printf( "DataInStream: Start...\n" );

  //Open PGM file
  res = _openinpgm( infname, IMWD, IMHT );
  if( res ) {
    printf( "DataInStream: Error openening %s\n.", infname );
    return;
  }

  //Read image line-by-line and send byte by byte to channel c_out
  for( int y = 0; y < IMHT; y++ ) {
    _readinline( line, IMWD );
    for( int x = 0; x < IMWD; x++ ) {
        if (line[x] == 0xFF) c_out <: 1;
        else c_out <: 0;
      printf( "-%4.1d ", line[ x ] ); //show image values
    }
    printf( "\n" );
  }

  //Close PGM image file
  _closeinpgm();
  printf( "DataInStream: Done...\n" );
  return;
}

void emptyChar(uchar val[IMWD/8][IMHT]) {
    for (int y = 0; y < IMHT; y++) for (int x = 0; x < IMWD/8; x++) val[x][y] = 0x00;
}

uchar oneBitChangeMask(int x, int bit) {
    uchar result = 0xFF;
    result ^= (bit ^ 1) << ((8 - (x + 1)) % 8);
}

int mod(int x, int m) {
//    if (x == m) x = 0;
    if (x == -1) x = m-1;
//    while (x < 0) x += m;
    return x % m;
}

/**
 * Function to check if given bit is live or dead.
 */
int checkNeighbourBit(int x, int y, uchar check[IMWD/8][IMHT]) {
    int arrayAddressX = mod(x, IMWD);
    int arrayAddressY = mod(y, IMHT);
    x = mod(x, 8);
    //What it should do:
    //Take the correct position array, we get this above, and right shift the character by the correct ammount of bits
    //for checking. -1 should give x = 7 which gives a shift of 8 - x+1 = 0, while say 16 should give x=0 and
    //a shift of 8 - x+1 = 7 to check the leftmost bit in the character (we consider this the first bit)
    return (((check[(int)floor(arrayAddressX/8)][arrayAddressY] >> (mod((8 - (x + 1)), 8))) & 1) == 1);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Start your implementation by changing this function to implement the game of life
// by farming out parts of the image to worker threads who implement it...
// Currently the function just inverts the image
//
/////////////////////////////////////////////////////////////////////////////////////////
void distributor(chanend c_in, chanend c_out, chanend fromAcc)
{
  int read;
  uchar val[IMWD/8][IMHT]; //IMWD and IMHT are defined as 16, will need to change to handle different file res
  uchar val2[IMWD/8][IMHT];
  int count = 0;

  emptyChar(val);//Set characters to all 0 bit
  emptyChar(val2);
  //Starting up and wait for tilting of the xCore-200 Explorer
  printf( "ProcessImage: Start, size = %dx%d\n", IMHT, IMWD );
  printf( "Waiting for Board Tilt...\n" );
  fromAcc :> int value;

  //Read in and do something with your image values..
  //This just inverts every pixel, but you should
  //change the image according to the "Game of Life"
  printf( "Processing...\n" );
  for (int y = 0; y < IMHT; y++) {
      for (int x = 0; x < IMWD; x++) {
          c_in :> read;
          val[(int)floor(x/8)][y] |= read << (mod((8 - (x + 1)), 8)); //Loads bits into character array
      }
  }
  for (int y = 0; y < IMHT; y++) {
         for (int x = 0; x < IMWD; x++) {
             if (checkNeighbourBit(x-1, y, val)) count++;
             if (checkNeighbourBit(x+1, y, val)) count++;
             if (checkNeighbourBit(x, y-1, val)) count++;
             if (checkNeighbourBit(x, y+1, val)) count++;
             if (checkNeighbourBit(x-1, y-1, val)) count++;
             if (checkNeighbourBit(x+1, y+1, val)) count++;
             if (checkNeighbourBit(x-1, y+1, val)) count++;
             if (checkNeighbourBit(x+1, y-1, val)) count++;
             if (checkNeighbourBit(x, y, val)) { //Issue is here
                 if (count > 3 || count < 2) val2[(int)floor(x/8)][y] ^= (1 << (mod((8 - (x + 1)),8))); //Toggles live bit to dead
             } else if (count == 3) { val2[(int)floor(x/8)][y] ^= (1 << (mod((8 - (x + 1)), 8)));
             } else val2[(int)floor(x/8)][y] |= ((val[(int)floor(x/8)][y] >> (mod((8 - (x + 1)), 8))) & 1) << (mod((8 - (x + 1)), 8));
             count = 0;
         }
     }
  for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 2; x++) {
                for (int i = 0; i < 8; i++) {
                      printf("%d", !!((val[x][y] << i) & 0x80));
                  } }
                  printf("\n");

        }
  printf("\n");
  for (int y = 0; y < 15; y++) {
      for (int x = 0; x < 2; x++) {
          for (int i = 0; i < 8; i++) {
                printf("%d", !!((val2[x][y] << i) & 0x80));
            } }
            printf("\n");

  }
   for( int y = 0; y < IMHT; y++ ) {   //go through all lines OR HERE <---------------
     for( int x = 0; x < IMWD; x++ ) { //go through each pixel per line
         if ((uchar)(((val2[(int)(floor(x/8))][y] ^ 0xFF ) >> (mod((8 - (x+1)), 8))) & 1) == 1) c_out <: 1;
         else c_out <: 0;
//         printf("%d\n", (uchar)(((val2[(int)(floor(x/8))][y] ^ 0xFF ) >> (mod((8 - (x+1)), 8))) & 1));
//       c_out <: (uchar)(((val2[(int)(floor(x/8))][(int)(floor(y/8))] ^ 0xFF ) >> (mod((8 - (x+1)), 8))) & 1); //send some modified pixel out
     }
   }
  printf( "\nOne processing round completed...\n" );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Write pixel stream from channel c_in to PGM image file
//
/////////////////////////////////////////////////////////////////////////////////////////
void DataOutStream(char outfname[], chanend c_in)
{
  int inc;
  int res;
  uchar line[ IMWD ];

  //Open PGM file
  printf( "DataOutStream: Start...\n" );
  res = _openoutpgm( outfname, IMWD, IMHT );
  if( res ) {
    printf( "DataOutStream: Error opening %s\n.", outfname );
    return;
  }

  //Compile each line of the image and write the image line-by-line
  for( int y = 0; y < IMHT; y++ ) {
    for( int x = 0; x < IMWD; x++ ) {
      c_in :> inc;
      if (inc == 1) line[x] = 0xFF;
      else line[x] = 0x00;
    }
    _writeoutline( line, IMWD );
    printf( "DataOutStream: Line written...\n" );
  }

  //Close the PGM image
  _closeoutpgm();
  printf( "DataOutStream: Done...\n" );
  return;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Initialise and  read orientation, send first tilt event to channel
//
/////////////////////////////////////////////////////////////////////////////////////////
void orientation( client interface i2c_master_if i2c, chanend toDist) {
  i2c_regop_res_t result;
  char status_data = 0;
  int tilted = 0;

  // Configure FXOS8700EQ
  result = i2c.write_reg(FXOS8700EQ_I2C_ADDR, FXOS8700EQ_XYZ_DATA_CFG_REG, 0x01);
  if (result != I2C_REGOP_SUCCESS) {
    printf("I2C write reg failed\n");
  }
  
  // Enable FXOS8700EQ
  result = i2c.write_reg(FXOS8700EQ_I2C_ADDR, FXOS8700EQ_CTRL_REG_1, 0x01);
  if (result != I2C_REGOP_SUCCESS) {
    printf("I2C write reg failed\n");
  }

  //Probe the orientation x-axis forever
  while (1) {

    //check until new orientation data is available
    do {
      status_data = i2c.read_reg(FXOS8700EQ_I2C_ADDR, FXOS8700EQ_DR_STATUS, result);
    } while (!status_data & 0x08);

    //get new x-axis tilt value
    int x = read_acceleration(i2c, FXOS8700EQ_OUT_X_MSB);

    //send signal to distributor after first tilt
    if (!tilted) {
      if (x>30) {
        tilted = 1 - tilted;
        toDist <: 1;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Orchestrate concurrent system and start up all threads
//
/////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

i2c_master_if i2c[1];               //interface to orientation

char infname[] = "test.pgm";     //put your input image path here
char outfname[] = "testout.pgm"; //put your output image path here
chan c_inIO, c_outIO, c_control;    //extend your channel definitions here

par {
    i2c_master(i2c, 1, p_scl, p_sda, 10);   //server thread providing orientation data
    orientation(i2c[0],c_control);        //client thread reading orientation data
    DataInStream(infname, c_inIO);          //thread to read in a PGM image
    DataOutStream(outfname, c_outIO);       //thread to write out a PGM image
    distributor(c_inIO, c_outIO, c_control);//thread to coordinate work on image
  }

  return 0;
}
