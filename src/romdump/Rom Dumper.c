///* Hey EMACS -*- linux-c -*- */
/* $Id: main.c 347 2004-05-31 09:39:53Z roms $ */

/*  RomDumper - an TI89/92+/V200PLT ROM dumper
 *
 *  Copyright (c) 2004, Romain Li�vin for the TiLP and TiEmu projects
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#define USE_TI89              // Compile for TI-89
#define USE_TI92PLUS          // Compile for TI-92 Plus
#define USE_V200              // Compile for V200

#define MIN_AMS 200           // Compile for AMS 2.00 or higher

#define SAVE_SCREEN           // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files

#define VERSION		"1.00"			// Version

int SendByte(BYTE c)
{
	while(OSWriteLinkBlock(&c, 1))
	{
		if(kbhit())
			return 0;
    }
    
    return 1;
}

int GetByte(BYTE *c)
{
    BYTE c;
    
    while(1)
    {
        if(OSReadLinkBlock(c, 1))
            return 1;
            
        if(kbhit())
        	return 0;	//0xcc
    }
    
    return c;
}

int SendSegment(char *ptr)
{
    WORD csum;
    int i;
    BYTE c;

        for (i = 0, csum = 0; i < 1024; i++)
        {
            BYTE ch = ptr[i];
            
            if(!SendByte(ch)) 
            	return 0;
            csum += (WORD)((BYTE)(ch));
        }
        
        if(!SendByte(csum >> 8)) 
        	return 0;
        if(!SendByte(csum & 0xff)) 
        	return 0;
        	
        i = GetByte(&c);
  
        if (!i)
            return 0;
            
        if (c == 0xda)
            return 1;
        
    
    return 0;
}


#define MB	(1024*1024)

//TI89  = 0
//TI92+ = 1
//V200  = 3
const unsigned long rom_sizes[4] = { 2097152,  2097152,  -1, 4194304  };
const unsigned long rom_bases[4] = { 0x200000, 0x400000, -1, 0x200000 };
const unsigned int  lcd_bases[4] = { 0x4c00,   0x4c00,   -1, 0x4c00 };

// Main Function
void _main(void)
{
	unsigned long rom_size = rom_sizes[CALCULATOR];
	unsigned long rom_base = rom_bases[CALCULATOR];
	//unsigned int  lcd_base = lcd_bases[CALCULATOR];
	
  unsigned long i;
  unsigned char *p;
  char str[30];
  
  ClrScr ();
  FontSetSys (F_8x10);
  
  sprintf(str, "RomDumper v%s", VERSION);
  DrawStr(0, 0, str, A_NORMAL);
  
  sprintf(str, "HW type: %i", HW_VERSION);
  DrawStr(0, 20, str, A_NORMAL);
  
  sprintf(str, "ROM base: %08x", rom_base);
  DrawStr(0, 30, str, A_NORMAL);  
  
  for(i = 0, p = (char *)rom_base; i < rom_size; i += 1024, p += 1024)
  {  	
		sprintf(str, "Done: %ld/%ldK", i >> 10, rom_size >> 10);
		
		switch(CALCULATOR)
		{
			case 0: // TI89
				DrawStr(0, 50, str, A_NORMAL);
			break;
			case 1: // TI92+
				DrawStr(0, 50, str, A_NORMAL);
			break;
			case 3: // V200
				DrawStr(0, 50, str, A_NORMAL);
			break;			
		}
		
		while(1);
		
		//if (!SendSegment(ptr))
      //      break;
  }
}

