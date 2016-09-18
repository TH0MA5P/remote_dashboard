///////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMMANDES
/// @{ Commande du protocole ACQ_ETH, protocole de controle des cartes ACQ_ETH sur Ethernet.
/// cf 6455D03 - Protocole ACQ_ETH.doc. Ce protocole est base sur IP (v4) et UDP.
/// @addtogroup VALUES
/// @{ Valeurs exportes en lecture et/ou ecriture vers le serveur.
/// @see VALUE_EXPORTED Contient l'ensemble des valeurs exportees et leurs fonction de lecture ecriture respectives.
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : CMD_H_ doit etre unique
#ifndef CMD_H_
#define CMD_H_

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
#include "acq_eth_protocol.h"


////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                           Variables exportees
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

T_STATUS CMD_writeValue(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply);

T_STATUS CMD_readValue(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply);

T_STATUS CMD_who(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply);

T_STATUS CMD_descValue(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply);

T_STATUS CMD_listValues(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply);


#endif /*WRITE_VALUE_H_*/

///
/// @}
/// @}
/// @}
///
