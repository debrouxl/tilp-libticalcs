//roms: receiving chars by interrupt (interrupt and user mode have been splitted)

// C Source File
// Created 18/09/02; 13:25:02

#define USE_TI89              // Produce .89z File

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#define SAVE_SCREEN           // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files

#define IO_DBUS_CONFIG 0x60000C
#define IO_DBUS_STATUS 0x60000D
#define IO_LINK_STATES 0x60000E
#define IO_LINK_BUFFER 0x60000F

#define AE   (1 << 7)
#define LD   (1 << 6)
#define LTO  (1 << 5)
#define CLE  (1 << 3)
#define CAIE (1 << 2)
#define CTX  (1 << 1)
#define CRX	 (1 << 0)

#define SLE (1 << 7)
#define STX (1 << 6)
#define SRX (1 << 5)
#define SLI (1 << 4)
#define SA  (1 << 3)

INT_HANDLER oldint4 = NULL;
volatile int counter = 0;
volatile unsigned char data = 0;
volatile int flag = FALSE;

// DBus interrupt handler
DEFINE_INT_HANDLER (link_handler)
{
  unsigned char status = peekIO(IO_DBUS_STATUS);	// save status (getting it -> clear register)
  
  counter++;
  if(status & SLE)
    { // link error
      putchar('$'); putchar('E');
      pokeIO(IO_DBUS_CONFIG, AE | LD | LTO);
      pokeIO(IO_DBUS_STATUS, peekIO(IO_DBUS_STATUS) & ~SLE);	// clear flag
    }
  
  if(status & STX)
    {	// TX buffer empty 	
      putchar('$'); putchar('T');	
      pokeIO(IO_DBUS_STATUS, peekIO(IO_DBUS_STATUS) & ~STX);	// clear flag
    }
  
  if(status & SRX)
    { // RX buffer full
      putchar('$'); putchar('R');
      data = peekIO(IO_LINK_BUFFER);
      flag = TRUE;
      
      pokeIO(IO_DBUS_STATUS, peekIO(IO_DBUS_STATUS) & ~SRX);	// clear flag
    }
  
  if(status & SLI)
    {	// link interrupt ?
      putchar('$'); putchar('I');
      pokeIO(IO_DBUS_STATUS, peekIO(IO_DBUS_STATUS) & ~SLI);	// clear flag
    }
  
  if(status & SA)
    {	// auto-start triggered: ignore
      putchar('$'); putchar('A');
      pokeIO(IO_DBUS_STATUS, peekIO(IO_DBUS_STATUS) & ~SA);	// clear flag
    }
  putchar(' ');
}

// Main Function
void _main(void)
{
  unsigned char  old_control = peekIO(IO_DBUS_CONFIG);
  
  // Save old handler and install new one
  oldint4 = GetIntVec(AUTO_INT_4);
  SetIntVec(AUTO_INT_4, &link_handler);
  pokeIO(IO_DBUS_CONFIG, /*AE |*/ CRX);		// enable interrupt on RX, auto-start must be DISABLED !
  
  ClrScr();
  FontSetSys(F_6x8);  
  while(!kbhit())
    {
      while(flag != TRUE);
      printf("%i: '%c'\n", counter, data);  //notice that counter is incremented twice 
      // spurious interrupts ? Flags: TRI and next TI
      flag = FALSE;
    }
  
  // restore register and handler
  pokeIO(IO_DBUS_CONFIG, old_control);
  SetIntVec(AUTO_INT_4, oldint4);	
}