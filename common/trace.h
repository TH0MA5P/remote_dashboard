////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TRACE
/// @{
/// @brief Le module TRACE permet d'afficher des logs en couleurs et de les
/// activer et désactiver facilement.
/// @file Fichier H
////////////////////////////////////////////////////////////////////////////////

#ifndef TRACE_H
#define TRACE_H

////////////////////////////////////////////////////////////////////////////////
//                                                                     Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//                                                              Exported types
////////////////////////////////////////////////////////////////////////////////

/// Manage colors
#define STOP_COLOR	"\033[0m"
#define	LOG_CRIT	"37;41"
#define	LOG_ERR		"31"
#define	LOG_WARNING	"33"
#define	LOG_NOTICE	"32"
#define	LOG_INFO	"36"
#define	LOG_DEBUG	"37"
#define FORMATCOLOR(prio) "\033[" prio "m"

/// Get file name without path
#define FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

//#define ENABLE_LOG

/// Print log
#ifdef ENABLE_LOG
#define LOG(prio, format, args...) \
	do { \
		time_t date = time(NULL); \
		struct tm ht; /* human readable time */ \
		localtime_r(&date, &ht); \
                printf(FORMATCOLOR(prio) "%02d:%02d:%02d  %s:%s(%d): " \
		STOP_COLOR format"\n", ht.tm_hour, ht.tm_min, ht.tm_sec, \
		FILE, __func__, __LINE__, ##args); \
	} while (0)
#else
#define LOG(prio, format, args...)	// do nothing
#endif

////////////////////////////////////////////////////////////////////////////////
//                                                           Exported functions
////////////////////////////////////////////////////////////////////////////////



#endif

///
/// @}
///

