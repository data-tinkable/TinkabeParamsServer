/*!
@file TinkableParamsServer.cpp
@author data.tinkable@gmail.com
@version 1.97
@copyright Copyright 2022 Tinkable Inc. All rights reserved.
@license Allowed only for Tinkable IoT Kit.
*/

#include "./TinkableParamsServer.h"

TinkableParamsServer::TinkableParamsServer(const unsigned int maxParams, bool endAfterConfig, bool rebootAfterConfig) {
  init(maxParams, endAfterConfig, rebootAfterConfig);
};

TinkableParamsServer::~TinkableParamsServer() {
  _end();
};


void TinkableParamsServer::init(const unsigned int maxParams, const bool endAfterConfig, const bool rebootAfterConfig) {
  _max_params = maxParams;
  _endAfterConfig = endAfterConfig;
  _rebootAfterConfig = rebootAfterConfig;
  _configDetailedStart = -1;
};

void TinkableParamsServer::_end(void) {
  _hasBegun = false;
};

void TinkableParamsServer::_begin(){
  if(_hasBegun) return;
  _hasBegun = true;
  _prevMode = WiFi.getMode();
};

void TinkableParamsServer::start(std::function<bool(void)> checkFunction) {
  char buff[20];
  snprintf_P(buff, 20, (PGM_P)F("TINK_%X"), ESP.getChipId());
  start(buff, "", checkFunction);
};

void TinkableParamsServer::start(char const *apName, char const *apPassword, std::function<bool(void)> checkFunction) {
  _begin();
  strncpy(_apName, apName, sizeof(_apName));
  strncpy(_apPassword, apPassword, sizeof(_apPassword));
  if(TK_SHOW_DEBUG_LEVEL >= TK_DEBUG) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer.start: WiFi.mode - "));
    TK_CONSOLE.print(WiFi.getMode());
    TK_CONSOLE.print(F(", apName - "));
    TK_CONSOLE.print(_apName);
    TK_CONSOLE.print(F(", apPassword - "));
    TK_CONSOLE.println(_apPassword);
  }
  _active = true;
  _abort = false;
  _saved = false;
  _start = millis();
  if (_enterApCallback != nullptr) {
    _enterApCallback(this);
  }
  startAP();
  setup();
  while(1) {
    if(checkFunction != nullptr) {
      if(TK_CONSOLE.available() > 0) {
        _start = millis();
      }
      if(checkFunction()) {
        shutdown();
        yield();
        break;
      }
    }
    if(hasTimeout() || _abort) {
      shutdown();
      yield();
      break;
    }
    else if(process()) {
      yield();
      break;
    }
  }
};

bool TinkableParamsServer::startAP() {
  bool ret = true;

  if(!WiFi.enableAP(true)) {
    if(TK_SHOW_DEBUG_LEVEL >= TK_ERROR) {
      TK_CONSOLE.println(F("[error] TinkableParamsServer.startAP: enableAP failed!"));
    }

    return false;
  }

  // WiFi.disconnect(true);
  // delay(TK_SHORT_DELAY);
  // WiFi.softAPdisconnect(true);
  // delay(TK_SHORT_DELAY);

  if(WiFi.getMode() != WIFI_AP) WiFi.mode(WIFI_AP);

  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer.startAP WiFi.mode - "));
    TK_CONSOLE.println(WiFi.getMode());
  }

  delay(TK_SHORT_DELAY);


  if(strcmp(_apPassword, "") != 0) {
    ret = WiFi.softAP(_apName, _apPassword, TK_PARAMS_SERVER_AP_CHANNEL, TK_PARAMS_SERVER_AP_HIDDEN, TK_PARAMS_SERVER_AP_CLIENTS_MAX);
  }
  else {
    ret = WiFi.softAP(_apName, "", TK_PARAMS_SERVER_AP_CHANNEL, TK_PARAMS_SERVER_AP_HIDDEN, TK_PARAMS_SERVER_AP_CLIENTS_MAX);
  }
  delay(TK_SHORT_DELAY);
  if(!ret) {
    if(TK_SHOW_DEBUG_LEVEL >= TK_ERROR) {
      TK_CONSOLE.println(F("[error] TinkableParamsServer.startAP: failed!"));
    }
  }
  else {
    if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
      TK_CONSOLE.print(F("[info] TinkableParamsServer.startAP: softAP start. WiFi.mode - "));
      TK_CONSOLE.println(WiFi.getMode());
    }
  }
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer.startAP: AP IP address is "));
    TK_CONSOLE.println(WiFi.softAPIP());
  }
  return ret;
}

void TinkableParamsServer::setup() {

  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) TK_CONSOLE.println(F("[info] TinkableParamsServer.setup: Starting paramsServer."));

  dnsServer = new DNSServer();
  server = new ESP8266WebServer(80);

  // dnsServer.reset(new DNSServer());
  // server->reset(new ESP8266WebServer(80));

  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);

  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer.setup: dns server started with ip - "));
    TK_CONSOLE.println(WiFi.softAPIP());
  }

  dnsServer->start(53, TK_PARAMS_SERVER_DNS_NAME, WiFi.softAPIP());
  server->on("/", std::bind(&TinkableParamsServer::handleRoot, this));
  server->on("/config", std::bind(&TinkableParamsServer::handleConfig, this));
  server->on("/save", std::bind(&TinkableParamsServer::handleSave, this));
  server->on("/info", std::bind(&TinkableParamsServer::handleInfo, this));
  server->on("/erase", std::bind(&TinkableParamsServer::handleErase, this));
  server->on("/exit", std::bind(&TinkableParamsServer::handleExit, this));
  server->on("/files", std::bind(&TinkableParamsServer::handleFiles, this));
  server->on("/download", std::bind(&TinkableParamsServer::handleDownload, this));
  server->on("/delete", std::bind(&TinkableParamsServer::handleDelete, this));
  server->onNotFound(std::bind(&TinkableParamsServer::handleNotFound, this));
  server->begin();

  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) TK_CONSOLE.println(F("[info] TinkableParamsServer.setup: HTTP server started."));
};

void TinkableParamsServer::setMaxParam(const unsigned int maxParams) {
  _max_params = maxParams;
};

void TinkableParamsServer::setSaveCallback(std::function<void(TinkableParamsServer*)> func) {
  _saveCallback = func;
};

void TinkableParamsServer::setEnterApCallback(std::function<void(TinkableParamsServer*)> func) {
  _enterApCallback = func;
};

void TinkableParamsServer::setShowInfoCallback(std::function<void(unsigned long int)> func) {
  _showInfoCallback = func;
};

void TinkableParamsServer::setShowConnectedCallback(std::function<void(unsigned long int)> func) {
  _showConnectedCallback = func;
};

void TinkableParamsServer::setShowShutdownCallback(std::function<void(void)> func) {
  _showShutdownCallback = func;
};

void TinkableParamsServer::setEndAfterConfig(bool endAfterConfig) {
  _endAfterConfig = endAfterConfig;
};

const bool TinkableParamsServer::getEndAfterConfig(void) const {
  return _endAfterConfig;
};

void TinkableParamsServer::setRebootAfterConfig(bool rebootAfterConfig) {
  _rebootAfterConfig = rebootAfterConfig;
};

const bool TinkableParamsServer::getRebootAfterConfig(void) const {
  return _rebootAfterConfig;
};

void TinkableParamsServer::setExtendedTimeout(const unsigned long int seconds) {
  _extendedTimeout = seconds * 1000;
};

void TinkableParamsServer::setTimeout(const unsigned long int seconds) {
  _timeout = seconds * 1000;
};

void TinkableParamsServer::setAbort(const bool abort) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer.setAbort: abort set to "));
    TK_CONSOLE.println(abort);
  }
  _abort = abort;
};

bool TinkableParamsServer::hasTimeout(){
  if(_timeout == 0 || wifi_softap_get_station_num() > 0) {

    if(millis() > (1000 + _timer2)){
      if(_showConnectedCallback != nullptr) {
        _showConnectedCallback((unsigned long int)((_start + _timeout - millis() + 100)/1000));
      }
      _timer2 = millis();
    }
    if(millis() > (5000 + _timer)) {
      if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
        TK_CONSOLE.print(F("[info] TinkableParamsServer.hasTimeout: num of clients is "));
        TK_CONSOLE.print(wifi_softap_get_station_num());
        TK_CONSOLE.print(F(". timeout: "));
        TK_CONSOLE.println((unsigned long int)((_start + _timeout - millis() + 100)/1000));
      }
      _timer = millis();
    }

    _start = millis();

    return false;
  }
  else {
    if(_accessed > _start) {
      _timeout = _extendedTimeout;
      _start = _accessed;
    }
    if(millis() > (_start + _timeout)) {
      if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) TK_CONSOLE.println(F("[info] TinkableParamsServer.hasTimeout: config portal has timed out."));
      return true;
    }

    if(millis() > (1000 + _timer2)){
      if(_showInfoCallback != nullptr) {
        _showInfoCallback((unsigned long int)((_start + _timeout - millis())/1000));
      }
      _timer2 = millis();
    }

    if(millis() > (5000 + _timer)) {
      if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
        TK_CONSOLE.print(F("[info] TinkableParamsServer.hasTimeout: paramsServer Timeout In "));
        TK_CONSOLE.print((_start + _timeout - millis())/1000);
        TK_CONSOLE.print(F(" seconds. mode - "));
        TK_CONSOLE.println(WiFi.getMode());
      }
      _timer = millis();
    }

    return false;
  }
};

bool TinkableParamsServer::process(){
    if(_saved) {
      _saved = false;
      if(_saveCallback != nullptr) {
        _saveCallback(this);
      }
      if(_endAfterConfig){
        shutdown();
        return true;
      }
      else {
        dnsServer->processNextRequest();
        server->handleClient();
        return false;
      }
    }
    else {
      dnsServer->processNextRequest();
      server->handleClient();
      return false;
    }
};

bool TinkableParamsServer::shutdown(){
  if(!_active) return false;
  server->stop();
  // server->reset();
  delete server;
  dnsServer->stop();
  delete dnsServer;
  // dnsServer.reset();
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.println(F("[info] TinkableParamsServer.shutdown: disconnect paramsServer."));
  }
  bool ret = false;
  ret = WiFi.softAPdisconnect(false);
  if(_rebootAfterConfig) {
    TK_CONSOLE.println(F("******************************************************"));
    TK_CONSOLE.println(F("  System will reboot.                                *"));
    TK_CONSOLE.println(F("******************************************************"));
    if(_showShutdownCallback != nullptr) {
      _showShutdownCallback();
    }
    yield();
    delay(TK_SHOW_REBOOT_DURATION);
    ESP.restart();
  }

  if(!ret && TK_SHOW_DEBUG_LEVEL >= TK_ERROR) {
    TK_CONSOLE.println(F("[error] TinkableParamsServer.shutdown: softAPdisconnect failed."));
  }

  delay(1000);

  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer.shutdown: restoring previous Mode - "));
    TK_CONSOLE.println(_prevMode);
  }

  if(WiFi.getMode() != _prevMode) WiFi.mode(_prevMode);
  _active = false;
  _end();
  return ret;
};

void TinkableParamsServer::setConfigDetailedStart(const int configDetailedStart) {
  _configDetailedStart = configDetailedStart;
};

const int TinkableParamsServer::getConfigDetailedStart(void) const {
  return _configDetailedStart;
};

bool TinkableParamsServer::addParameter(TinkableParam *p) {

  if(p->getID()){
    for (size_t i = 0; i < strlen(p->getID()); i++){
      if(!(isAlphaNumeric(p->getID()[i])) && !(p->getID()[i]=='_')) {
        if(TK_SHOW_DEBUG_LEVEL >= TK_ERROR) {
          TK_CONSOLE.println(F("[error] TinkableParamsServer.addParameter: parameter IDs can only contain alpha numeric chars"));
        }
        return false;
      }
    }
  }

  if(params.size() >= _max_params) {
    if(TK_SHOW_DEBUG_LEVEL >= TK_ERROR) {
      TK_CONSOLE.println(F("[error] TinkableParamsServer::addParameter: can't add. Increase TK_PARAMS_SERVER_PARAMS_MAX"));
    }
    return false;
  }
  params.push_back(p);
  return true;
};

void TinkableParamsServer::handleRoot(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleRoot: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  server->sendHeader(FPSTR(TK_PARAMS_SERVER_CONTENT_LENGTH_TEXT), FPSTR(TK_PARAMS_SERVER_INDEX_HTML_TEMPLATE_LENGTH));
  server->send(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_INDEX_HTML_TEMPLATE));
};

void TinkableParamsServer::handleConfig(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleRoot: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
    server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
    return;
  }
  bool langIsAlt = (server->hasArg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) && server->arg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) == FPSTR(TK_PARAMS_SERVER_ALTLANG_PA));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_CONFIG_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_CONFIG_TITLE));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
  server->sendContent(_apName);
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_CONFIG_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_CONFIG_TITLE));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_CONFIG_FORM_START));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_CONFIG_INPUT_LANG_ALTLANG): FPSTR(TK_PARAMS_SERVER_CONFIG_INPUT_LANG));
  if(params.size() > 0){
    char lengthBuffer[10];
    for (unsigned int i=0; i < params.size(); i++) {
      if (params[i] == nullptr || params[i]->_length == 0) {
        if(TK_SHOW_DEBUG_LEVEL >= TK_ERROR) {
          TK_CONSOLE.print(F("[error] TinkableParamsServer.handleConfig: "));
          TK_CONSOLE.print(i);
          TK_CONSOLE.println(F("th TinkableParameter has error."));
        }
        continue;
      }

      if(params[i]->getID() != nullptr && getConfigDetailedStart() != -1) {
        if((unsigned int) getConfigDetailedStart() == i) {
          server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_CONFIG_DETAILED_PRE_ALTLANG): FPSTR(TK_PARAMS_SERVER_CONFIG_DETAILED_PRE));
        }
      }

      String paramStr((char *)0);
      paramStr.reserve(300);
      if(params[i]->getCustomFullHTML() == nullptr) {
        if(params[i]->getID() != nullptr) {

          if(params[i]->getType() == TK_PARMAS_SERVER_SELECT_BOOL) {
            paramStr = (strcmp(params[i]->getValue(), "1") == 0)? FPSTR(TK_PARAMS_SERVER_CONFIG_PARAM_SELECT_TRUE) : FPSTR(TK_PARAMS_SERVER_CONFIG_PARAM_SELECT_FALSE);
          }
          else if(params[i]->getType() == TK_PARMAS_SERVER_TEXTAREA) {
            paramStr = FPSTR(TK_PARAMS_SERVER_CONFIG_PARAM_TEXTAREA);
            paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_VALUE_RE), params[i]->getValue());
          }
          else {
            paramStr = FPSTR(TK_PARAMS_SERVER_CONFIG_PARAM_INPUT_TEXT);
            paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_VALUE_RE), params[i]->getValue());
          }
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_ID_RE), params[i]->getID());
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_LABEL_RE), langIsAlt? params[i]->getLabelKo() : params[i]->getLabel());
          snprintf(lengthBuffer, 10, "%d", params[i]->getValueLength());
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_MAXLENGTH_RE),lengthBuffer);
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_PARAMS_RE), params[i]->getCustomHTML());
        }
        else {
          paramStr = params[i]->getCustomHTML();
        }
      }
      else {
        paramStr = params[i]->getCustomFullHTML();
        bool custom_has_id = paramStr.indexOf(FPSTR(TK_PARAMS_SERVER_CONFIG_ID_RE)) > 0; // "{i}"
        bool custom_has_label = paramStr.indexOf(FPSTR(TK_PARAMS_SERVER_CONFIG_LABEL_RE)) > 0; // "{i}"
        bool custom_has_maxlength = paramStr.indexOf(FPSTR(TK_PARAMS_SERVER_CONFIG_MAXLENGTH_RE)) > 0; // "{l}"
        bool custom_has_value = paramStr.indexOf(FPSTR(TK_PARAMS_SERVER_CONFIG_VALUE_RE)) > 0; // {{v}}
        bool custom_has_params = paramStr.indexOf(FPSTR(TK_PARAMS_SERVER_CONFIG_PARAMS_RE)) > 0; // {p}
        if(custom_has_id) {
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_ID_RE), params[i]->getID());
        }
        if(custom_has_label) {
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_LABEL_RE), langIsAlt? params[i]->getLabelKo() : params[i]->getLabel());
        }
        if(custom_has_maxlength) {
          snprintf(lengthBuffer, 10, "%d", params[i]->getValueLength());
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_MAXLENGTH_RE),lengthBuffer);
        }
        if(custom_has_value) {
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_VALUE_RE), params[i]->getValue());
        }
        if(custom_has_params) {
          paramStr.replace(FPSTR(TK_PARAMS_SERVER_CONFIG_PARAMS_RE), params[i]->getCustomHTML());
        }
      }
      server->sendContent(paramStr);
    }
  }
  if(_configDetailedStart != -1) {
    server->sendContent(FPSTR(TK_PARAMS_SERVER_CONFIG_DETAILED_AFTER));
  }
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_CONFIG_SAVE_BUTTON_ALTLANG) : FPSTR(TK_PARAMS_SERVER_CONFIG_SAVE_BUTTON));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_CONFIG_FORM_END));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
  server->chunkedResponseFinalize();
};

void TinkableParamsServer::handleSave(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleSave: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
    server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
    return;
  }
  bool langIsAlt = (server->hasArg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) && server->arg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) == FPSTR(TK_PARAMS_SERVER_ALTLANG_PA));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
  if(params.size() > 0) {
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_TITLE));
  }
  else {
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_SAVE_ERR_TITLE));
  }
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
  server->sendContent(_apName);
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));

  if(params.size() > 0){
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
    if(_endAfterConfig) {
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_PRE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_PRE));
    }
    else {
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_NOT_END_PRE_ALTLANG): FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_NOT_END_PRE));
    }
    server->sendContent(F("<ul>"));

    for (unsigned int i = 0; i < params.size(); i++) {
      if(params[i] != nullptr && params[i]->getID() != nullptr) {
        if(server->hasArg(params[i]->getID())) {
          strncpy(params[i]->_value, server->arg(params[i]->getID()).c_str(), params[i]->_length+1);
          server->sendContent(F("<li>"));
          server->sendContent(params[i]->getID());
          server->sendContent(F(": "));
          if(strcmp(server->arg(params[i]->getID()).c_str(), "") == 0) {
            server->sendContent(" ");
          }
          else {
            server->sendContent(server->arg(params[i]->getID()).c_str());
          }
          server->sendContent(F("</li>"));
        }
        else {
          server->sendContent(F("<li>no "));
          server->sendContent(params[i]->getID());
          server->sendContent(F(" parameter.</li>"));
        }
      }
      else {
        server->sendContent(F("<li>"));
        char sizeBuffer[12];
        snprintf(sizeBuffer, 12, "%d", (int)i);
        server->sendContent(sizeBuffer);
        server->sendContent(F("th param is nullptr.</li>"));
      }
    }
    server->sendContent(F("</ul>"));
    if(_endAfterConfig) {
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_AFTER_ALTLANG) : FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_AFTER));
    }
    else {
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_NOT_END_AFTER_ALTLANG): FPSTR(TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_NOT_END_AFTER));
    }
    _saved = true;
  }
  else {
    if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
      TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
      TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_SAVE_ERR_TITLE));
    }
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_SAVE_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_SAVE_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_SAVE_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
  }
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
  server->chunkedResponseFinalize();
};

void TinkableParamsServer::sendInfoContent(const __FlashStringHelper *title, const char *value) {
  server->sendContent(FPSTR(TK_PARAMS_SERVER_INFO_STR_PRE));
  server->sendContent(title);
  server->sendContent(FPSTR(TK_PARAMS_SERVER_INFO_STR_MID));
  if(strcmp(value, "") == 0) {
    server->sendContent(" ");
  }
  else {
    server->sendContent(value);
  }
  server->sendContent(FPSTR(TK_PARAMS_SERVER_INFO_STR_END));
};

String TinkableParamsServer::getInfoStr(const String &title, const String &value) {
  String result((char *)0);
  result.reserve(title.length() + value.length() + 9 + 11 + 13 + 1);
  result = FPSTR(TK_PARAMS_SERVER_INFO_STR_PRE); // 9
  result += title;
  result += FPSTR(TK_PARAMS_SERVER_INFO_STR_MID); // 11
  result += value;
  result += FPSTR(TK_PARAMS_SERVER_INFO_STR_END); // 13
  return result;
};

void TinkableParamsServer::handleInfo(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleInfo: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
    server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
    return;
  }
  bool langIsAlt = (server->hasArg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) && server->arg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) == FPSTR(TK_PARAMS_SERVER_ALTLANG_PA));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_INFO_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_INFO_TITLE));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
  server->sendContent(_apName);
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_INFO_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_INFO_TITLE));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
  server->sendContent(F("<div class='flex2p'>"));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_INFO_ESP_HEAD));

  char outputBuffer[32];
  snprintf_P(outputBuffer, 32, (PGM_P)F("%lums"), millis());
  sendInfoContent(F("millis"), outputBuffer);
  snprintf_P(outputBuffer, 32, (PGM_P)F("%x"), ESP.getChipId());
  sendInfoContent(F("Chipid"), outputBuffer);
  snprintf_P(outputBuffer, 32, (PGM_P)F("%lu"), ESP.getFlashChipId());
  sendInfoContent(F("FlashChipId"), outputBuffer);
  snprintf_P(outputBuffer, 32, (PGM_P)F("%lu"), ESP.getFlashChipSize());
  sendInfoContent(F("FlashChip Size"), outputBuffer);
  snprintf_P(outputBuffer, 32, (PGM_P)F("%lu"), ESP.getFlashChipRealSize());
  sendInfoContent(F("FlashChip RealSize"), outputBuffer);
  sendInfoContent(F("Sdk Ver."), system_get_sdk_version());
  sendInfoContent(F("Core Ver."), ESP.getCoreVersion().c_str());
  snprintf_P(outputBuffer, 32, (PGM_P)F("%u"), ESP.getCpuFreqMHz());
  sendInfoContent(F("Cpu Freq."), outputBuffer);
  snprintf_P(outputBuffer, 32, (PGM_P)F("%lu"), ESP.getFreeHeap());
  sendInfoContent(F("Free Heap"), outputBuffer);
  snprintf_P(outputBuffer, 32, (PGM_P)F("%lu"), ESP.getSketchSize());
  sendInfoContent(F("Used SketchSize"), outputBuffer);
  snprintf_P(outputBuffer, 32, (PGM_P)F("%lu"), ESP.getFreeSketchSpace());
  sendInfoContent(F("Free SketchSize"), outputBuffer);
  sendInfoContent(F("Reset Reason"), ((String) ESP.getResetReason()).c_str());

  server->sendContent(FPSTR(TK_PARAMS_SERVER_INFO_WIFI_HEAD));

  sendInfoContent(F("SoftAP IP"), WiFi.softAPIP().toString().c_str());
  sendInfoContent(F("SoftAP Mac"), WiFi.softAPmacAddress().c_str());
  sendInfoContent(F("Mac Address"), WiFi.macAddress().c_str());
  sendInfoContent(F("Host Name"), WiFi.hostname().c_str());
  sendInfoContent(F("AutoConnect"), WiFi.getAutoConnect()? "1" : "0");


  server->sendContent(F("</div>"));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_BUTTON_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_BUTTON));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));

  server->chunkedResponseFinalize();
};

void TinkableParamsServer::handleErase(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleErase: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
    server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
    return;
  }
  bool langIsAlt = (server->hasArg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) && server->arg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) == FPSTR(TK_PARAMS_SERVER_ALTLANG_PA));
  bool littleFSBegin = LittleFS.begin();
  bool littleFSFormat = false;
  bool espEraseConfig = false;
  if(littleFSBegin) {
    littleFSFormat = LittleFS.format();
    if(littleFSFormat) {
      espEraseConfig = ESP.eraseConfig();
      if(!espEraseConfig) {
        if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
          TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
          TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE));
        }
      }
    }
    else {
      if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
        TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
        TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE));
      }
    }
  }
  else {
    if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
      TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
      TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    }
  }
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
  if(littleFSBegin) {
    if(littleFSFormat) {
      if(espEraseConfig) {
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_SUCCESS_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_SUCCESS_TITLE));
      }
      else {
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE));
      }
    }
    else {
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE));
    }
  }
  else {
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
  }
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_INFO_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_INFO_TITLE));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
  server->sendContent(_apName);
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
  if(littleFSBegin) {
    if(littleFSFormat) {
      if(espEraseConfig) {
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_SUCCESS_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_SUCCESS_TITLE));
      }
      else {
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE));
      }
    }
    else {
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE));
    }
  }
  else {
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
  }
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
  if(littleFSBegin) {
    if(littleFSFormat) {
      if(espEraseConfig) {
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_SUCCESS_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_SUCCESS_MESSAGE));
      }
      else {
        server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
      }
    }
    else {
      server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
    }
  }
  else {
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
  }
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
  server->chunkedResponseFinalize();
};

void TinkableParamsServer::handleFiles_printOutFiles(const char *dirName, const bool langIsAlt, short int &fileCount) {
  Dir dir = LittleFS.openDir(dirName);
  while(dir.next()) {
    char nameBuffer[32];
    strncpy(nameBuffer, dirName, 32);
    strcat(nameBuffer, "/");
    strcat(nameBuffer, dir.fileName().c_str());
    if(dir.isFile()) {
      fileCount++;
      char sizeBuffer[10];
      snprintf(sizeBuffer, 10, "%lu", (unsigned long int)dir.fileSize());
      server->sendContent(F("pf('"));
      server->sendContent(nameBuffer);
      server->sendContent(F("',"));
      server->sendContent(sizeBuffer);
      server->sendContent(F(","));
      server->sendContent(langIsAlt? "1" : "0");
      server->sendContent(F(");"));
    }
    else {
      handleFiles_printOutFiles(nameBuffer, langIsAlt, fileCount);
    }
  }
}

void TinkableParamsServer::handleFiles(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleFiles: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
    server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
    return;
  }
  bool langIsAlt = (server->hasArg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) && server->arg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) == FPSTR(TK_PARAMS_SERVER_ALTLANG_PA));
  if(!LittleFS.begin()) {
    if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
      TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
      TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    }
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
    server->sendContent(_apName);
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
    server->chunkedResponseFinalize();
  }
  else {
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
    server->sendContent(_apName);
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_PRINT_OUT_FILES_SCRIPT_PRE));
    short int fileCount = 0;
    handleFiles_printOutFiles("", langIsAlt, fileCount);
    server->sendContent(FPSTR(TK_PARAMS_SERVER_PRINT_OUT_FILES_SCRIPT_AFTER));
    if(fileCount == 0) {
      server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NO_FILE_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NO_FILE_ERR_TITLE));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
    }
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
    server->chunkedResponseFinalize();
  }
};

void TinkableParamsServer::handleDelete(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleDelete: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
    server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
    return;
  }
  bool langIsAlt = (server->hasArg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) && server->arg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) == FPSTR(TK_PARAMS_SERVER_ALTLANG_PA));
  if (!LittleFS.begin()) {
    if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
      TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
      TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    }
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
    server->sendContent(_apName);
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
    server->chunkedResponseFinalize();
  }
  else {
    if(!LittleFS.exists(server->arg("file"))) {
      if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
        TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
        TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
      }
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
      server->sendContent(_apName);
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
      server->chunkedResponseFinalize();
    }
    else {
      if(LittleFS.remove(server->arg("file"))) {
        if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
          TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_INFO_LOG));
          TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE));
        }
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
        server->sendContent(_apName);
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
        server->chunkedResponseFinalize();
      }
      else {
        if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
          TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_INFO_LOG));
          TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE));
        }
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
        server->sendContent(_apName);
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
        server->chunkedResponseFinalize();
      }
    }
  }
};

void TinkableParamsServer::handleDownload(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleDownload: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  bool langIsAlt = (server->hasArg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) && server->arg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) == FPSTR(TK_PARAMS_SERVER_ALTLANG_PA));
  if(!LittleFS.begin()) {
    if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
      TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
      TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    }
    if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
      server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
      return;
    }
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
    server->sendContent(_apName);
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
    server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
    server->chunkedResponseFinalize();
  }
  else {
    if(!LittleFS.exists(server->arg("file"))) {
      if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
        TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_ERROR_LOG));
        TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
      }
      if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
        server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
        return;
      }
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
      server->sendContent(_apName);
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
      server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
      server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
      server->chunkedResponseFinalize();
    }
    else {
      File openFile = LittleFS.open(server->arg("file"), "r");
      if(!openFile) {
        if(TK_SHOW_DEBUG_LEVEL >= TK_INFO){
          TK_CONSOLE.print(FPSTR(TK_PARAMS_SERVER_INFO_LOG));
          TK_CONSOLE.println(FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE));
        }
        if (!server->chunkedResponseModeStart(200, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML))) {
          server->send(505, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML), FPSTR(TK_PARAMS_SERVER_HTTP1_1_REQUIRED));
          return;
        }
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
        server->sendContent(_apName);
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_PRE));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_ERROR_AFTER));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
        server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
        server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
        server->chunkedResponseFinalize();
      }
      else {
        char sizeBuffer[10];
        snprintf(sizeBuffer, 10, "%lu", (unsigned long int)openFile.size());
        server->sendHeader(FPSTR(TK_PARAMS_SERVER_CONTENT_LENGTH_TEXT), sizeBuffer);
        char nameBuffer[32];
        strncpy(nameBuffer, server->arg("file").c_str(), 32);
        char *found = strrchr(nameBuffer,'/');
        char contentBuffer[53];
        strncpy_P(contentBuffer, (PGM_P) F("attachment; filename="), 22);
        if(found != 0) {
          found++;
          strncat(contentBuffer, found, 32);
          server->sendHeader(FPSTR(TK_PARAMS_SERVER_CONTENT_DISPOSITION_TEXT), contentBuffer);
        }
        else {
          strncat(contentBuffer, server->arg("file").c_str(), 32);
          server->sendHeader(FPSTR(TK_PARAMS_SERVER_CONTENT_DISPOSITION_TEXT), contentBuffer);
        }
        /*
        short int folderIndex = fileName.lastIndexOf('/');
        if(folderIndex != -1) {
          fileName.remove(0, folderIndex+1);
        }
        server->sendHeader(FPSTR(TK_PARAMS_SERVER_CONTENT_LENGTH_TEXT), String(openFile.size()));
        server->sendHeader(FPSTR(TK_PARAMS_SERVER_CONTENT_DISPOSITION_TEXT), String("attachment; filename=") + fileName);
        */
        TK_CONSOLE.println(server->streamFile(openFile, FPSTR(TK_PARAMS_SERVER_STREAM_MIME_TEXT)));
        openFile.close();
      }

    }
  }
};

void TinkableParamsServer::handleNotFound(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleNotFound: url - "));
    TK_CONSOLE.println(server->uri());
  }
  server->send(404, FPSTR(TK_PARAMS_SERVER_HTML_MIME_TEXT_PLAIN), FPSTR(TK_PARAMS_SERVER_HTML_404_BODY));
};

void TinkableParamsServer::handleExit(void) {
  if(TK_SHOW_DEBUG_LEVEL >= TK_INFO) {
    TK_CONSOLE.print(F("[info] TinkableParamsServer::handleExit: url - "));
    TK_CONSOLE.println(server->uri());
  }
  _accessed = millis();
  bool langIsAlt = (server->hasArg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) && server->arg(FPSTR(TK_PARAMS_SERVER_LANG_PA)) == FPSTR(TK_PARAMS_SERVER_ALTLANG_PA));

  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_EXIT_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_EXIT_TITLE));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME));
  server->sendContent(_apName);
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_LANG_TEXT) : FPSTR(TK_PARAMS_SERVER_LANG_TEXT_ALTLANG));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_EXIT_TITLE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_EXIT_TITLE));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_MENU_ALTLANG) : FPSTR(TK_PARAMS_SERVER_MENU));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY));
  if(_rebootAfterConfig) {
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_EXIT_MESSAGE_ALTLANG) : FPSTR(TK_PARAMS_SERVER_EXIT_MESSAGE));
  }
  else {
    server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_EXIT_MESSAGE_ALTLANG_NO_REBOOT) : FPSTR(TK_PARAMS_SERVER_EXIT_MESSAGE_NO_REBOOT));
  }
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_START));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_DELETE_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE));
  server->sendContent(langIsAlt? FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG): FPSTR(TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM));
  server->sendContent(FPSTR(TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT));
  server->chunkedResponseFinalize();
  delay(500);
  _abort = true;
};

const char *TinkableParamsServer::getApName(void) const {
  return _apName;
};

const char *TinkableParamsServer::getApPassword(void) const {
  return _apPassword;
};
