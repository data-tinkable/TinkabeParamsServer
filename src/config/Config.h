/*!
@file Config.h
@author data.tinkable@gmail.com
@version 1.97
@copyright Copyright 2022 Tinkable Inc. All rights reserved.
@license Allowed only for Tinkable IoT Kit.
*/

#pragma once

#ifndef TinkableParamsServerConfig_h
#define TinkableParamsServerConfig_h

#include "./ConfigString.h"
#include "./ConfigStringAltLangKr.h"

#ifndef TK_SHOW_INFO
  #define TK_SHOW_INFO true
#endif

#ifndef TK_SHOW_DEBUG_LEVEL
  typedef enum TkShowDebugLevel {
      TK_ERROR = 2, // 2
      TK_WARN, // 3
      TK_DEBUG, // 4
      TK_INFO, // 5
      TK_NONE, // 6
  } TkShowDebugLevel;
#endif

#ifndef TK_SHOW_DEBUG_LEVEL
  #define TK_SHOW_DEBUG_LEVEL 4
#endif

#ifndef TK_CONSOLE
  #define TK_CONSOLE Serial
#endif

#ifndef TK_SHOW_REBOOT_DURATION
  #define TK_SHOW_REBOOT_DURATION 10000UL
#endif

#ifndef TK_SHORT_DELAY
  #define TK_SHORT_DELAY 50
#endif

#ifndef TK_PARAMS_SERVER_PARAMS_MAX
  #define TK_PARAMS_SERVER_PARAMS_MAX 28
#endif


#ifndef TK_PARAMS_SERVER_PARAMS_MAX
  #define TK_PARAMS_SERVER_PARAMS_MAX 28
#endif

#ifndef TK_PARAMS_SERVER_TIMEOUT
  #define TK_PARAMS_SERVER_TIMEOUT 99000UL
#endif

#ifndef TK_PARAMS_SERVER_EXTENDED_TIMEOUT
  #define TK_PARAMS_SERVER_EXTENDED_TIMEOUT 900000UL
#endif

#ifndef TK_PARAMS_SERVER_AP_CHANNEL
  #define TK_PARAMS_SERVER_AP_CHANNEL 5
#endif

#ifndef TK_PARAMS_SERVER_AP_HIDDEN
  #define TK_PARAMS_SERVER_AP_HIDDEN false
#endif

#ifndef TK_PARAMS_SERVER_AP_CLIENTS_MAX
  #define TK_PARAMS_SERVER_AP_CLIENTS_MAX 4
#endif

#ifndef TK_PARAMS_SERVER_AP_PASSWORD
  #define TK_PARAMS_SERVER_AP_PASSWORD "issmart!"
#endif

#ifndef TK_PARAMS_SERVER_END_AFTER_CONFIG
  #define TK_PARAMS_SERVER_END_AFTER_CONFIG true
#endif

#ifndef TK_PARAMS_SERVER_REBOOT_AFTER_CONFIG
  #define TK_PARAMS_SERVER_REBOOT_AFTER_CONFIG true
#endif

#ifndef TK_PARAMS_SERVER_DNS_NAME
  #define TK_PARAMS_SERVER_DNS_NAME "con.tinkable.net"
#endif

enum TkParamsServerFormControl {
  TK_PARMAS_SERVER_CUSTOM = 1,
  TK_PARMAS_SERVER_INPUT_TEXT,
  TK_PARMAS_SERVER_SELECT_BOOL,
  TK_PARMAS_SERVER_TEXTAREA
};

#endif
