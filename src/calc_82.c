/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

/*
	TI82 support. Note: the source code is the SAME as the TI85 support (same indentation).
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ticonv.h"
#include "ticalcs.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmd82.h"
#include "rom82.h"

#ifdef __WIN32__
#undef snprintf
#define snprintf _snprintf
#endif

// Screen coordinates of the TI83
#define TI82_ROWS  64
#define TI82_COLS  96

static int		is_ready	(CalcHandle* handle)
{
	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t max_cnt;
	int err;
	uint8_t buf[TI82_COLS * TI82_ROWS / 8];

	sc->width = TI82_COLS;
	sc->height = TI82_ROWS;
	sc->clipped_width = TI82_COLS;
	sc->clipped_height = TI82_ROWS;

	TRYF(ti82_send_SCR());
	TRYF(ti82_recv_ACK(NULL));

	err = ti82_recv_XDP(&max_cnt, buf);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti82_send_ACK());

	*bitmap = (uint8_t *)malloc(TI82_COLS * TI82_ROWS / 8);
	if(*bitmap == NULL)	return ERR_MALLOC;
	memcpy(*bitmap, buf, TI82_COLS * TI82_ROWS / 8);

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, TNode** vars, TNode** apps)
{	
	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* mem)
{
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
  int err = 0;
  uint16_t length;
  char varname[9];
  uint8_t rej_code;
  uint16_t status;

  snprintf(update_->text, sizeof(update_->text), _("Waiting user's action..."));
  update_label();

  length = content->data_length1;
  varname[0] = LSB(content->data_length2);
  varname[1] = MSB(content->data_length2);
  varname[2] = LSB(content->data_length3);
  varname[3] = MSB(content->data_length3);
  varname[4] = LSB(content->mem_address);
  varname[5] = MSB(content->mem_address);

  TRYF(ti82_send_VAR(content->data_length1, TI82_BKUP, varname));
  TRYF(ti82_recv_ACK(&status));

  do 
  {
	  // wait user's action
		update_refresh();

		if (update_->cancel)
		return ERR_ABORT;

		err = ti82_recv_SKP(&rej_code);
  }
  while (err == ERROR_READ_TIMEOUT);

  TRYF(ti82_send_ACK());
  switch (rej_code) 
  {
  case REJ_EXIT:
  case REJ_SKIP:
    return ERR_ABORT;
    break;
  case REJ_MEMORY:
    return ERR_OUT_OF_MEMORY;
    break;
  default:			// RTS
    break;
  }

  snprintf(update_->text, sizeof(update_->text), _("Sending..."));
  update_label();

  update_->max2 = 3;
  update_->cnt2 = 0;
  update_->pbar();

  TRYF(ti82_send_XDP(content->data_length1, content->data_part1));
  TRYF(ti82_recv_ACK(&status));
  update_->cnt2++;
  update_->pbar();

  TRYF(ti82_send_XDP(content->data_length2, content->data_part2));
  TRYF(ti82_recv_ACK(&status));
  update_->cnt2++;
    update_->pbar();

  TRYF(ti82_send_XDP(content->data_length3, content->data_part3));
  TRYF(ti82_recv_ACK(&status));
  update_->cnt2++;
    update_->pbar();

  //TRYF(ti82_send_EOT());

  return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
  char varname[9] = { 0 };

  snprintf(update_->text, sizeof(update_->text), _("Waiting for backup..."));
  update_label();

  content->model = CALC_TI82;
  strcpy(content->comment, tifiles_comment_set_backup());

  TRYF(ti82_recv_VAR(&(content->data_length1), &content->type, varname));
  content->data_length2 = (uint8_t)varname[0] | ((uint8_t)varname[1] << 8);
  content->data_length3 = (uint8_t)varname[2] | ((uint8_t)varname[3] << 8);
  content->mem_address  = (uint8_t)varname[4] | ((uint8_t)varname[5] << 8);
  TRYF(ti82_send_ACK());

  TRYF(ti82_send_CTS());
  TRYF(ti82_recv_ACK(NULL));

  update_->max2 = 3;
  update_->cnt2 = 0;
  update_->pbar();

  content->data_part1 = tifiles_ve_alloc_data(65536);
  TRYF(ti82_recv_XDP(&content->data_length1, content->data_part1));
  TRYF(ti82_send_ACK());
  update_->cnt2++;
  update_->pbar();

  content->data_part2 = tifiles_ve_alloc_data(65536);
  TRYF(ti82_recv_XDP(&content->data_length2, content->data_part2));
  TRYF(ti82_send_ACK());
  update_->cnt2++;
  update_->pbar();

  content->data_part3 = tifiles_ve_alloc_data(65536);
  TRYF(ti82_recv_XDP(&content->data_length3, content->data_part3));
  TRYF(ti82_send_ACK());
  update_->cnt2++;
  update_->pbar();

  content->data_part4 = NULL;

  return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content);
static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return send_var_ns(handle, mode, content);
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
  int i;
  int err;
  uint8_t rej_code;
  uint16_t status;
  char *utf8;

  snprintf(update_->text, sizeof(update_->text), _("Sending..."));
  update_label();

  for (i = 0; i < content->num_entries; i++) 
  {
    VarEntry *entry = content->entries[i];

    TRYF(ti82_send_VAR((uint16_t)entry->size, entry->type, entry->name));
    TRYF(ti82_recv_ACK(&status));

    snprintf(update_->text, sizeof(update_->text), _("Waiting user's action..."));
    update_label();
    do 
	{			// wait user's action
      update_refresh();
      if (update_->cancel)
		return ERR_ABORT;
      
	  err = ti82_recv_SKP(&rej_code);
    }
    while (err == ERROR_READ_TIMEOUT);

    TRYF(ti82_send_ACK());
    switch (rej_code) 
	{
    case REJ_EXIT:
      return ERR_ABORT;
      break;
    case REJ_SKIP:
      continue;
      break;
    case REJ_MEMORY:
      return ERR_OUT_OF_MEMORY;
      break;
    default:			// RTS
      break;
    }
    utf8 = ticonv_varname_to_utf8(handle->model, entry->name);
    snprintf(update_->text, sizeof(update_->text), _("Sending '%s'"), utf8);
	g_free(utf8);
    update_label();

    TRYF(ti82_send_XDP(entry->size, entry->data));
    TRYF(ti82_recv_ACK(&status));
  }

  if ((mode & MODE_SEND_ONE_VAR) || (mode & MODE_SEND_LAST_VAR)) 
  {
    TRYF(ti82_send_EOT());
    TRYF(ti82_recv_ACK(NULL));
  }

  return 0;
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** vr)
{
  int nvar = 0;
  int err = 0;
  char *utf8;

  snprintf(update_->text, sizeof(update_->text), _("Waiting var(s)..."));
  update_label();

  content->model = CALC_TI82;

  for (nvar = 0;; nvar++) 
  {
    VarEntry *ve;

	content->entries = tifiles_ve_resize_array(content->entries, nvar+1);
    ve = content->entries[nvar] = tifiles_ve_create();

    do 
	{
      update_refresh();
      if (update_->cancel)
		return ERR_ABORT;

      err = ti82_recv_VAR((uint16_t *)&(ve->size), &(ve->type), ve->name);
      fixup(ve->size);
    }
    while (err == ERROR_READ_TIMEOUT);

    TRYF(ti82_send_ACK());
    if (err == ERR_EOT) 
	  goto exit;
    TRYF(err);

    TRYF(ti82_send_CTS());
    TRYF(ti82_recv_ACK(NULL));

    utf8 = ticonv_varname_to_utf8(handle->model, ve->name);
    snprintf(update_->text, sizeof(update_->text), _("Sending '%s'"), utf8);
	g_free(utf8);
    update_label();

	ve->data = tifiles_ve_alloc_data(ve->size);
    TRYF(ti82_recv_XDP((uint16_t *)&(ve->size), ve->data));
    TRYF(ti82_send_ACK());
  }

exit:
  content->num_entries = nvar;
  if(nvar == 1)
  {
	strcpy(content->comment, tifiles_comment_set_single());
	*vr = tifiles_ve_dup(content->entries[0]);
  }
  else
  {
	strcpy(content->comment, tifiles_comment_set_group());
	*vr = NULL;
  }

  return 0;
}

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* idlist)
{
	return 0;
}

extern int rom_dump(CalcHandle* h, FILE* f);
extern int rom_dump_ready(CalcHandle* h);

static int		dump_rom	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	const char *prgname = "romdump.82p";
	FILE *f;
	int err;

	// Copies ROM dump program into a file
	f = fopen(prgname, "wb");
	if (f == NULL)
		return ERR_FILE_OPEN;
	fwrite(romDump82, sizeof(unsigned char), romDumpSize82, f);
	fclose(f);

	// Transfer program to calc
	handle->busy = 0;
	TRYF(ticalcs_calc_send_var2(handle, MODE_SEND_ONE_VAR, prgname));
	unlink(prgname);
	PAUSE(1000);

	// Wait for user's action (execing program)
	sprintf(handle->updat->text, _("Waiting execing of program..."));
	handle->updat->label();

	do
	{
		handle->updat->refresh();
		if (handle->updat->cancel)
			return ERR_ABORT;
		
		//send RDY request ???
		PAUSE(1000);
		err = rom_dump_ready(handle);
	}
	while (err == ERROR_READ_TIMEOUT);

	// Get dump
	f = fopen(filename, "wb");
	if (f == NULL)
		return ERR_OPEN_FILE;

	err = rom_dump(handle, f);
	if(err)
	{
		fclose(f);
		return err;
	}

	fclose(f);
	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* clock)
{
	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* clock)
{
	return 0;
}


static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	return 0;
}

static int		send_cert	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

static int		recv_cert	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

const CalcFncts calc_82 = 
{
	CALC_TI82,
	"TI82",
	N_("TI-82"),
	N_("TI-82"),
	OPS_SCREEN | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP |
	FTS_MEMFREE,
	&is_ready,
	&send_key,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
	&recv_idlist,
	&dump_rom,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
};
