/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * httpd.c
 *
 * Author : Adam Dunkels <adam@sics.se>
 *
 * CHANGELOG: this file has been modified by Sergio Perez Alcañiz <serpeal@upvnet.upv.es>
 *            Departamento de Informática de Sistemas y Computadores
 *            Universidad Politécnica de Valencia
 *            Valencia (Spain)
 *            Date: March 2003
 *
 */

#include "httpd.h"
#include "lwip/tcp.h"
#include "fsdata.c"
#include "main.h"
#include <string.h>
#include "phil_can_functions.h"
#include "phil_aux.h"

struct http_state
{
  char *file;
  u32_t left;
};

/*-----------------------------------------------------------------------------------*/
static void
conn_err(void *arg, err_t err)
{
  struct http_state *hs;

  hs = arg;
  mem_free(hs);
}
/*-----------------------------------------------------------------------------------*/
static void
close_conn(struct tcp_pcb *pcb, struct http_state *hs)
{

  tcp_arg(pcb, NULL);
  tcp_sent(pcb, NULL);
  tcp_recv(pcb, NULL);
  mem_free(hs);
  tcp_close(pcb);
}
/*-----------------------------------------------------------------------------------*/
static void
send_data(struct tcp_pcb *pcb, struct http_state *hs)
{
//  err_t err;
//  u16_t len;

	if (dataFromSlaveBoardReceived)
	{
		dataFromSlaveBoardReceived = 0;
		tcp_write(pcb, dataFromSlaveBoard, LENGTH_OF_RESPONSE, 0);
		tcp_output(pcb);
	}
  /* We cannot send more data than space avaliable in the send
     buffer. */
//  if (tcp_sndbuf(pcb) < hs->left)
//  {
//    len = tcp_sndbuf(pcb);
//  }
//  else
//  {
//    len = hs->left;
//  }
//	

//  err = tcp_write(pcb, hs->file, len, 0);

//  if (err == ERR_OK)
//  {
//    hs->file += len;
//    hs->left -= len;
//  }
}

/*-----------------------------------------------------------------------------------*/
static err_t
http_poll(void *arg, struct tcp_pcb *pcb)
{
  if (arg == NULL)
  {
    /*    printf("Null, close\n");*/
    tcp_close(pcb);
  }
  else
  {
    send_data(pcb, (struct http_state *)arg);
  }

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
  struct http_state *hs;

  hs = arg;

  if (hs->left > 0)
  {
    send_data(pcb, hs);
  }
  else
  {
    close_conn(pcb, hs);
  }

  return ERR_OK;
}

static err_t
http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	uint32_t i;
	uint8_t action;
	uint8_t motorID;
	uint16_t setID;
	uint8_t res;
	u16_t mes_length;
	char *data;
	struct http_state *hs;
	uint8_t test_can_mess[8];
	uint32_t CANtransferCounter;
	uint8_t* auxpointer;

  hs = arg;

	if (err == ERR_OK && p != NULL) {
		/* Inform TCP that we have taken the data. */
		mes_length = p->tot_len;
    tcp_recved(pcb, mes_length);
		
		data = p->payload;

		hs->file = data;
		hs->left = mes_length;
		
		if (!(WhatToDo(data, test_can_mess, &setID))) {
			PhilCANSend(setID, test_can_mess, 8);
		}
		GPIOB->ODR ^= GPIO_Pin_9;
		
		pbuf_free(p);
		if (err == ERR_OK && p == NULL)
		{
			close_conn(pcb, hs);
		}
	}
	return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  struct http_state *hs;

  /* Allocate memory for the structure that holds the state of the
     connection. */
  hs = mem_malloc(sizeof(struct http_state));

  if (hs == NULL)
  {
    return ERR_MEM;
  }

  /* Initialize the structure. */
  hs->file = NULL;
  hs->left = 0;

  /* Tell TCP that this is the structure we wish to be passed for our
     callbacks. */
  tcp_arg(pcb, hs);

  /* Tell TCP that we wish to be informed of incoming data by a call
     to the http_recv() function. */
  tcp_recv(pcb, http_recv);

  tcp_err(pcb, conn_err);

  tcp_poll(pcb, http_poll, 10);
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
void
httpd_init(void)
{
	struct tcp_pcb *pcb;
  pcb = tcp_new();
  tcp_bind(pcb, IP_ADDR_ANY, 80);
  pcb = tcp_listen(pcb);
  tcp_accept(pcb, http_accept);
}
/*-----------------------------------------------------------------------------------*/
int
fs_open(char *name, struct fs_file *file)
{
  struct fsdata_file_noconst *f;

  for (f = (struct fsdata_file_noconst *)FS_ROOT; f != NULL; f = (struct fsdata_file_noconst *)f->next)
  {
    if (!strcmp(name, f->name))
    {
      file->data = f->data;
      file->len = f->len;
      return 1;
    }
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/

void SendDataToComp(uint8_t *data, uint16_t len)
{
//	tcp_write(pcb, data, len, 0);
//	tcp_output(pcb);
}