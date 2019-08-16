/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#ifndef __HILEVEL_H
#define __HILEVEL_H

// Include functionality relating to newlib (the standard C library).

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

// Include functionality relating to the platform.

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

// Include functionality relating to the   kernel.

#include "lolevel.h"
#include     "int.h"

#define PROC_MAX (200) //Define values here to make changing easier CHANGE THIS TO DYNAMICALLY ADJUST TO SIZE OF STACK
#define PIPE_MAX (2000)

typedef int pid_t;

typedef enum { 
  STATUS_CREATED,
  STATUS_READY,
  STATUS_EXECUTING,
  STATUS_WAITING,
  STATUS_TERMINATED
} status_t;

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

typedef struct {
       int    pri;
       int    age;
     pid_t    pid;
  status_t status;
     ctx_t    ctx;
} pcb_t;

typedef struct {
    int sp;
    int size;
    int arr[PIPE_MAX]; //Set to max number of pipes available
} stack;

typedef struct {
    int readFD; //Read file decriptor
    int writeFD; //Write file descriptor
    int openFlag;
    int bufferMax;
    int bufferCount;
    char msg[8]; //Msg sent down pipe (Maybe turn into buffer to have ptrs to multiple chars)
} pipe_t;

#endif
