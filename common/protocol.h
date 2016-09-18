///////////////////////////////////////////////////////////////////////////////
/// @addtogroup PROTOCOL
/// @{ 
/// \brief 
////////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_H
#define PROTOCOL_H

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/// Taille maximale de donne dans le protocole
#define SIZE_ACQ_ETH_DATA                    1000

/// Definition du format d'un message de commande
struct T_ACQ_ETH_CMD {
    uint16_t src;
    uint16_t dst;
    uint8_t  cmd;
    uint8_t  ack;
    uint16_t size;
    uint8_t  data[SIZE_ACQ_ETH_DATA]; ///< donnees du message (traitees par le service destinataire
} __attribute__((packed));

/// Definition du format d'un message d'acquittement
struct T_ACQ_ETH_REPLY {
    uint16_t src;
    uint16_t dst;
    uint8_t  cmd;
    uint8_t  ack;
    uint16_t size;
    int16_t status;
    uint8_t  data[SIZE_ACQ_ETH_DATA]; ///< donnees du message (traitees par le service destinataire
} __attribute__((packed));

/// Status de retour d'une commande
typedef enum {
    NOT_RETURN_ACQUITMENT = 1,
    OK = 0,
    UNIMPLMENTED = -1,
    NOT_AVAILABLE = -3,
    INVALID_PARAMETERS = -5,
    FAILED_TO_EXECUTE = -6,
} T_STATUS;

#ifdef __cplusplus
}
#endif

#endif // PROTOCOL_H
