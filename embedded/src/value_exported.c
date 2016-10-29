////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VALUE_EXPORTED
/// @{ 
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

// Include Libraries
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../common/values.h"
#include "value_exported.h"
#include "trace.h"

////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////

const int value_type_size[] = {1, 2, 4, 8, 4, 1};

////////////////////////////////////////////////////////////////////////////////
//                                                                     Variables
////////////////////////////////////////////////////////////////////////////////

static T_VALUE VALUES_Array[MAX_VALUES];
static uint16_t  VALUES_nbInfo;

static uint8_t ProcDesc[DESC_SIZE];

////////////////////////////////////////////////////////////////////////////////
///                                                           Fonctions internes
////////////////////////////////////////////////////////////////////////////////

static void convertValue(void *dst, void *src, T_VALUE *def)
{
    for (int i = 0; i < def->info.nb_elem_y; i++) {
        for (int j = 0; j < def->info.nb_elem_x; j++) {
            int offset = j + i * def->info.nb_elem_x;
            switch (def->info.type) {
            case type_int16:
                *((int16_t *)dst + offset) = ntohs(*((int16_t *)src + offset));
                break;
            case type_int32:
            case type_float:
                *((int32_t *)dst + offset) = ntohl(*((int32_t *)src + offset));
                break;
            case type_int64:
    #if (PROC_LITTLE_ENDIAN == true)
            {
                int32_t lsb = *((int32_t *)src + offset + 1);
                int32_t msb = *((int32_t *)src + offset);
                *((int32_t *)dst + offset) = ntohl(lsb);
                *((int32_t *)dst + offset + 1) = ntohl(msb);
            }
    #else
                *((int32_t *)dst + offset) = ntohl(*((int32_t *)src + offset));
                *((int32_t *)dst + offset + 1) = ntohl(*((int32_t *)src + offset + 1));
    #endif
                break;
            default:
                *((int8_t *)dst + offset) = *((int8_t *)src + offset);
                break;
            }
        }
    }
}

static T_VALUE * getValue(uint32_t group, uint32_t name)
{
    uint16_t i;
    T_VALUE * value = NULL;

    LOG(LOG_INFO, "Get value for %d %d", group, name);

    for (i = 0; i < VALUE_EXPORTED_nbInfo(); i++)
    {
        uint32_t idGroup = htonl(VALUES_Array[i].info.id.group);
        uint32_t idName = htonl(VALUES_Array[i].info.id.name);
        if ((idName == name) && (idGroup == group))
        {
            value = &VALUES_Array[i];
            LOG(LOG_INFO, "Found value");
        }
    }
    return value;
}

////////////////////////////////////////////////////////////////////////////////
//                                                          Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Construit la liste des fonctions exportés en fonction du materiel
////////////////////////////////////////////////////////////////////////////////
void VALUE_EXPORTED_init(uint8_t * desc)
{
    strncpy((char *)ProcDesc, (char *)desc, DESC_SIZE);
}

uint8_t *VALUE_EXPORTED_GetProcDesc(void)
{
    return ProcDesc;
}

bool VALUE_EXPORTED_add_Ptr(char group[sizeof(uint32_t)],
                            char name[sizeof(uint32_t)],
                            uint16_t nb_elem_x,
                            uint16_t nb_elem_y, void *ptr_value, bool readPermission,
                            bool writePermission, value_type_t type,
                            void *ptr_value_min,
                            void *ptr_value_max,
                            char desc[DESC_SIZE])
{
    T_VALUE value;
    memcpy(&value.info.id.group, group, sizeof(uint32_t));
    memcpy(&value.info.id.name, name, sizeof(uint32_t));
    value.info.nb_elem_x = nb_elem_x;
    value.info.nb_elem_y = nb_elem_y;
    value.ptr_value = ptr_value;
    value.ptr_value_min = ptr_value_min;
    value.ptr_value_max = ptr_value_max;
    value.info.readPermission = readPermission;
    value.info.writePermission = writePermission;
    value.info.type = type;
    memcpy(value.desc, desc, DESC_SIZE);
    value.readFunction = NULL;
    value.writeFunction = NULL;

    return VALUE_EXPORTED_add(value);
}

bool VALUE_EXPORTED_add_Fct(char group[sizeof(uint32_t)],
                            char name[sizeof(uint32_t)],
                            uint16_t nb_elem_x,
                            uint16_t nb_elem_y, T_READ_WRITE_CB readFunction,
                            T_READ_WRITE_CB writeFunction, bool readPermission,
                            bool writePermission, value_type_t type,
                            void *ptr_value_min,
                            void *ptr_value_max,
                            char desc[DESC_SIZE])
{
    T_VALUE value;
    memcpy(&value.info.id.group, group, sizeof(uint32_t));
    memcpy(&value.info.id.name, name, sizeof(uint32_t));
    value.info.nb_elem_x = nb_elem_x;
    value.info.nb_elem_y = nb_elem_y;
    value.ptr_value = NULL;
    value.ptr_value_min = ptr_value_min;
    value.ptr_value_max = ptr_value_max;
    value.info.readPermission = readPermission;
    value.info.writePermission = writePermission;
    value.info.type = type;
    memcpy(value.desc, desc, DESC_SIZE);
    value.readFunction = readFunction;
    value.writeFunction = writeFunction;

    return VALUE_EXPORTED_add(value);
}

////////////////////////////////////////////////////////////////////////////////
/// Construit la liste des fonctions exportés en fonction du materiel
////////////////////////////////////////////////////////////////////////////////
bool VALUE_EXPORTED_add(const T_VALUE value)
{
    bool ret = false;
    if (VALUES_nbInfo < MAX_VALUES)
    {
        ret = true;
        VALUES_Array[VALUES_nbInfo] = value;
        VALUES_nbInfo++;
    }
    else
    {
        LOG(LOG_INFO, "Too many values (%d)", VALUES_nbInfo);
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// Lit la donnee correspondante a l'identifiant
///
/// \param  id             Identifiant de la donnee a alire
/// \param  data           Pointeur sur lequel on doit placer les donnees lues
/// \param  size           Taille des donnees lues
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
bool VALUE_EXPORTED_read(uint32_t group, uint32_t id, uint8_t * data, uint16_t * size)
{
    T_VALUE * value = getValue(group, id);
    *size = 0;
    bool ret = false;

    LOG(LOG_INFO, "Read val %d", id);
  

    if (value != NULL)
    {
        *size = value->info.nb_elem_y * value->info.nb_elem_x * value_type_size[value->info.type];
        uint32_t buf[value->info.nb_elem_x * value->info.nb_elem_y];        // le processuer ne supporte pas les lectures / ecritures non alignées
        if (value->readFunction) {
            ret = value->readFunction((int8_t*)buf);
            convertValue(buf, buf, value);
        } else {
            ret = true;
            convertValue(buf, value->ptr_value, value);
        }
        memcpy(data, buf, *size);
    }
    else
    {
        LOG(LOG_INFO, "Read invalid val %d", id);
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// Lit la donnee correspondante a l'identifiant
///
/// \param  id             Identifiant de la donnee a alire
/// \param  data           Pointeur sur lequel on doit placer les donnees lues
/// \param  size           Taille des donnees lues
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
bool VALUE_EXPORTED_readLimit(uint32_t group, uint32_t id, uint8_t isLowLimit, uint8_t * data, uint16_t * size)
{
    T_VALUE * value = getValue(group, id);
    *size = 0;
    bool ret = false;

    if (value != NULL)
    {
        *size = value->info.nb_elem_y * value->info.nb_elem_x * value_type_size[value->info.type];
        uint32_t buf[value->info.nb_elem_x * value->info.nb_elem_y];        // le processuer ne supporte pas les lectures / ecritures non alignées
        if (isLowLimit)
        {
            if (value->ptr_value_min != NULL)
            {
                convertValue(buf, value->ptr_value_min, value);
            }
            else
            {
                *size = 0;
            }
        }
        else
        {
            if (value->ptr_value_min != NULL)
            {
                convertValue(buf, value->ptr_value_max, value);
            }
            else
            {
                *size = 0;
            }
        }

        memcpy(data, buf, *size);
        ret = true;
    }
    else
    {
        LOG(LOG_INFO, "Read invalid limit val %d", id);
    }

    LOG(LOG_INFO, "Read limit val %d (is low limit %d)", id, isLowLimit);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// Lit la description correspondante a l'identifiant
///
/// \param  id             Identifiant de la donnee a alire
/// \param  data           Pointeur sur lequel on doit placer les donnees lues
/// \param  size           Taille des donnees lues
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
bool VALUE_EXPORTED_getDesc(uint32_t group, uint32_t id, uint8_t * data, uint16_t * size)
{
    T_VALUE * value = getValue(group, id);
    *size = 0;
    bool ret = false;

    LOG(LOG_INFO, "Get desc val %d", id);


    if (value != NULL)
    {
        *size = strnlen(value->desc, sizeof(value->desc)-1) + 1;

        memcpy(data, value->desc, *size);
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// Lit la donnee correspondante a l'identifiant
///
/// \param  id             Identifiant de la donnee a lire
/// \param  data           Pointeur sur lequel on doit ecrire les donnees
/// \return                Status de retour
////////////////////////////////////////////////////////////////////////////////
bool VALUE_EXPORTED_write(uint32_t group, uint32_t name, uint8_t * data)
{

    uint32_t size;

    bool ret = false;
    T_VALUE * value = getValue(group, name);
    if (value != NULL)
    {
        size = value->info.nb_elem_x * value->info.nb_elem_y * value_type_size[value->info.type];
        uint32_t buf[value->info.nb_elem_x * value->info.nb_elem_x];        // le processuer ne supporte pas les lectures / ecritures non alignées
        memcpy(buf, data, size);

        if (value->writeFunction)
        {
            convertValue(buf, buf, value);
            ret = value->writeFunction((int8_t*)buf);
        }
        else
        {
            convertValue(value->ptr_value, buf, value);
            ret = true;
        }
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// Retourne le nombre de champs que l'on peut lire ou ecrire
///
/// \return                Nombre de champs que l'on peut lire ou ecrire
////////////////////////////////////////////////////////////////////////////////
uint16_t VALUE_EXPORTED_nbInfo(void)
{
    return VALUES_nbInfo;
}

////////////////////////////////////////////////////////////////////////////////
/// Retourne les informations de nom, taille et droit en lecture, ecriture pour un champ
///
/// \param i     Index du champ dont on veut lire les informations       
/// \return      Informations lues
////////////////////////////////////////////////////////////////////////////////
struct T_VALUE_INFO VALUE_EXPORTED_info(uint16_t i)
{
    return VALUES_Array[i].info;
}



///
/// @}
///
