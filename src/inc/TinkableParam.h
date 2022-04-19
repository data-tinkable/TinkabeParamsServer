/*!
@file TinkableParam.h
@author data.tinkable@gmail.com
@version 1.97
@copyright Copyright 2022 Tinkable Inc. All rights reserved.
@license Allowed only for Tinkable IoT Kit.
*/

#pragma once

#ifndef TinkableParam_h
#define TinkableParam_h

#include <core_version.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>

#include "../config/Config.h"

class TinkableParam {

  public:
    ~TinkableParam();

    TinkableParam(const	__FlashStringHelper *customHTML);

    TinkableParam(const	char *id, const	__FlashStringHelper *label, const	__FlashStringHelper *labelKo, const TkParamsServerFormControl type=TK_PARMAS_SERVER_INPUT_TEXT);

    TinkableParam(const	char *id, const	__FlashStringHelper *label, const	__FlashStringHelper *labelKo, const char *defaultValue, short int length, const TkParamsServerFormControl type=TK_PARMAS_SERVER_INPUT_TEXT);

    TinkableParam(const	char *id, const	__FlashStringHelper *label, const	__FlashStringHelper *labelKo, const char *defaultValue, short int length, const	__FlashStringHelper *custom, const TkParamsServerFormControl type=TK_PARMAS_SERVER_INPUT_TEXT);


    const	char *getID(void);
    const	char *getValue(void);

    const	__FlashStringHelper *getLabel(void);
    const	__FlashStringHelper *getLabelKo(void);

    short int getValueLength(void);

    const	__FlashStringHelper *getCustomHTML(void);

    TkParamsServerFormControl getType(void);

    const	__FlashStringHelper	*getCustomFullHTML(void);
    void setCustomFullHTML(const	__FlashStringHelper	*customFull);

    void setValue(const char *defaultValue, const short int length);

    const	char *_id = nullptr;

  protected:
    void init(const	char *id, const	__FlashStringHelper *label, const	__FlashStringHelper *labelKo, const	char *defaultValue, short int length, const	__FlashStringHelper *custom, const TkParamsServerFormControl type=TK_PARMAS_SERVER_INPUT_TEXT);

  private:
    const	__FlashStringHelper *_label = nullptr;
    const	__FlashStringHelper *_labelKo = nullptr;
    char *_value = nullptr;
    short int _length;
    TkParamsServerFormControl _type = TK_PARMAS_SERVER_INPUT_TEXT;
    const	__FlashStringHelper *_customHTML = nullptr;
    const	__FlashStringHelper	*_customFullHTML = nullptr;

  friend class TinkableParamsServer;
};

#endif
