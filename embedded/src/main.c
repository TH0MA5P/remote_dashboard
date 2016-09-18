#include <stdint.h>
#include <stdbool.h>

#include "../common/udpProtocol.h"
#include "../common/values.h"
#include "acq_eth_protocol.h"
#include "value_exported.h"
#include "trace.h"
#include "udp.h"


#define NB_INFO_DAC                                 4
#define NB_CAL                                      3

/// name Valeurs utiles pour le BACQ (constantes)
/// @{
static float vMaxAdc = 2.5;
static float dacDc[NB_INFO_DAC] = {1.25, 1., 0., 12};
static float dacAc[NB_INFO_DAC] = {2.5,  1., 0., 23};
static uint32_t maxGen = 25;
static float vTest = 3.1445454754;
/// @}

static float manufGain[NB_CAL] = {1.1, 1.2, 1.3};
static float manufOffset[NB_CAL]= {2.1, 2.2, 2.3};

static uint16_t array[3][2]= {{1, 2}, {3, 4}, {5, 6}};

static char * string = "string_test";

////////////////////////////////////////////////////////////////////////////////
/// Ecrit les gains de calibration de la carte CA16
/// \param  data  Pointeur sur lequel on ecrit les donnees
////////////////////////////////////////////////////////////////////////////////
static bool writeManufGain(int8_t * data)
{
    memcpy(&manufGain, data, sizeof(manufGain));
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Lit les gains de calibration de la carte CA16
/// Ces donnees n'ont pas a etre interpretes par la carte, elle sont justes lues et ecrites
/// \param  data  Pointeur sur lequel on ecrite les donnees
////////////////////////////////////////////////////////////////////////////////
static bool readManufGain(int8_t * data)
{

    memcpy(data, &manufGain, sizeof(manufGain));
    return true;
}


////////////////////////////////////////////////////////////////////////////////
/// Ecrit les offsets de calibration de la carte CA16
/// \param  data  Pointeur sur lequel on ecrit les donnees
////////////////////////////////////////////////////////////////////////////////
static bool writeManufOffset(int8_t * data)
{
    memcpy(&manufOffset, data, sizeof(manufOffset));
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Lit les offsets de calibration de la carte CA16
/// Ces donnees n'ont pas a etre interpretes par la carte, elle sont justes lues et ecrites
/// \param  data  Pointeur sur lequel on ecrite les donnees
////////////////////////////////////////////////////////////////////////////////
static bool readManufOffset(int8_t * data)
{
    memcpy(data, &manufOffset, sizeof(manufOffset));
    return true;
}

/// Tableau conetant les donnees pouvant etre lues ou ecrtites avec les commandes READ_VALU et WRITE_VALUE
static const T_VALUE VALUES_ArrayGeneric[] =
{
    // Nom
    //              nb_elem
    //                              ptr_value
    //                                             Fonction appelee a la lecture
    //                                                                Fonction appelee a l'ecriture
    //                                                                                      Droit en lecture
    //                                                                                              Droit en ecriture
    //                                                                                                     Type
    //{"TITI", "VADC",                  1, 1  ,&vMaxAdc,         NULL,              NULL,                 true,   false, type_float, "Voltage ADC"},        // Vmax ADC
};

int main(void)
{
    LOG(LOG_INFO, "Start of program");

    UDP_init(UDP_PORT);

    VALUE_EXPORTED_init("Processeur embarqué");

    //VALUE_EXPORTED_add(VALUES_ArrayGeneric[0]);

    VALUE_EXPORTED_add_Ptr("TOTO", "TGEN", 1, 1 ,&maxGen, true,   true,  type_int32, "Generation");
    VALUE_EXPORTED_add_Ptr("TOTO", "DACD", NB_INFO_DAC, 1  ,dacDc, true,   false, type_float, "Voltage direct ADC");       // Infos DAC DC
    VALUE_EXPORTED_add_Ptr("TOTO", "DACA", NB_INFO_DAC, 1  ,dacAc,   true,   false, type_float, "Voltage alternative ADC");        // Infos DAC AC
    VALUE_EXPORTED_add_Ptr("TOTO", "TGEN",  1, 1  ,&maxGen,  true,   true,  type_int32, "Generation");        // Infos DAC AC
    VALUE_EXPORTED_add_Ptr("TOTO", "TEST",  1, 1  ,&vTest,    true,   true,  type_float, "Test value !!");        // Vmax ADC
    VALUE_EXPORTED_add_Ptr("TOTO", "TAB ",  3, 2  ,&array,   true,   true,  type_int16, "Tableau");       // Vmax ADC

    VALUE_EXPORTED_add_Ptr("TOTO", "STRG",  strlen(string), 1, string,   true, true,  type_string, "string");


    VALUE_EXPORTED_add_Fct("TOTO", "MGAI", NB_CAL, 1  ,readManufGain, writeManufGain,   true,   true,  type_float, "Calibration gain");      // Gains de calibration
    VALUE_EXPORTED_add_Fct("TOTO", "MOFF",  NB_CAL, 1  ,readManufOffset, writeManufOffset,   true,   true,  type_float, "Offset gain");      // Offset de calibration



    while (1)
    {
        ACQ_ETH_PROTOCOL_run();
    }

    return 0;
}
