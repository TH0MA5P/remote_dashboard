///////////////////////////////////////////////////////////////////////////////
/// @addtogroup VALUES
/// @{ 
/// \brief 
////////////////////////////////////////////////////////////////////////////////

#ifndef VALUES_H
#define VALUES_H

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/// Nombre maximum de champs exportés
#define MAX_VALUES      64

/// Taille maximum de la chaine de description
#define DESC_SIZE       32

/// Type des champs exportés
typedef enum {
        type_int8 = 0,
        type_int16,
        type_int32,
        type_int64,
        type_float,
        type_string
} value_type_t;


/// Structure correspondante a l'a trame'identification d'une variable
struct T_VALUE_ID
{
    /// Groupe sur 4 octets
    uint32_t group;
    /// Nom sur 4 octets
    uint32_t name;
}__attribute__((packed));

/// Structure correspondante a l'information d'une variable
struct T_VALUE_INFO
{
    struct T_VALUE_ID id;
    /// Taille en octet
    uint16_t nb_elem_x;
    /// Taille en octet
    uint16_t nb_elem_y;
    /// true si la lecture est permise
    uint8_t readPermission;
    /// true si l'ecriture est permise
    uint8_t writePermission;
    /// Type of value @see value_type_t
    uint8_t type;
} __attribute__((packed));

#ifdef __cplusplus
}
#endif

#endif // VALUES_H
