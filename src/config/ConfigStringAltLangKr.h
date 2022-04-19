/*!
@file ConfigStringAltLangKr.h
@author data.tinkable@gmail.com
@version 1.97
@copyright Copyright 2022 Tinkable Inc. All rights reserved.
@license Allowed only for Tinkable IoT Kit.
*/

#pragma once

#ifndef TinkableParamsServerConfigStringAltLang_h
#define TinkableParamsServerConfigStringAltLang_h

static const char TK_PARAMS_SERVER_ALTLANG_PA[] PROGMEM = "ko";

// static const char FILES_FILE_BOX_SCRIPT_ALTLANG[] PROGMEM = "pf('{f}',{s},1);";

static const char TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_ALTLANG[] PROGMEM = "메모리를 새로 포맷하는 데는 10초 이상 걸립니다. 플래시 메모리의 데이터를 모두 삭제할까요?";

static const char TK_PARAMS_SERVER_DELETE_CONFIRM_ALTLANG[] PROGMEM = "파일을 삭제하면 되돌릴 수 없습니다. 파일을 삭제할까요?";

static const char TK_PARAMS_SERVER_MENU_ALTLANG[] PROGMEM = "<a href='/config?lang=ko'>설정</a> | <a href='/info?lang=ko'>정보</a> | <a href='/files?lang=ko'>파일</a> | <a href='/exit?lang=ko'>종료</a>";

// static const char DELETE_TITLE_ALTLANG[] PROGMEM = "파일 삭제";

static const char TK_PARAMS_SERVER_LANG_TEXT_ALTLANG[] PROGMEM = "<a href=\"javascript:cLa('ko')\"><small>한글</small></a>";

static const char TK_PARAMS_SERVER_CONFIG_TITLE_ALTLANG[] PROGMEM = "설정";

static const char TK_PARAMS_SERVER_CONFIG_DETAILED_PRE_ALTLANG[] PROGMEM = "<h3>상세설정 <small>(단위: 1/1000초)</small> <button id='deB' class='rB' onclick='t();' type='button'>+</button></h3><div id='de' style='display:none'>";

static const char TK_PARAMS_SERVER_CONFIG_INPUT_LANG_ALTLANG[] PROGMEM = "<input type='hidden' name='lang' value='ko'/>";

static const char TK_PARAMS_SERVER_CONFIG_SAVE_BUTTON_ALTLANG[] PROGMEM = "<div><button type='submit'>설정 저장하기</button></div>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_TITLE_ALTLANG[] PROGMEM = "설정을 저장했습니다.";

// static const char SAVE_SUCCESS_ALTLANG[] PROGMEM = "<div class='box'> <ul> <li>새로운 설정을 저장했습니다.</li><li>설정 서버를 종료하고 다음 작업을 합니다.</li><li>저장된 설정값</li>{m}</ul></div>";

// static const char SAVE_SUCCESS_ALTLANG_NOT_END[] PROGMEM = "<div class='box'> <ul> <li>새로운 설정을 저장했습니다.</li><li>endAfterConfig가 false입니다. 설정 서버에서 다른 작업을 하셔도 됩니다.</li><li>설정 서버를 종료하려면 메뉴에서 종료를 눌러주세요.</li><li>저장된 설정값</li>{m}</ul></div>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_PRE_ALTLANG[] PROGMEM = "<div class='box'> <ul> <li>새로운 설정을 저장했습니다.</li><li>설정 서버를 종료하고 다음 작업을 합니다.</li><li>저장된 설정값</li>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_AFTER_ALTLANG[] PROGMEM = "</ul></div>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_NOT_END_PRE_ALTLANG[] PROGMEM = "<div class='box'> <ul> <li>새로운 설정을 저장했습니다.</li><li>endAfterConfig가 false입니다. 설정 서버에서 다른 작업을 하셔도 됩니다.</li><li>설정 서버를 종료하려면 메뉴에서 종료를 눌러주세요.</li><li>저장된 설정값</li>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_NOT_END_AFTER_ALTLANG[] PROGMEM = "</ul></div>";

static const char TK_PARAMS_SERVER_SAVE_ERR_TITLE_ALTLANG[] PROGMEM = "에러. 저장할 설정값이 없습니다.";

static const char TK_PARAMS_SERVER_EXIT_TITLE_ALTLANG[] PROGMEM = "설정 서버를 닫았습니다.";

static const char TK_PARAMS_SERVER_EXIT_MESSAGE_ALTLANG[] PROGMEM = "<div class='box'> <ul> <li>변경사항을 저장하지 않고 설정 서버를 종료합니다. </li><li>잠시뒤에 Tinkable IoT 보드가 껐다 켜집니다. 점퍼2가 Off 상태인지 확인하세요.</li></ul></div>";

static const char TK_PARAMS_SERVER_EXIT_MESSAGE_ALTLANG_NO_REBOOT[] PROGMEM = "<div class='box'> <ul> <li>변경사항을 저장하지 않고 설정 서버를 종료합니다. </li><li>리부팅하지 않고 다음 작업을 계속합니다.</li></ul></div>";

static const char TK_PARAMS_SERVER_FILES_TITLE_ALTLANG[] PROGMEM = "파일";

static const char TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE_ALTLANG[] PROGMEM = "에러. 파일 시스템을 불러올 수 없습니다.";

static const char TK_PARAMS_SERVER_FILES_NO_FILE_ERR_TITLE_ALTLANG[] PROGMEM = "파일이 없습니다.";

static const char TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE_ALTLANG[] PROGMEM = "에러. 그런 이름의 파일이 없습니다.";

static const char TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE_ALTLANG[] PROGMEM = "성공. 파일을 삭제했습니다.";

static const char TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE_ALTLANG[] PROGMEM = "에러. 파일을 삭제할 수 없습니다.";

static const char TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE_ALTLANG[] PROGMEM = "에러. 파일을 열 수 없습니다.";


// static const char FILES_FILE_BOX_ALTLANG[] PROGMEM = "<div class='box'><h4>{f}</h4><div class='flex'><span>{s} 바이트</span><span><button class='rB del' onclick='clinkD(\"/delete?file={f}&lang=ko\")'>삭제</button><button class='rB download' onclick='clink(\"/download?file={f}&lang=ko\")'>다운로드</button></span></div></div>";

static const char TK_PARAMS_SERVER_INFO_TITLE_ALTLANG[] PROGMEM = "정보";

static const char TK_PARAMS_SERVER_ERASE_BUTTON_ALTLANG[] PROGMEM = "<div><button class='D' onclick='clinkE(\"/erase?lang=ko\")'>메모리 삭제하기</button></div>";

static const char TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE_ALTLANG[] PROGMEM = "에러. 포맷하는 중에 에러가 발생했습니다.";

static const char TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE_ALTLANG[] PROGMEM = "ESP 설정을 삭제하다가 에러가 발생했습니다.";

static const char TK_PARAMS_SERVER_ERASE_FORMAT_SUCCESS_TITLE_ALTLANG[] PROGMEM = "메모리를 지웠습니다.";

static const char TK_PARAMS_SERVER_ERASE_SUCCESS_ALTLANG[] PROGMEM = "<div class='box'><ul><li>플래시 메모리를 LittleFS로 포맷했습니다.</li></ul></div>";


#endif
