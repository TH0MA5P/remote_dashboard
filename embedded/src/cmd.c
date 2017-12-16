////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CMD
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

// Include libraries
#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>

// Include applicatif
#include "../common/values.h"
#include "../common/trace.h"
#include "value_exported.h"
#include "../common/protocol.h"
#include "../common/commands.h"
#include "cmd.h"

////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Traitement de la requete WRITE_VALUE
///
/// \param  ptrDataCmd     Pointeur sur lequel on doit lire la commande les donnees
/// \param  ptrDataReply   Pointeur sur lequel on doit ecrire les donnees
/// \param  sizeData       Taille du message
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
T_STATUS CMD_writeValue(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply)
{
    struct T_WRITE_VALUE writeValue;
    T_STATUS res = OK;
    bool writeSucceded;

    LOG(LOG_INFO, "Write value of size %d", *sizeData);
    
    memcpy(&writeValue, ptrDataCmd, *sizeData);
    
    writeSucceded = VALUE_EXPORTED_write(ntohl(writeValue.id.group), ntohl(writeValue.id.name), writeValue.data);
 
    *sizeData = 0;  // Acquitement sans commande
    if (writeSucceded == true)
    {
        res = OK;
    }
    else
    {
        res = FAILED_TO_EXECUTE;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////
/// Traitement de la requete READ_VALUE
///
/// \param  ptrDataCmd     Pointeur sur lequel on doit lire la commande les donnees
/// \param  ptrDataReply   Pointeur sur lequel on doit ecrire les donnees
/// \param  sizeData       Taille du message
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
T_STATUS CMD_readValue(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply)
{
    T_STATUS res = OK;
    struct T_READ_VALUE readValue;
    bool readSucceded;

    memcpy(&readValue, ptrDataCmd, *sizeData);

    readSucceded = VALUE_EXPORTED_read(ntohl(readValue.id.group), ntohl(readValue.id.name), ptrDataReply, sizeData);

    return res;
}

////////////////////////////////////////////////////////////////////////////////
/// Traitement de la requete READ_VALUE_LIMIT
///
/// \param  ptrDataCmd     Pointeur sur lequel on doit lire la commande les donnees
/// \param  ptrDataReply   Pointeur sur lequel on doit ecrire les donnees
/// \param  sizeData       Taille du message
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
T_STATUS CMD_readValueLimitLow(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply)
{
    T_STATUS res = OK;
    struct T_READ_VALUE readValueLimit;
    bool readSucceded;

    memcpy(&readValueLimit, ptrDataCmd, *sizeData);

    readSucceded = VALUE_EXPORTED_readLimit(ntohl(readValueLimit.id.group), ntohl(readValueLimit.id.name), true, ptrDataReply, sizeData);

    return res;
}

////////////////////////////////////////////////////////////////////////////////
/// Traitement de la requete READ_VALUE_LIMIT
///
/// \param  ptrDataCmd     Pointeur sur lequel on doit lire la commande les donnees
/// \param  ptrDataReply   Pointeur sur lequel on doit ecrire les donnees
/// \param  sizeData       Taille du message
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
T_STATUS CMD_readValueLimitHigh(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply)
{
    T_STATUS res = OK;
    struct T_READ_VALUE readValueLimit;
    bool readSucceded;

    memcpy(&readValueLimit, ptrDataCmd, *sizeData);

    readSucceded = VALUE_EXPORTED_readLimit(ntohl(readValueLimit.id.group), ntohl(readValueLimit.id.name), false, ptrDataReply, sizeData);

    return res;
}

////////////////////////////////////////////////////////////////////////////////
/// Envoie la commande a la requete WHO
///
/// \param  ptrDataCmd   pointeur sur lequel on doit lire la commande les donnees
/// \param  ptrDataReply   pointeur sur lequel on doit ecrire les donnees
/// \param  sizeData  taille du message
/// \return           Status de retour
////////////////////////////////////////////////////////////////////////////////
T_STATUS CMD_who(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply)
{
    struct T_WHO whoAcq;
    *sizeData = sizeof(whoAcq);

    LOG(LOG_INFO, "Who received");

    strncpy((char *)whoAcq.desc, (char *)VALUE_EXPORTED_GetProcDesc(), DESC_SIZE);

    memcpy(ptrDataReply, &whoAcq, *sizeData);    // Copie les donnees dans le pointeur

    return OK;
}

////////////////////////////////////////////////////////////////////////////////
/// Traitement de la requete READ_VALUE
///
/// \param  ptrDataCmd     Pointeur sur lequel on doit lire la commande les donnees
/// \param  ptrDataReply   Pointeur sur lequel on doit ecrire les donnees
/// \param  sizeData       Taille du message
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
T_STATUS CMD_descValue(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply)
{
    T_STATUS res = OK;
    struct T_GET_DESC_VALUE getDescValue;

    memcpy(&getDescValue, ptrDataCmd, *sizeData);

    VALUE_EXPORTED_getDesc(ntohl(getDescValue.id.group), ntohl(getDescValue.id.name), ptrDataReply, sizeData);

    return res;
}

////////////////////////////////////////////////////////////////////////////////
/// Traitement de la requete LIST_VALUE
///
/// \param  ptrDataCmd     Pointeur sur lequel on doit lire la commande les donnees
/// \param  ptrDataReply   Pointeur sur lequel on doit ecrire les donnees
/// \param  sizeData       Taille du message
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
T_STATUS CMD_listValues(uint16_t * sizeData, uint8_t * ptrDataCmd, uint8_t * ptrDataReply)
{
    LOG(LOG_INFO, "List value cmd received");
    struct T_LIST_VALUE *listValue = (struct T_LIST_VALUE *)ptrDataReply;
    uint16_t i;
    uint16_t nbValues;
    struct T_VALUE_INFO valueInfo;

    nbValues = VALUE_EXPORTED_nbInfo();
    listValue->nbValues = htons(nbValues);

    LOG(LOG_INFO, "List value %d", nbValues);

    for (i = 0; i < nbValues; i++)
    {
        valueInfo = VALUE_EXPORTED_info(i);

        // Conversion big endian
        listValue->valueInfo[i].id = valueInfo.id;
        listValue->valueInfo[i].nb_elem_x = htons(valueInfo.nb_elem_x);
        listValue->valueInfo[i].nb_elem_y = htons(valueInfo.nb_elem_y);
        listValue->valueInfo[i].readPermission = valueInfo.readPermission;
        listValue->valueInfo[i].writePermission = valueInfo.writePermission;
        listValue->valueInfo[i].type = valueInfo.type;

    }

    *sizeData = sizeof(listValue->nbValues) + nbValues*sizeof(struct T_VALUE_INFO);

    return OK;
}

////////////////////////////////////////////////////////////////////////////////
//                                                            Fonctions internes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/interne-logiciel_acquisition/TRUNK/src/EMBEDDED/BACQ/FIRMWARE/ACQ_ETH_CMD/WRITE_VALUE.c $
////////////////////////////////////////////////////////////////////////////////

/// @}
