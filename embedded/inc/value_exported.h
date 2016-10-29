///////////////////////////////////////////////////////////////////////////////
/// @addtogroup VALUE_EXPORTED
/// @{ 
/// \brief Ce fichier contient le tableau des donnees exportes ainsi que les pointeurs
/// de fonction correspondants.
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : VALUE_H doit etre unique
#ifndef VALUE_EXPORTED_H_
#define VALUE_EXPORTED_H_

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

/// Pointeur de fonction d'ecriture ou de lecture d'une valeur
typedef bool (*T_READ_WRITE_CB)(int8_t *data);

typedef struct {
    struct T_VALUE_INFO info;
    /// Pointeur vers la donnee a  lire ecrire (si directement accessible)
    void *ptr_value;
    /// Pointeur vers la donnee a  lire
    void *ptr_value_min;
    /// Pointeur vers la donnee a  lire
    void *ptr_value_max;
    /// Pointeur sur la fonction a appeller en cas de lecture (si ptr_value NULL)
    T_READ_WRITE_CB readFunction;
    /// Pointeur sur la fonction a appeller en cas d'ecriture (si ptr_value NULL)
    T_READ_WRITE_CB writeFunction;
    /// Description of the filed
    char desc[DESC_SIZE];
} T_VALUE;

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

bool VALUE_EXPORTED_read(uint32_t group,  uint32_t id, uint8_t * data, uint16_t * size);
bool VALUE_EXPORTED_getDesc(uint32_t group, uint32_t id, uint8_t * data, uint16_t * size);
bool VALUE_EXPORTED_write(uint32_t group, uint32_t id, uint8_t * data);
bool VALUE_EXPORTED_readLimit(uint32_t group, uint32_t id, uint8_t isLowLimit, uint8_t * data, uint16_t * size);
uint16_t VALUE_EXPORTED_nbInfo(void);
struct T_VALUE_INFO VALUE_EXPORTED_info(uint16_t i);
void VALUE_EXPORTED_init(uint8_t *desc);
uint8_t * VALUE_EXPORTED_GetProcDesc(void);

bool VALUE_EXPORTED_add(const T_VALUE value);
bool VALUE_EXPORTED_add_Ptr(char group[sizeof(uint32_t)],
                            char name[sizeof(uint32_t)],
                            uint16_t nb_elem_x,
                            uint16_t nb_elem_y, void *ptr_value, bool readPermission,
                            bool writePermission, value_type_t type,
                            void *ptr_value_min,
                            void *ptr_value_max,
                            char desc[DESC_SIZE]);

bool VALUE_EXPORTED_add_Fct(char group[sizeof(uint32_t)],
                            char name[sizeof(uint32_t)],
                            uint16_t nb_elem_x,
                            uint16_t nb_elem_y, T_READ_WRITE_CB readFunction,
                            T_READ_WRITE_CB writeFunction, bool readPermission,
                            bool writePermission, value_type_t type,
                            void *ptr_value_min,
                            void *ptr_value_max,
                            char desc[DESC_SIZE]);


#endif /*VALUE_H_*/

///
/// @}
///
