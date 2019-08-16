/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#include "hilevel.h"

uint32_t offset = 0x00001000; //Tracks offset for each program on program stack, needs to change, what if early program finishes?
//Multiply offset by position in program is placed in pcb
int active_proc = 0; //Tracks active number of processes

stack freeIds = {.sp = 0, .size = PROC_MAX, .arr = {0}}; //Take another look, see if we can start sp at 0
stack freePipes = {.sp = 0, .size = PIPE_MAX, .arr = {0}}; //Take another look, see if we can start sp at 0

pipe_t signals[PROC_MAX]; //Using to get kill signals to processes
pcb_t pcb[ PROC_MAX ]; pcb_t* current = NULL;
pipe_t pipes[ PIPE_MAX ];

void push(int i, int pcb) {
    if (pcb == 1) {
        if (freeIds.sp < freeIds.size) freeIds.arr[freeIds.sp++] = i;
    } else {
        if (freePipes.sp < freePipes.size) freePipes.arr[freePipes.sp++] = i;
    }
}

int pop(int pcb) {
    if (pcb == 1) {
        if (freeIds.sp > 0) return freeIds.arr[--freeIds.sp];
    } else {
        if (freePipes.sp > 0) return freePipes.arr[--freePipes.sp];
    }
    return -1;
}

int stackFull(int pcb) {
    if (pcb == 1) return freeIds.sp == freeIds.size;
    else return freePipes.sp == freePipes.size;
}

int stackEmpty(int pcb) {
    if (pcb == 1) return freeIds.sp == 0;
    else return freePipes.sp == 0;
}

void dispatch( ctx_t* ctx, pcb_t* prev, pcb_t* next ) {
  char prev_pid = '?', next_pid = '?';

  if( NULL != prev ) {
    memcpy( &prev->ctx, ctx, sizeof( ctx_t ) ); // preserve execution context of P_{prev}
    prev_pid = '0' + prev->pid;
  }
  if( NULL != next ) {
    memcpy( ctx, &next->ctx, sizeof( ctx_t ) ); // restore  execution context of P_{next}
    next_pid = '0' + next->pid;
  }

    PL011_putc( UART0, '[',      true );
    PL011_putc( UART0, prev_pid, true );
    PL011_putc( UART0, '-',      true );
    PL011_putc( UART0, '>',      true );
    PL011_putc( UART0, next_pid, true );
    PL011_putc( UART0, ']',      true );

    current = next;                             // update   executing index   to P_{next}

  return;
}

void schedule( ctx_t* ctx ) {
  int cur_index = 0; //Index of current pcb
  int index = -1; //Index of pcb with higherst pri+age
  int max_pri = -1; //High pri+age found
  for (int i = 0; i < PROC_MAX; i++) { //Needs to loop though all as no guarantee of order of processes
      //Only set as index if it has highest pri + age and isnt terminated or waiting
      if ((pcb[i].pri + pcb[i].age) > max_pri && !(pcb[i].status == STATUS_TERMINATED || pcb[i].status == STATUS_WAITING)) {
          max_pri = pcb[i].pri + pcb[i].age;
          if (index != -1) pcb[index].age += 1; //If overwriting an index, add 1 to the previous index age as it won't be scheduled this time
          index = i;
      } else pcb[i].age += 1; //If priority not high enough, increment age as it won't be scheduled this time
      if (current->pid == pcb[i].pid ) cur_index = i;
  }
  dispatch( ctx, &pcb[cur_index], &pcb[index] );      
    
  pcb[index].age = 0;
  pcb[cur_index].status = STATUS_READY;             
  pcb[index].status = STATUS_EXECUTING;
   
  return;
}

extern uint32_t tos_P;
// extern void     main_P3();
// extern void     main_P4();
// extern void     main_P5();
// extern void     main_P6();
extern void     main_console();
extern void     main_console2();

void hilevel_handler_rst( ctx_t* ctx              ) { 
  /* Initialise two PCBs, representing user processes stemming from execution 
   * of two user programs.  Note in each case that
   *    
   * - the CPSR value of 0x50 means the processor is switched into USR mode, 
   *   with IRQ interrupts enabled, and
   * - the PC and SP values matche the entry point and top of stack. 
   */
  
  for (int i = PROC_MAX - 1; i >= 0; i--) {
      push(i, 1);
      memset( &pcb[ i ], 0, sizeof( pcb_t ) ); 
      pcb[ i ].status   = STATUS_TERMINATED; //Sets all unused pcb slots to terminated so they are ignored when scheduling
      memset( &signals[ i ], 0, sizeof( pipe_t ) ); 
      signals[i].readFD = -1;
      signals[i].writeFD = -1;
      signals[i].openFlag = 1;
      signals[i].bufferMax = 1; //Only allow 1 signal at a time
      signals[i].bufferCount = 0;
  }
  for (int i = PIPE_MAX - 1; i >= 0; i--) {
      push(i, 0);
      memset( &pipes[ i ], 0, sizeof( pipe_t ) ); 
      pipes[i].readFD = 0;
      pipes[i].writeFD = 0;
      pipes[i].openFlag = 0;
      pipes[i].bufferMax = 8;
      pipes[i].bufferCount = 0;
  }
  
  int i = pop(1);
  
  memset( &pcb[ i ], 0, sizeof( pcb_t ) );     // initialise 0-th PCB = P_3
  pcb[ i ].pid      = i+1; //pcb[i] i + 1 = id
  pcb[ i ].pri      = 0;
  pcb[ i ].age      = 0;
  pcb[ i ].status   = STATUS_CREATED;
  pcb[ i ].ctx.cpsr = 0x50;
  pcb[ i ].ctx.pc   = ( uint32_t )( &main_console2 ); //Not generating shell, codio issue? mYabe meant to pop up window
  pcb[ i ].ctx.sp   = ( uint32_t )( &tos_P + i * offset  );
  
  active_proc++;    
    
  TIMER0->Timer1Load  = 0x00004000;
  TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

  GICC0->PMR          = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
  GICC0->CTLR         = 0x00000001; // enable GIC interface
  GICD0->CTLR         = 0x00000001; // enable GIC distributor

  /* Once the PCBs are initialised, we arbitrarily select the one in the 0-th 
   * PCB to be executed: there is no need to preserve the execution context, 
   * since it is is invalid on reset (i.e., no process will previously have
   * been executing).
   */

  dispatch( ctx, NULL, &pcb[ 0 ] );
	
  int_enable_irq();

  return;
}

void hilevel_handler_irq(ctx_t* ctx) {

  uint32_t id = GICC0->IAR;

  if( id == GIC_SOURCE_TIMER0 ) {
    schedule(ctx);
    TIMER0->Timer1IntClr = 0x01;
  }

  GICC0->EOIR = id;

  return;
}

void hilevel_handler_svc( ctx_t* ctx, uint32_t id ) {
  /* Based on the identifier (i.e., the immediate operand) extracted from the
   * svc instruction, 
   *
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call, then
   * - write any return value back to preserved usr mode registers.
   */
  switch( id ) {
    case 0x00 : { // 0x00 => yield()
      schedule( ctx );
      TIMER0->Timer1IntClr = 0x01; //Reset timer after cooperative yield
      break;
    }

    case 0x01 : { // 0x01 => write( fd, x, n )
      int   fd = ( int   )( ctx->gpr[ 0 ] );  
      char*  x = ( char* )( ctx->gpr[ 1 ] );  
      int    n = ( int   )( ctx->gpr[ 2 ] ); 
      if (fd < 3) { // FDs 3 and up are allocated to pipes
          for( int i = 0; i < n; i++ ) {
            PL011_putc( UART0, *x++, true );
          }

          ctx->gpr[ 0 ] = n;
          break;
      } else {
          int index = -1;
          for (int i = 0; i < PIPE_MAX; i++) {
              if (pipes[i].writeFD == fd) index = i;
          }
          if (index == -1) {
              ctx->gpr[0] = -1;
              break;
          }
          if (pipes[index].bufferCount + n > pipes[index].bufferMax) { //Reject if trying to write more characters than spaces in buffer
              ctx->gpr[0] = -1;
              break;
          }
          if (pipes[index].openFlag == 0) {
              ctx->gpr[0] = -1;
              break;
          }
          if (pipes[index].bufferCount >= pipes[index].bufferMax) {
              ctx->gpr[0] = -1;
              break;
          }
          for (int i = 0; i < n; i++) {
              pipes[index].msg[pipes[index].bufferCount] = *x++;
              pipes[index].bufferCount++;
          }
          ctx->gpr[0] = n;
          break;
      }
    }
          
    case 0x02 : { //0x02 => read(fd, x, n)
      int   fd = ( int   )( ctx->gpr[ 0 ] );  
      char*  x = ( char* )( ctx->gpr[ 1 ] );  
      int    n = ( int   )( ctx->gpr[ 2 ] ); 
      if (fd == -1) {
          if (signals[current->pid - 1].bufferCount <= 0) {
              memset(x, '0', 1);
              ctx->gpr[0] = -1;
              break;
          }
          memset(x, signals[current->pid - 1].msg[signals[current->pid - 1].bufferCount - 1], 1);
          signals[current->pid - 1].msg[signals[current->pid - 1].bufferCount - 1] = '0';
          --signals[current->pid - 1].bufferCount;
          ctx->gpr[0] = 1;
          break;
      }
      int index = -1;
      for (int i = 0; i < PIPE_MAX; i++) {
          if (pipes[i].readFD == fd) index = i;
      }
      if (index == -1) {
          ctx->gpr[0] = -1;
          break;
      }
      if (pipes[index].bufferCount - (n) < 0) { //Reject if trying to read more characters than available
          ctx->gpr[0] = -1;
          break;
      }
      if (pipes[index].openFlag == 0) { 
          ctx->gpr[0] = -1;
          break;
      }
      if (pipes[index].bufferCount <= 0) {
          ctx->gpr[0] = -1;
          break;
      }
      for (int i = 0; i < n; i++) pipes[index].bufferCount--; //Messages read out backwards as they are written in backwards
      for (int i = 0; i < n; i++) {
          memset(x, pipes[index].msg[pipes[index].bufferCount], 1);
          pipes[index].bufferCount++;
          *x++;
      }
      for (int i = 0; i < n; i++) pipes[index].bufferCount--;
      ctx->gpr[0] = n;
      break;
    }
          
    case 0x03 : { //0x03 => fork()
      //Fork duplicates calling process
      int i = pop(1); //Get next available pcb id
      if (i != -1) { //Only if id available duplicate current process
          memset( &pcb[ i ], 1, sizeof( pcb_t ) );
          pcb[ i ].pid      = i+1;
          pcb[ i ].pri      = current->pri;
          pcb[ i ].age      = current->age;
          pcb[ i ].status   = current->status;
          memcpy( &pcb[i].ctx, ctx, sizeof( ctx_t ) );
          pcb[ i ].ctx.sp   = ( uint32_t )( &tos_P + i * offset  ); //Give memory space
          uint32_t pcbSP = pcb[ i ].ctx.sp; //Copy all memory in memory space
          uint32_t ctxSP = ctx->sp;
          uint32_t limit = pcb[ i ].ctx.sp + offset;
          char* pcbP = pcb[ i ].ctx.sp;
          char* ctxP = ctx->sp;
          while (pcbP < limit) {
              memset(pcbP, *ctxP, 1);
              pcbP += 4;
              ctxP += 4;
          }
          pcb[ i ].ctx.sp = pcbSP;
          ctx->sp = ctxSP;
          ctx->gpr[0] = i+1; //Set r0 to the process id of child for parent
          pcb[ i ].ctx.gpr[0] = 0; //Set r0 to 0 for child
          active_proc++;
          ctx->gpr[0] = 1;
      } else {
          pcb[ i ].ctx.gpr[0] = -1;
      }
      
      break;
    }
          
    case 0x04 : { //0x04 => exit(x)
      int i = current->pid - 1; //Get location of current in pcb
      pcb[i].status = STATUS_TERMINATED; //Set process status to terminated so it can't be prioritised in scheduler
      schedule(ctx);
      pcb[i].status = STATUS_TERMINATED; //Set process status to terminated again as it has been reset by scheduler
      active_proc--;
      if (signals[i].bufferCount > 0) { //Remove any signals currently being sent to closed process id
          signals[i].msg[signals[i].bufferCount - 1] = '0';
          --signals[i].bufferCount;
      }
      push(i, 1); //Push pcb location to available pcb locations
        
      break;
    }
          
    case 0x05 : { //0x05 => exec(x)
      //Exec creates process from program pointed to by x, replace current process image
      int i = current->pid - 1;
      memset( &pcb[ i ], 0, sizeof( pcb_t ) );
      pcb[ i ].pid      = i + 1; //pcb[i].id i + 1
      pcb[ i ].pri      = 0;
      pcb[ i ].age      = 0;
      pcb[ i ].status   = STATUS_CREATED;
      pcb[ i ].ctx.cpsr = 0x50;
      pcb[ i ].ctx.pc   = ( uint32_t )( ctx->gpr[0] );
      pcb[ i ].ctx.sp   = ( uint32_t )( &tos_P + i * offset  );
      memcpy( ctx, &pcb[i].ctx, sizeof( ctx_t ) );
        
      break;
    }
          
    case 0x06 : { //0x06 => kill(pid, x)
      int id = ctx->gpr[0] - 1;
      int sig = ctx->gpr[1];
      if (pcb[id].status == STATUS_TERMINATED) { //Don't allow sending kill signal to non-existant processes
          ctx->gpr[0] = -1;
          break;
      }
      if (sig == 0x01) { //SIG_QUIT
          pcb[id].status = STATUS_TERMINATED; //Set process status to terminated so it can't be set in scheduler
          if (id != current->pid - 1) { //Don't allow process to call kill on self
              active_proc--;
              push(id, 1); //Push pcb location to available pcb locations
              if (signals[id].bufferCount > 0) {
                  signals[id].msg[signals[id].bufferCount - 1] = '0';
                  --signals[id].bufferCount;
              }
              ctx->gpr[0] = 1;
          } else {
              ctx->gpr[0] = -1;
          }
          break;
      }
      if (signals[id].bufferCount >= signals[id].bufferMax) {
          ctx->gpr[0] = -1;
          break;
      }
      signals[id].msg[signals[id].bufferCount] = 't';
      signals[id].bufferCount++;
      break;
    }
          
    case 0x07 : { //0x07 => nice(pid, x)
      //set process with id pid to priority x
      int pid = ctx->gpr[0];
      int pri = ctx->gpr[1];
      if (pid > PROC_MAX || pid < 1) break;
      pcb[pid-1].pri = pri;
      break;
    }
          
    case 0x08 : { //0x08 => pipe(fd[2]) Having way to close unwanted pipes may be useful
      int read = (int)(ctx->gpr[0]); //Pipe is meant to set pfds, copy functionality from open to here
      int write = (int)(ctx->gpr[1]);
      int id = pop(0);
      if (id == -1) {
          ctx->gpr[0] = -1;
          break;
      }
      pipes[id].readFD = read;
      pipes[id].writeFD = write;
      pipes[id].openFlag = 1;
      pipes[id].bufferMax = 8;
      pipes[id].bufferCount = 0;
      ctx->gpr[0] = 1;
      break;
    }
          
    case 0x09 : { //0x09 => open()
      int i = pop(0); //Get lowest stack address free
      push(i, 0);
      if (i >= 0) ctx->gpr[0] = i + 3; //Add 3 to bypass first 3 fds if i is 0
      else ctx->gpr[0] = -1; //Fail if no fds free
      break;
    }
          
    case 0x10 : { //0x10 => close(fd)
      int fd = ctx->gpr[0];
      int index = -1;
      for (int i = 0; i < PIPE_MAX; i++) {
          if (pipes[i].readFD == fd || pipes[i].writeFD == fd) index = i;
      }
      if (index == -1) {
          ctx->gpr[0] = -1;
          break;
      }
      pipes[index].readFD = -1;
      pipes[index].writeFD = -1;
      pipes[index].openFlag = 0;
      pipes[index].bufferMax = 8;
      pipes[index].bufferCount = 0;
      for (int i = 0; i < 8; i++) pipes[index].msg[i] = '0';
      push(index, 0);
      ctx->gpr[0] = 1;
      break;  
    }

    default   : { // 0x?? => unknown/unsupported
      break;
    }
  }
  return;
}
