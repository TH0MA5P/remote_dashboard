////////////////////////////////////////////////////////////////////////////////
/// @addtogroup UDP
/// @{
/// @file Fichier C
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                     Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "trace.h"

////////////////////////////////////////////////////////////////////////////////
//                                                     Defines & Internal types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                           Internal variables
////////////////////////////////////////////////////////////////////////////////

static int s;
static uint slen = sizeof(struct sockaddr_in);

////////////////////////////////////////////////////////////////////////////////
//                                                           Internal functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                           Exported functions
////////////////////////////////////////////////////////////////////////////////

void UDP_init(uint16_t port)
{
    struct sockaddr_in si_me;
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
		LOG(LOG_ERR, "socket error");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        printf("err = %d", errno);
		LOG(LOG_ERR, "bind error");
    }

    LOG(LOG_INFO, "socket open");
}

void UDP_snd(uint8_t * buf, uint16_t len, struct sockaddr * dest)
{
	LOG(LOG_INFO, "send frame of size %d", len);
    if (len > 0)
    {
        //now reply the client with the same data
        if (sendto(s, buf, len, 0, dest, slen) == -1)
        {
			LOG(LOG_ERR, "sendto() error");
        }
    }
}

int UDP_rcv(uint8_t * buf, uint16_t maxLen, struct sockaddr * src)
{
    //try to receive some data, this is a blocking call
    int len = recvfrom(s, buf, maxLen, 0, src, &slen);
    if (len == -1)
    {
		LOG(LOG_ERR, "error recvfrom()");
    }
    return len;
}

///
/// @}
///
