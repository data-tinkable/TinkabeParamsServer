/*!
@file TinkableParam.cpp
@author data.tinkable@gmail.com
@version 1.97
@copyright Copyright 2022 Tinkable Inc. All rights reserved.
@license Allowed only for Tinkable IoT Kit.
*/

#ifndef TinkableParam_cpp
#define TinkableParam_cpp

#include "./TinkableParam.h"

TinkableParam::TinkableParam(const	__FlashStringHelper *customHTML) {
  _length = 1;
  _customHTML = customHTML;
}

TinkableParam::TinkableParam(const char *id, const	__FlashStringHelper *label, const	__FlashStringHelper *labelKo, const TkParamsServerFormControl type) {
  init(id, label, labelKo, nullptr, 0, nullptr, type);
}

TinkableParam::TinkableParam(const	char *id, const	__FlashStringHelper *label, const	__FlashStringHelper *labelKo, const	char *defaultValue, const short int length, const TkParamsServerFormControl type) {
  init(id, label, labelKo, defaultValue, length, nullptr, type);
}

TinkableParam::TinkableParam(const	char *id, const	__FlashStringHelper *label, const	__FlashStringHelper *labelKo, const	char *defaultValue, const short int length, const	__FlashStringHelper *custom, const TkParamsServerFormControl type) {
  init(id, label, labelKo, defaultValue, length, custom, type);
}

void TinkableParam::init(const	char *id, const	__FlashStringHelper *label, const	__FlashStringHelper *labelKo, const	char *defaultValue, const short int length, const	__FlashStringHelper *custom, const TkParamsServerFormControl type) {
  _id = id;
  _label = label;
  _labelKo = labelKo;
  _customHTML = custom;
  _customFullHTML = nullptr;
  _type = type;
  setValue(defaultValue, length);
}

TinkableParam::~TinkableParam(void) {
	if (_value != nullptr) {
    delete[] _value;
  }
}

void TinkableParam::setValue(const char *defaultValue, const short int length) {
  if(!_id) return;
  _length = length;
  _value  = new char[_length + 1];
  memset(_value, 0, _length + 1);
  strncpy(_value, defaultValue, _length);
};

const	char *TinkableParam::getValue(void) {
  return _value;
};

const	char *TinkableParam::getID(void) {
  return _id;
}

const	__FlashStringHelper* TinkableParam::getLabel(void) {
  return _label;
};

const	__FlashStringHelper* TinkableParam::getLabelKo(void) {
  return _labelKo;
};

short int TinkableParam::getValueLength(void) {
  return _length;
};

const	__FlashStringHelper* TinkableParam::getCustomHTML(void) {
  return _customHTML;
};

TkParamsServerFormControl TinkableParam::getType(void){
  return _type;
};

const	__FlashStringHelper	*TinkableParam::getCustomFullHTML(void) {
  return _customFullHTML;
};

void TinkableParam::setCustomFullHTML(const	__FlashStringHelper	*customFull) {
  _customFullHTML = customFull;
};

#endif
