///////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMMANDS
/// @{ 
/// \brief 
////////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDS_H
#define COMMANDS_H

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/// @name Codes des commandes
/// @{
#define CODE_CMD_WHO		0
#define CODE_CMD_LIST_VALUES	10
#define CODE_CMD_READ_VALUES	20
#define CODE_CMD_WRITE_VALUES	30
#define CODE_CMD_DESC_VALUES	40
/// @}

/// Structure correspondante a la trame de découverte
struct T_WHO
{
    uint8_t desc[DESC_SIZE];
} __attribute__((packed));

/// Structure correspondante a la trame d'écriture
struct T_WRITE_VALUE
{
    struct T_VALUE_ID id;
    uint8_t data[SIZE_ACQ_ETH_DATA - sizeof(struct T_VALUE_ID)];
} __attribute__((packed));

/// Structure correspondante a la trame de lecture
struct T_READ_VALUE
{
    struct T_VALUE_ID id;
} __attribute__((packed));

/// Structure correspondante a la trame de description
struct T_GET_DESC_VALUE
{
    struct T_VALUE_ID id;
} __attribute__((packed));

/// Structure correspondante a la trame de liste
struct T_LIST_VALUE
{
    uint16_t nbValues;
    struct T_VALUE_INFO valueInfo[MAX_VALUES];
} __attribute__((packed));

#ifdef __cplusplus
}
#endif

#endif // COMMANDS_H
