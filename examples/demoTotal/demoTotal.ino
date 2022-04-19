/*!
@file demoTotal.ino
@author data.tinkable@gmail.com
@version 1.97
@copyright Copyright 2022 Tinkable Inc. All rights reserved.
@license Allowed only for Tinkable IoT Kit.
*/

#include <TinkableParamsServer.h>

TinkableParamsServer *paramsServer = nullptr;

TinkableParam *_param_text = nullptr;
TinkableParam *_param_number = nullptr;
TinkableParam *_param_bool = nullptr;
TinkableParam *_param_textarea = nullptr;
TinkableParam *_param_custom = nullptr;

void TkParamsServerShowInfoCallback(const unsigned long sec) {
  char buffer[128];
  snprintf_P(buffer, 128, (PGM_P)F("[info] connect WiFi %s with your PC. Password is [%s] without []. timeout is %lu."), paramsServer->getApName(), paramsServer->getApPassword(), sec);
  Serial.println(buffer);
};

void TkParamsServerShowConnectedCallback(const unsigned long sec) {
  char buffer[128];
  snprintf_P(buffer, 128, (PGM_P)F("[info] Connected %s. Visit con.tinkable.net with browser. timeout is %lu."), paramsServer->getApName(), sec);
  Serial.println(buffer);
};

void TkParamsServerShowShutdownCallback(void) {
  Serial.println(F("[info] paramsServer Shutdown. Reboot Now."));
};

void TkParamsServerEnteringConfigModeCallback(const TinkableParamsServer *server) {
  char buffer[180];
  snprintf_P(buffer, 180, (PGM_P)F("[info] paramsServer started. ssid: %s, password: %s, IP Address: %s."), server->getApName(),  server->getApPassword(), WiFi.softAPIP().toString().c_str());
  Serial.println(buffer);
};

void TkParamsServerSaveConfigCallback(TinkableParamsServer *server) {
  Serial.println(F("[info] paramsServer Result."));
  for(short int i=0;i<server->params.size();i++) {
    Serial.print("\t");
    Serial.print(server->params[i]->getID());
    Serial.print(": ");
    Serial.print(server->params[i]->getValue());
    Serial.println("");
  }
};


void setup() {

  TK_CONSOLE.begin(9600);

  paramsServer = new TinkableParamsServer(5, true, true);
  paramsServer->setSaveCallback(TkParamsServerSaveConfigCallback);
  paramsServer->setEnterApCallback(TkParamsServerEnteringConfigModeCallback);
  paramsServer->setShowInfoCallback(TkParamsServerShowInfoCallback);
  paramsServer->setShowConnectedCallback(TkParamsServerShowConnectedCallback);
  paramsServer->setShowShutdownCallback(TkParamsServerShowShutdownCallback);

  _param_text = new TinkableParam("text", F("text input"), F("텍스트 입력"), "", 4, F(" minlength='1'"), TK_PARMAS_SERVER_INPUT_TEXT);
  paramsServer->addParameter(_param_text);

  _param_number = new TinkableParam("number", F("number input"), F("숫자 입력"), "", 8, F("type='number' min='0' max='44676000'"), TK_PARMAS_SERVER_INPUT_TEXT);
  paramsServer->addParameter(_param_text);

  _param_bool = new TinkableParam("bool", F("bool select"), F("이진값 선택"), "1", 1, nullptr, TK_PARMAS_SERVER_SELECT_BOOL);
  paramsServer->addParameter(_param_bool);

  _param_textarea = new TinkableParam("textarea", F("textarea"), F("텍스트 줄"), "", 64, TK_PARMAS_SERVER_TEXTAREA);
  paramsServer->addParameter(_param_textarea);

  _param_custom = new TinkableParam("custom", F("custom select"), F("custom 선택"), "3", TK_PARMAS_SERVER_CUSTOM);
  _param_custom->setCustomFullHTML(F("<div class='flex'><label>{t}</label><select id='custom' name='custom' onchange='_ch()'><option value='2'>number 2</option><option value='3'>number 3</option><option value='4'>number 4</option></select></div><script>let w=el('custom');function _setCustom(v){for(var i=0;i<w.options.length;i++){if(w.options[i].value==v){w.selectedIndex=i;break;}}};window.addEventListener('load',function(){ _setCustom('{v}')},false);</script>"));
  paramsServer->addParameter(_param_custom);

  char apName[16];
  snprintf_P(apName, 16, (PGM_P)F("tink_%X"), ESP.getChipId());

  paramsServer->start(apName, TK_PARAMS_SERVER_AP_PASSWORD);
}

void loop() {}
