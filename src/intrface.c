/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIN32__
#include <windows.h>
#endif

#include "intl2.h"
#include "headers.h"
#include "calc_ver.h"
#include "calc_err.h"
#include "calc_def.h"
#include "tixx.h"

/*****************/
/* Internal data */
/*****************/
TicableLinkCable *cable;
TicalcInfoUpdate *update;
int               ticalcs_calc_type;
int               lock = 0;

/****************/
/* Entry points */
/****************/

/*
  This function should be the first one to call.
*/
TIEXPORT int TICALL ticalc_init()
{
  tifiles_init();

  DISPLAY(_("Libticalcs: version %s\n"),
          LIBTICALCS_VERSION);

  if(strcmp(tifiles_get_version(), LIB_FILES_VERSION_REQUIRED) < 0)
    {
      DISPLAY(_("Libtifiles: version mismatches. Library version >= <%s> is required.\n"),
              LIB_FILES_VERSION_REQUIRED);
      exit(-1);
    }

  return 0;
}

/*
  This function should be called when the libticalcs library is
  no longer used.
*/
TIEXPORT int TICALL ticalc_exit()
{
  tifiles_exit();

  return 0;
}

/***********/
/* Methods */
/***********/

/* Return the version number of the library */
TIEXPORT const char* TICALL ticalc_get_version()
{
  return LIBTICALCS_VERSION;
}

/* 
   Set up the update functions in order to be independant of the GUI
*/
TIEXPORT
void TICALL ticalc_set_update(TicalcInfoUpdate *iu,
			      void (*start)   (void),
			      void (*stop)    (void),
			      void (*refresh) (void),
			      void (*pbar)    (void),
			      void (*label)   (void))
{
  iu->cancel  = 0;
  iu->start   = start;
  iu->stop    = stop;
  iu->refresh = refresh;
  iu->pbar    = pbar;
  iu->label   = label;

  update = iu;
}

static void print_informations();

/* Set up link cable to use for calculator access */
TIEXPORT
void TICALL ticalc_set_cable(TicableLinkCable *lc)
{
  cable = lc;
}

/*
  Set up the calculator functions according to the calculator type
*/
TIEXPORT
void TICALL ticalc_set_calc(int type, TicalcFncts *calc)
{
  ticalcs_calc_type = type;
  tifiles_set_calc(ticalcs_calc_type);

#ifdef ENABLE_NLS
#if !defined(__WIN32__)
  bindtextdomain(PACKAGE, NULL);
  textdomain(PACKAGE);
#endif
#endif

  print_informations();

  switch(type)
    {
    case CALC_TI92:
      calc->supported_operations = ti92_supported_operations;
      calc->is_silent = !0;
      calc->has_folder = !0;      
      calc->memory = MEMORY_USED;
      
      calc->isready         = ti92_isready;
      calc->send_key        = ti92_send_key;
      calc->screendump      = ti92_screendump;
      calc->recv_backup     = ti92_recv_backup;
      calc->send_backup     = ti92_send_backup;
      calc->directorylist   = ti92_directorylist;
      calc->recv_var        = ti92_recv_var;
      calc->send_var        = ti92_send_var;
      calc->dump_rom        = ti92_dump_rom;
      calc->send_flash      = ti92_send_flash;
      calc->recv_flash      = ti92_recv_flash;
      calc->get_idlist      = ti92_get_idlist;
      break;
      
    case CALC_V200:
    case CALC_TI92P:
    case CALC_TI89:
      calc->supported_operations = ti89_supported_operations;
      calc->is_silent = !0;
      calc->has_folder = !0;      
      calc->memory = MEMORY_USED;
      
      calc->isready         = ti89_isready;
      calc->send_key        = ti89_send_key;
      calc->screendump      = ti89_screendump;
      calc->recv_backup     = ti89_recv_backup;
      calc->send_backup     = ti89_send_backup;
      calc->directorylist   = ti89_directorylist;
      calc->recv_var        = ti89_recv_var;
      calc->send_var        = ti89_send_var;
      calc->dump_rom        = ti89_dump_rom;
      calc->send_flash      = ti89_send_flash;
      calc->recv_flash      = ti89_recv_flash;
      calc->get_idlist      = ti89_get_idlist;
      calc->get_clock       = ti89_get_clock;
      calc->set_clock       = ti89_set_clock;
      break;

    case CALC_TI86:
      calc->supported_operations = ti86_supported_operations;
      calc->is_silent = !0;
      calc->has_folder = 0;      
      calc->memory = MEMORY_FREE;
      
      calc->isready         = ti86_isready;
      calc->send_key        = ti86_send_key;
      calc->screendump      = ti86_screendump;
      calc->recv_backup     = ti86_recv_backup;
      calc->send_backup     = ti86_send_backup;
      calc->directorylist   = ti86_directorylist;
      calc->recv_var        = ti86_recv_var;
      calc->send_var        = ti86_send_var;
      calc->dump_rom        = ti86_dump_rom;
      calc->send_flash      = ti86_send_flash;
      calc->recv_flash      = ti86_recv_flash;
      calc->get_idlist      = ti86_get_idlist;
      break;

    case CALC_TI85:
      calc->supported_operations = ti85_supported_operations;
      calc->is_silent = 0;
      calc->has_folder = 0;      
      calc->memory = MEMORY_NONE;
      
      calc->isready         = ti85_isready;
      calc->send_key        = ti85_send_key;
      calc->screendump      = ti85_screendump;
      calc->recv_backup     = ti85_recv_backup;
      calc->send_backup     = ti85_send_backup;
      calc->directorylist   = ti85_directorylist;
      calc->recv_var        = ti85_recv_var;
      calc->send_var        = ti85_send_var;
      calc->dump_rom        = ti85_dump_rom;
      calc->send_flash      = ti85_send_flash;
      calc->recv_flash      = ti85_recv_flash;
      calc->get_idlist      = ti85_get_idlist;
      break;

    case CALC_TI83:
      calc->supported_operations = ti83_supported_operations;
      calc->is_silent = !0;
      calc->has_folder = 0;      
      calc->memory = MEMORY_FREE;
      
      calc->isready         = ti83_isready;
      calc->send_key        = ti83_send_key;
      calc->screendump      = ti83_screendump;
      calc->recv_backup     = ti83_recv_backup;
      calc->send_backup     = ti83_send_backup;
      calc->directorylist   = ti83_directorylist;
      calc->recv_var        = ti83_recv_var;
      calc->send_var        = ti83_send_var;
      calc->dump_rom        = ti83_dump_rom;
      calc->send_flash      = ti83_send_flash;
      calc->recv_flash      = ti83_recv_flash;
      calc->get_idlist      = ti83_get_idlist;
      break;

    case CALC_TI82:
      calc->supported_operations = ti82_supported_operations;
      calc->is_silent = 0;
      calc->has_folder = 0;      
      calc->memory = MEMORY_NONE;

      calc->isready         = ti82_isready;
      calc->send_key        = ti82_send_key;
      calc->screendump      = ti82_screendump;
      calc->recv_backup     = ti82_recv_backup;
      calc->send_backup     = ti82_send_backup;
      calc->directorylist   = ti82_directorylist;
      calc->recv_var        = ti82_recv_var;
      calc->send_var        = ti82_send_var;
      calc->dump_rom        = ti82_dump_rom;
      calc->send_flash      = ti82_send_flash;
      calc->recv_flash      = ti82_recv_flash;
      calc->get_idlist      = ti82_get_idlist;
      break;

    case CALC_TI83P:
    case CALC_TI73:
      calc->supported_operations = ti73_supported_operations;
      calc->is_silent = !0;
      calc->has_folder = 0;      
      calc->memory = MEMORY_FREE;
      
      calc->isready         = ti73_isready;
      calc->send_key        = ti73_send_key;
      calc->screendump      = ti73_screendump;
      calc->recv_backup     = ti73_recv_backup;
      calc->send_backup     = ti73_send_backup;
      calc->directorylist   = ti73_directorylist;
      calc->recv_var        = ti73_recv_var;
      calc->send_var        = ti73_send_var;
      calc->dump_rom        = ti73_dump_rom;
      calc->send_flash      = ti73_send_flash;
      calc->recv_flash      = ti73_recv_flash;
      calc->get_idlist      = ti73_get_idlist;
      break;

    default:
    fprintf(stderr, "Function not implemented. This is a bug. Please report it.");
    fprintf(stderr, "Informations:\n");
    fprintf(stderr, "Calc: %i\n", type);
    fprintf(stderr, "Program halted before crashing...\n");
    exit(-1);
    break;
  }  
}

/*
  Set up the calculator functions according to the calculator type
*/
TIEXPORT
int  TICALL ticalc_get_calc(int *type)
{
  *type = ticalcs_calc_type;
  return 0;
}

TIEXPORT
int TICALL ticalc_get_calc2(void)
{
  return ticalcs_calc_type;
}

static void print_informations(void)
{
  DISPLAY(_("Libticalcs settings...\n"));
  
  switch(ticalcs_calc_type)
    {
    case CALC_V200:
      DISPLAY(_("  Calc type: V200\n"));
      break;
    case CALC_TI92P:
      DISPLAY(_("  Calc type: TI92+\n"));
      break;
    case CALC_TI92:
      DISPLAY(_("  Calc type: TI92\n"));
      break;
    case CALC_TI89:
      DISPLAY(_("  Calc type: TI89\n"));
      break;
    case CALC_TI86:
      DISPLAY(_("  Calc type: TI86\n"));
      break;
    case CALC_TI85:
      DISPLAY(_("  Calc type: TI85\n"));
      break;
    case CALC_TI83P:
      DISPLAY(_("  Calc type: TI83+\n"));
      break;
    case CALC_TI83:
      DISPLAY(_("  Calc type: TI83\n"));
      break;
    case CALC_TI82:
      DISPLAY(_("  Calc type: TI82\n"));
      break;
    case CALC_TI73:
      DISPLAY(_("  Calc type: TI73\n"));
      break;
    default: // error !
      DISPLAY(_("Oops, there is a bug. Unknown calculator.\n"));
      break;
    }
}

/* Initialize i18n support under Win32 platforms */
#if defined(__WIN32__)
BOOL WINAPI DllMain(  HINSTANCE hinstDLL,  // handle to DLL module
		      DWORD fdwReason,     // reason for calling function
		      LPVOID lpvReserved)   // reserved);
{
#ifdef ENABLE_NLS
  char buffer[65536];
  HINSTANCE hDLL = hinstDLL;
  int i=0;

  GetModuleFileName(hinstDLL, buffer, 65535);
  for(i=strlen(buffer); i>=0; i--) { if(buffer[i]=='\\') break; }
  buffer[i]='\0';
  strcat(buffer, "\\locale\\");
  
  bindtextdomain (PACKAGE, buffer);	
  //textdomain (PACKAGE);
#endif
}
#endif
