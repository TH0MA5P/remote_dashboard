////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ACQ_ETH
/// @{ 
/// \brief Gestion des trames de commandes issues du serveur et l'envoie de l'acquitement correspondant.
/// @addtogroup ACQ_ETH_UDP
/// @{ 
/// \brief Interfaces du module de gestion de la liaison UDP vers le serveur
/// \see ETH_CONFIG Pour la gestion des adresse IP et MAC
//////////////////////////////////////////////////////////////////////////////// 


#ifndef ACQ_ETH_UDP_H
#define ACQ_ETH_UDP_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// Uniquement des includes projet : pas d'inclusion de fichier standard
#include "acq_eth_task.h"

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                           Variables exportees
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

PUBLIC bool ACQ_ETH_UDP_Init(void);

PUBLIC void ACQ_ETH_UDP_Send(T_ACQ_ETH_BUFF *pMntBuff);
PUBLIC void ACQ_ETH_UDP_SendEmpty(uint8_t * ip);

#ifdef __cplusplus
}
#endif


#endif // ETH_UDP_H


///
/// @}
/// @}
///
