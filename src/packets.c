/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Li�vin
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
	This unit manages packets
*/


#include "ticalcs.h"
#include "packets.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

// We split packets into chucks to get control regularly and update statistics.
#define BLK_SIZE	1024

/*
    Send a packet from PC (host) to TI (target):
    - target [in] : a machine ID uint8_t
    - cmd [in]	  : a command ID uint8_t
    - length [in] : length of buffer
    - data [in]	  : data to send (or 0x00 if NULL)
    - int [out]	  : an error code
*/
int send_packet(CalcHandle* handle,
				uint8_t target, uint8_t cmd, uint16_t len, uint8_t* data)
{
	int i;
	uint16_t sum;
	uint32_t length = (len == 0x0000) ? 65536 : len;	//  wrap around
	uint8_t *buf = (uint8_t *)handle->priv2;			//[65536+4];
	int r, q;

	ticables_progress_reset(handle->cable);

	if(data == NULL)
	{
		// short packet (no data)
		buf[0] = target;
		buf[1] = cmd;
		buf[2] = 0x00;
		buf[3] = 0x00;

		TRYF(ticables_cable_send(handle->cable, buf, 4));
	}
	else 
	{
		// std packet (data + checksum)
		buf[0] = target;
		buf[1] = cmd;
		buf[2] = LSB(length);
		buf[3] = MSB(length);

		// copy data
		memcpy(buf+4, data, len);

		// add checksum of packet
		sum = tifiles_checksum(data, length);
		buf[len+4+0] = LSB(sum);
		buf[len+4+1] = MSB(sum);

		// compute chunks
		q = (len + 6) / BLK_SIZE;
		r = (len + 6) % BLK_SIZE;
		handle->updat->max1 = length;

		// send full chunks
		for(i = 0; i < q; i++)
		{
			TRYF(ticables_cable_send(handle->cable, &buf[i*BLK_SIZE], BLK_SIZE));
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			handle->updat->cnt1 += BLK_SIZE;
			handle->updat->pbar();
			if (handle->updat->cancel)
				return ERR_ABORT;
		}

		// send last chunk
		{
			TRYF(ticables_cable_send(handle->cable, &buf[i*BLK_SIZE], (uint16_t)r));
			handle->updat->cnt1 += 1;
			handle->updat->pbar();
			if (handle->updat->cancel)
				return ERR_ABORT;
		}
	}

	return 0;
}

static uint8_t host_ids(CalcHandle *handle)
{
  switch (handle->model) 
  {
  case CALC_TI73:
    return TI73_PC;
    break;
  case CALC_TI82:
    return TI82_PC;
    break;
  case CALC_TI83:
    return TI83_PC;
    break;
  case CALC_TI83P:
  case CALC_TI84P:
    return TI83p_PC;
    break;
  case CALC_TI85:
    return TI85_PC;
    break;
  case CALC_TI86:
    return TI86_PC;
    break;
  case CALC_TI89:
  case CALC_TI89T:
    return TI89_PC;
    break;
  case CALC_TI92:
    return TI92_PC;
    break;
  case CALC_TI92P:
    return TI92p_PC;
    break;
  case CALC_V200:
    return V200_PC;
    break;
  }
  return 0x00;
}

/*
  Receive a packet from TI (target) to PC (host):
  - host [out]	 : a machine ID uint8_t
  - cmd [out]	 : a command ID uint8_t
  - length [out] : length of buffer
  - data [out]	 : received data (depending on command)
  - int [out]	 : an error code
*/
int recv_packet(CalcHandle* handle, 
				uint8_t* host, uint8_t* cmd, uint16_t* length, uint8_t* data)
{
	int i;
	uint16_t chksum;
	uint8_t buf[4];
	int r, q;

	// Any packet has always at least 4 bytes (MID, CID, LEN)
	TRYF(ticables_cable_recv(handle->cable, buf, 4));

	*host = buf[0];
	*cmd = buf[1];
	*length = buf[2] | (buf[3] << 8);

	if(*host != host_ids(handle)) 
		return ERR_INVALID_HOST;

	if(*cmd == CMD_ERR) 
		return ERR_CHECKSUM;

	switch (*cmd) 
	{
	case CMD_VAR:	// std packet ( data + checksum)
	case CMD_XDP:
	case CMD_SKP:
	case CMD_SID:
	case CMD_REQ:
	case CMD_IND:
	case CMD_RTS:		
		// compute chunks
		q = *length / BLK_SIZE;
		r = *length % BLK_SIZE;
		handle->updat->max1 = *length;

		// recv full chunks
		for(i = 0; i < q; i++)
		{
			TRYF(ticables_cable_recv(handle->cable, &data[i*BLK_SIZE], BLK_SIZE));
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			handle->updat->cnt1 += BLK_SIZE;
			handle->updat->pbar();
			if (handle->updat->cancel)
				return ERR_ABORT;
		}

		// recv last chunk
		{
			TRYF(ticables_cable_recv(handle->cable, &data[i*BLK_SIZE], (uint16_t)(r+2)));
			handle->updat->cnt1 += 1;
			handle->updat->pbar();
			if (handle->updat->cancel)
				return ERR_ABORT;
		}

		// verify checksum
		chksum = data[*length] | (data[*length+1] << 8);
		if (chksum != tifiles_checksum(data, *length))
			return ERR_CHECKSUM;

		break;
	case CMD_CTS:	// short packet (no data)
	case CMD_ACK:
	case CMD_ERR:
	case CMD_RDY:
	case CMD_SCR:
	case CMD_RID:
	case CMD_KEY:
	case CMD_EOT:
	case CMD_CNT:
		break;
	default:
		return ERR_INVALID_CMD;
	}

	return 0;
}

/* Fill-up a 8-chars buffer with NUL chars */
void pad_buffer(char *varname, uint8_t value)
{
	int i, len = strlen(varname);

	for (i = len; i < 8; i++)
		varname[i] = value;
}
