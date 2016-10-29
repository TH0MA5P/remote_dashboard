////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ACQ_ETH_PROTOCOL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../common/values.h"
#include "../common/protocol.h"
#include "../common/commands.h"
#include "acq_eth_protocol.h"
#include "trace.h"
#include "udp.h"
#include "cmd.h"

////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////

typedef T_STATUS (*T_COMMAND_CB)(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply);

typedef struct {
    /// Numero de la commande
    int16_t code;
    /// Fonction de callback liee a la commande
    T_COMMAND_CB callback;
} T_ACQ_ETH_CMD_CB;

// Tableau contenant la liste des commandes ACQ_ETH a lesquelles la carte peut repondre
static const T_ACQ_ETH_CMD_CB cmdArray[] =
{
    {CODE_CMD_WHO,  CMD_who},
    {CODE_CMD_LIST_VALUES, CMD_listValues},
    {CODE_CMD_READ_VALUES, CMD_readValue},
    {CODE_CMD_WRITE_VALUES, CMD_writeValue},
    {CODE_CMD_DESC_VALUES, CMD_descValue},
    {CODE_CMD_READ_VALUES_LIMIT_LOW, CMD_readValueLimitLow},
    {CODE_CMD_READ_VALUES_LIMIT_HIGH, CMD_readValueLimitHigh},
};

////////////////////////////////////////////////////////////////////////////////
//                                                          Fonctions exportees
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// Traite la commande et envoie la reponse
/// par un service interne lorsqu'il veut envoyer la reponse a une requete.
///
/// \param pMsg      pointeur contenant le message
///
////////////////////////////////////////////////////////////////////////////////
void ACQ_ETH_PROTOCOL_run(void)
{ 
    // On calcule le corps du message
    uint16_t size;
    uint16_t i;

    struct sockaddr dest;

    uint8_t pMsgCmd[SIZE_ACQ_ETH_DATA];
    uint8_t pMsgReply[SIZE_ACQ_ETH_DATA];

    UDP_rcv(pMsgCmd, sizeof(pMsgCmd), &dest);

    LOG(LOG_INFO, "Frame received");
    
    struct T_ACQ_ETH_CMD *acqEthCmd = (struct T_ACQ_ETH_CMD *)pMsgCmd;
    struct T_ACQ_ETH_REPLY *acqEthReply = (struct T_ACQ_ETH_REPLY *)pMsgReply;
    
    size = ntohs(acqEthCmd->size);
    acqEthReply->status = NOT_AVAILABLE;
    
    for (i = 0; i < (sizeof(cmdArray)/sizeof(T_ACQ_ETH_CMD_CB)); i++)
    {
        if (cmdArray[i].code == acqEthCmd->cmd)
        {
            acqEthReply->status = cmdArray[i].callback(&size, acqEthCmd->data, acqEthReply->data);
            size+= sizeof(struct T_ACQ_ETH_REPLY) - sizeof(acqEthReply->data);
        }
    }
   
    if (acqEthReply->status != NOT_AVAILABLE)
    {
        // On garde les champ CMD a la meme valeur
		acqEthReply->cmd = acqEthCmd->cmd;

        acqEthReply->src = acqEthCmd->dst;
        acqEthReply->dst = acqEthCmd->src;
        acqEthReply->ack = 1;
		
		// Convert to good endianness
		acqEthReply->size = htons(size);
		acqEthReply->status = htons(acqEthReply->status);

        UDP_snd(pMsgReply, size, &dest);;
	}
    else
    {
        LOG(LOG_ERR, "Command not found");
    }


}

/// @}
