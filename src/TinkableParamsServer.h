/*!
@file TinkableParamsServer.h
@author data.tinkable@gmail.com
@version 1.97
@copyright Copyright 2022 Tinkable Inc. All rights reserved.
@license Allowed only for Tinkable IoT Kit.
*/

#pragma once

#include <vector>
#include <memory>

#include <core_version.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>

#include "./config/config.h"
#include "./inc/TinkableParam.h"

#ifndef TinkableParamsServer_h
#define TinkableParamsServer_h

class TinkableParamsServer {

public:

  TinkableParamsServer(const unsigned int maxParams = TK_PARAMS_SERVER_PARAMS_MAX, bool endAfterConfig = TK_PARAMS_SERVER_END_AFTER_CONFIG, bool rebootAfterConfig = TK_PARAMS_SERVER_REBOOT_AFTER_CONFIG);

  ~TinkableParamsServer(void);

  void start(std::function<bool(void)> checkFunction = nullptr);
  void start(const char *apName, const char *apPassword, std::function<bool(void)> checkFunction = nullptr);
  bool startAP(void);
  void init(const unsigned int maxParams = TK_PARAMS_SERVER_PARAMS_MAX, const bool endAfterConfig = TK_PARAMS_SERVER_END_AFTER_CONFIG, const bool rebootAfterConfig = TK_PARAMS_SERVER_REBOOT_AFTER_CONFIG);
  void setup(void);
  bool shutdown(void);
  bool process(void);
  void sendInfoContent(const __FlashStringHelper *title, const char *value);
  String getInfoStr(const String &title, const String &value);

  void setMaxParam(const unsigned int maxParams);

  void setSaveCallback(std::function<void(TinkableParamsServer*)> func);
  void setEnterApCallback(std::function<void(TinkableParamsServer*)> func);
  void setShowInfoCallback(std::function<void(unsigned long int)> func);
  void setShowConnectedCallback(std::function<void(unsigned long int)> func);
  void setShowShutdownCallback(std::function<void(void)> func);

  void setEndAfterConfig(bool endAfterConfig);
  const bool getEndAfterConfig(void) const;

  void setRebootAfterConfig(bool rebootAfterConfig);
  const bool getRebootAfterConfig(void) const;

  void setExtendedTimeout(const unsigned long int seconds);
  void setTimeout(const unsigned long int seconds);
  void setAbort(const bool abort);

  bool addParameter(TinkableParam *p);
  // TinkableParam** getParameters();
  // short int getParametersCount();
  void setConfigDetailedStart(const int configDetailedStart);
  const int getConfigDetailedStart(void) const;
  const char *getApName(void) const;
  const char *getApPassword(void) const;

  // std::unique_ptr<DNSServer> dnsServer;
  // std::unique_ptr<ESP8266WebServer> server;
  DNSServer* dnsServer;
  ESP8266WebServer* server;

  // TinkableParam** _params = nullptr;
  // short int _paramsCount = 0;
  std::vector<TinkableParam*> params;

private:
  char _apName[16];
  char _apPassword[16];
  int _configDetailedStart = -1;

  unsigned long int _timer = 0;
  unsigned long int _timer2 = 0;
  unsigned long int _start = 0;
  unsigned long int _accessed = 0;

  bool _endAfterConfig = TK_PARAMS_SERVER_END_AFTER_CONFIG;
  bool _rebootAfterConfig = TK_PARAMS_SERVER_REBOOT_AFTER_CONFIG;

  WiFiMode_t _prevMode = WIFI_OFF;

  bool _hasBegun = false;
  bool _active = false;
  bool _abort = false;
  bool _saved = false;

  unsigned int _max_params = TK_PARAMS_SERVER_PARAMS_MAX;
  // short int _initial_params = TK_PARAMS_SERVER_PARAMS_MAX;
  unsigned long int _timeout = TK_PARAMS_SERVER_TIMEOUT;
  unsigned long int _extendedTimeout = TK_PARAMS_SERVER_EXTENDED_TIMEOUT;

  std::function<void(TinkableParamsServer*)> _enterApCallback = nullptr;
  std::function<void(TinkableParamsServer*)> _saveCallback = nullptr;
  std::function<void(unsigned long int)> _showInfoCallback = nullptr;
  std::function<void(unsigned long int)> _showConnectedCallback = nullptr;
  std::function<void(void)> _showShutdownCallback = nullptr;

  void _end(void);
  void _begin(void);
  bool hasTimeout(void);

  void handleRoot(void);

  void handleConfig(void);
  void handleSave(void);
  void handleInfo(void);
  void handleErase(void);
  void handleFiles(void);
  void handleFiles_printOutFiles(const char *dirName, const bool langIsKo, short int &fileCount);
  void handleDownload(void);
  void handleDelete(void);
  void handleNotFound(void);
  void handleExit(void);

  friend class TinkableParam;
};

#endif
