/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2003  Romain Lievin
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
  This unit handles TI92 commands.
*/

#include <stdio.h>
#include <string.h>

#include "headers.h"
#include "packets.h"
#include "calc_err.h"
#include "update.h"


int ti92_send_VAR(uint32_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[32];
  uint8_t trans[9];

  tifiles_translate_varname(varname, trans, vartype);
  DISPLAY(" PC->TI: VAR (size=0x%08X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, trans);

  buffer[0] = LSB(LSW(varsize));
  buffer[1] = MSB(LSW(varsize));
  buffer[2] = LSB(MSW(varsize));
  buffer[3] = MSB(MSW(varsize));
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));

  TRYF(send_packet(PC_TI92, CMD_VAR, 6 + strlen(varname), buffer));

  return 0;
}

int ti92_send_CTS(void)
{
  DISPLAY(" PC->TI: CTS\n");
  TRYF(send_packet(PC_TI92, CMD_CTS, 2, NULL));

  return 0;
}

int ti92_send_XDP(uint32_t length, uint8_t * data)
{
  DISPLAY(" PC->TI: XDP (0x%04X = %i bytes)\n", length, length);
  TRYF(send_packet(PC_TI92, CMD_XDP, length, data));

  return 0;
}

/*
  Skip variable
  - rej_code [in]: a rejection code
  - int [out]: an error code
 */
int ti92_send_SKIP(uint8_t rej_code)
{
  DISPLAY(" PC->TI: SKIP");
  TRYF(send_packet(PC_TI92, CMD_SKIP, 1, &rej_code));
  DISPLAY(" (rejection code = %i)\n", rej_code);

  return 0;
}

int ti92_send_ACK(void)
{
  DISPLAY(" PC->TI: ACK\n");
  TRYF(send_packet(PC_TI92, CMD_ACK, 2, NULL));

  return 0;
}

int ti92_send_ERR(void)
{
  DISPLAY(" PC->TI: ERR\n");
  TRYF(send_packet(PC_TI92, CMD_ERR, 2, NULL));

  return 0;
}

int ti92_send_RDY(void)
{
  DISPLAY(" PC->TI: RDY?\n");
  TRYF(send_packet(PC_TI92, CMD_RDY, 2, NULL));

  return 0;
}

int ti92_send_SCR(void)
{
  DISPLAY(" PC->TI: SCR\n");
  TRYF(send_packet(PC_TI92, CMD_SCR, 2, NULL));

  return 0;
}

int ti92_send_CONT(void)
{
  DISPLAY(" PC->TI: CONT\n");
  TRYF(send_packet(PC_TI92, CMD_CONT, 2, NULL));

  return 0;
}

#include "externs.h"

int ti92_send_KEY(uint16_t scancode)
{
  DISPLAY(" PC->TI: KEY\n");
  TRYF(cable->put(PC_TI92));
  TRYF(cable->put(CMD_KEY));
  TRYF(cable->put(LSB(scancode)));
  TRYF(cable->put(MSB(scancode)));

  return 0;
}

int ti92_send_EOT(void)
{
  DISPLAY(" PC->TI: EOT\n");
  TRYF(send_packet(PC_TI92, CMD_EOT, 2, NULL));

  return 0;
}

int ti92_send_REQ(uint32_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[32] = { 0 };

  DISPLAY(" PC->TI: REQ (size=0x%08X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, varname);

  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));

  TRYF(send_packet(PC_TI92, CMD_REQ, 6 + strlen(varname), buffer));

  return 0;
}

int ti92_send_RTS(uint32_t varsize, uint8_t vartype, char *varname)
{
  uint8_t buffer[32] = { 0 };

  DISPLAY(" PC->TI: REQ (size=0x%08X=%i, id=%02X, name=<%s>)\n",
	  varsize, varsize, vartype, varname);

  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  buffer[4] = vartype;
  buffer[5] = strlen(varname);
  memcpy(buffer + 6, varname, strlen(varname));

  TRYF(send_packet(PC_TI92, CMD_RTS, 6 + strlen(varname), buffer));

  return 0;
}


/* Variable (std var header: variable length) */
int ti92_recv_VAR(uint32_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t buffer[32] = { 0 };
  uint16_t length;
  uint8_t strl;

  DISPLAY(" TI->PC: VAR");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;

  *varsize = buffer[0] | (buffer[1] << 8) |
      (buffer[2] << 16) | (buffer[3] << 24);
  *vartype = buffer[4];
  strl = buffer[5];
  memcpy(varname, buffer + 6, strl);
  varname[strl] = '\0';

  if (length != (6 + strlen(varname)))
    return ERR_INVALID_PACKET;

  DISPLAY(" (size=0x%08X=%i, id=%02X, name=<%s>)",
	  *varsize, *varsize, *vartype, varname);
  DISPLAY(".\n");

  return 0;
}

int ti92_recv_CTS(void)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[5];

  DISPLAY(" TI->PC: CTS");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  else if (cmd != CMD_CTS)
    return ERR_INVALID_CMD;
  if (length != 0x0000)
    return ERR_CTS_ERROR;
  DISPLAY(".\n");

  return 0;
}

int ti92_recv_SKIP(uint8_t * rej_code)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[5];
  *rej_code = 0;

  DISPLAY(" TI->PC: SKIP");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_CTS) {
    DISPLAY("CTS");
    return 0;
  }
  if (cmd != CMD_SKIP)
    return ERR_INVALID_CMD;
  DISPLAY(" (rejection code = %i)", *rej_code = buffer[0]);
  DISPLAY(".\n");

  return 0;
}

int ti92_recv_XDP(uint32_t * length, uint8_t * data)
{
  uint8_t host, cmd;
  int err;

  DISPLAY(" TI->PC: XDP");
  err = recv_packet(&host, &cmd, (uint16_t *) length, data);
  fixup(*length);
  if (cmd != CMD_XDP)
    return ERR_INVALID_CMD;
  DISPLAY(" (%04X=%i bytes)", *length, *length);
  DISPLAY(".\n");
  TRYF(err);

  return 0;
}

/*
  Receive acknowledge
  - status [in/out]: if NULL is passed, the function checks that 00 00 has
  been received. Otherwise, it put in status the received value.
  - int [out]: an error code
*/
int ti92_recv_ACK(uint16_t * status)
{
  uint8_t host, cmd;
  uint16_t length;
  uint8_t buffer[5];

  DISPLAY(" TI->PC: ACK");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (status != NULL)
    *status = length;
  else if (length != 0x0000)
    return ERR_NACK;
  if (cmd != CMD_ACK)
    return ERR_INVALID_CMD;

  DISPLAY(".\n");

  return 0;
}

int ti92_recv_CONT(void)
{
  uint8_t host, cmd;
  uint16_t sts;


  DISPLAY(" TI->PC: CONT");
  TRYF(recv_packet(&host, &cmd, &sts, NULL));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd != CMD_CONT)
    return ERR_INVALID_CMD;
  DISPLAY(".\n");

  return 0;
}

int ti92_recv_EOT(void)
{
  uint8_t host, cmd;
  uint16_t length;

  DISPLAY(" TI->PC: EOT");
  TRYF(recv_packet(&host, &cmd, &length, NULL));
  if (cmd != CMD_EOT)
    return ERR_INVALID_CMD;
  DISPLAY(".\n");

  return 0;
}

int ti92_recv_RTS(uint32_t * varsize, uint8_t * vartype, char *varname)
{
  uint8_t host, cmd;
  uint8_t buffer[32] = { 0 };
  uint16_t length;
  uint8_t strl;

  DISPLAY(" TI->PC: VAR");
  TRYF(recv_packet(&host, &cmd, &length, buffer));
  if (cmd == CMD_EOT)
    return ERR_EOT;		// not really an error
  if (cmd == CMD_SKIP)
    return ERR_VAR_REJECTED;
  if (cmd != CMD_VAR)
    return ERR_INVALID_CMD;

  *varsize = buffer[0] | (buffer[1] << 8) |
      (buffer[2] << 16) | (buffer[3] << 24);
  *vartype = buffer[4];
  strl = buffer[5];
  memcpy(varname, buffer + 6, strl);
  varname[strl] = '\0';

  if (length != (6 + strlen(varname)))
    return ERR_INVALID_PACKET;

  DISPLAY(" (size=0x%08X=%i, id=%02X, name=<%s>)",
	  *varsize, *varsize, *vartype, varname);
  DISPLAY(".\n");

  return 0;
}