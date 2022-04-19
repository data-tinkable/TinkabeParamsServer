/*!
@file ConfigString.h
@author data.tinkable@gmail.com
@version 1.97
@copyright Copyright 2022 Tinkable Inc. All rights reserved.
@license Allowed only for Tinkable IoT Kit.
*/

#pragma once

#ifndef TinkableParamsServerConfigString_h
#define TinkableParamsServerConfigString_h

static const char TK_PARAMS_SERVER_LANG_PA[] PROGMEM = "lang";

// static const char FILE_PA[] PROGMEM = "file";
// static const char EN_PA[] PROGMEM = "en";

// static const char LANG_RE[] PROGMEM = "{lang}";
// static const char TITLE_RE[] PROGMEM = "{title}";
// static const char AP_NAME_RE[]  PROGMEM = "{apName}";

// static const char MENU_RE[] PROGMEM = "{menu}";
// static const char BODY_RE[] PROGMEM = "{body}";

static const char TK_PARAMS_SERVER_CONFIG_ID_RE[] PROGMEM = "{i}";
static const char TK_PARAMS_SERVER_CONFIG_LABEL_RE[] PROGMEM = "{t}";
static const char TK_PARAMS_SERVER_CONFIG_MAXLENGTH_RE[] PROGMEM = "{l}";
static const char TK_PARAMS_SERVER_CONFIG_VALUE_RE[] PROGMEM = "{v}";
static const char TK_PARAMS_SERVER_CONFIG_PARAMS_RE[] PROGMEM = "{p}";

// static const char MESSAGE_RE[] PROGMEM = "{m}";
// static const char ERROR_RE[] PROGMEM = "{m}";
// static const char FILES_FILE_RE[] PROGMEM = "{f}";
// static const char FILES_SIZE_RE[] PROGMEM = "{s}";

// static const char TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM_RE[] PROGMEM = "{format}";
// static const char TK_PARAMS_SERVER_DELETE_CONFIRM_RE[] PROGMEM = "{delete}";

static const char TK_PARAMS_SERVER_FORMAT_MEMORY_CONFIRM[] PROGMEM = "It takes more than 10 seconds to format the Flash Memory. Erase completely data in Flash Memory?";

static const char TK_PARAMS_SERVER_DELETE_CONFIRM[] PROGMEM = "Once you delete a file, you can not recover it. delete the file?";

static const char TK_PARAMS_SERVER_ERROR_LOG[] PROGMEM = "[ERROR] ";
static const char TK_PARAMS_SERVER_INFO_LOG[] PROGMEM = "[INFO] ";

static const char TK_PARAMS_SERVER_CONTENT_LENGTH_TEXT[] PROGMEM = "Content-Length";
static const char TK_PARAMS_SERVER_CONTENT_DISPOSITION_TEXT[] PROGMEM = "Content-Disposition";

static const char TK_PARAMS_SERVER_HTML_MIME_TEXT_HTML[] PROGMEM = "text/html";
static const char TK_PARAMS_SERVER_HTML_MIME_TEXT_PLAIN[] PROGMEM = "text/plain";
static const char TK_PARAMS_SERVER_HTML_404_BODY[] PROGMEM = "404: Not found";

static const char TK_PARAMS_SERVER_STREAM_MIME_TEXT[] PROGMEM = "application/octet-stream";

static const char TK_PARAMS_SERVER_INDEX_HTML_TEMPLATE[] PROGMEM = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>con.tinkable.net</title></head><body><script>var la=window.navigator.userLanguage||window.navigator.language; if(la=='ko'||la=='ko-KR'){location.href='/config?lang=ko';}else{location.href='/config?lang=en';}</script><div><a href='/config'>config</a></div></body></html>";

static const char TK_PARAMS_SERVER_INDEX_HTML_TEMPLATE_LENGTH[] PROGMEM = "335";

static const char TK_PARAMS_SERVER_MENU[] PROGMEM = "<a href='/config'>Config</a> | <a href='/info'>Info</a> | <a href='/files'>Files</a> | <a href='/exit'>Exit</a>";

static const char TK_PARAMS_SERVER_PRINT_OUT_FILES_SCRIPT_PRE[] PROGMEM = "<script>var dw=document.write;function pf(ur,sz,ko){if(ko){document.write(\"<div class='box'><h4>\");document.write(ur);document.write(\"</h4><div class='flex'><span>\");document.write(sz);document.write(\" bytes</span><span><button class='rB del' onclick='clinkD(\\\"/delete?file=\");document.write(ur);document.write(\"&lang=ko\\\")'>delete</button><button class='rB download' onclick='clink(\\\"/download?file=\");document.write(ur);document.write(\"&lang=ko\\\")'>download</button></span></div></div>\");}else{document.write(\"<div class='box'><h4>\");document.write(ur);document.write(\"</h4><div class='flex'><span>\");document.write(sz);document.write(\" Bytes</span><span><button class='rB del' onclick='clinkD(\\\"/delete?file=\");document.write(ur);document.write(\"&lang=en\\\")'>Delete</button><button class='rB download' onclick='clink(\\\"/download?file=\");document.write(ur);document.write(\"&lang=en\\\")'>Download</button></span></div></div>\");}};";

static const char TK_PARAMS_SERVER_PRINT_OUT_FILES_SCRIPT_AFTER[] PROGMEM = "</script>";

static const char TK_PARAMS_SERVER_LANG_TEXT[] PROGMEM = "<a href=\"javascript:cLa('en')\"><small>English</small></a>";

static const char TK_PARAMS_SERVER_HTTP1_1_REQUIRED[] PROGMEM = "HTTP1.1 required";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_1[] PROGMEM = "<!DOCTYPE html><html lang='en'><head><meta name='format-detection' content='telephone=no'> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width,initial-scale=1,user-scalable=no'/><title>";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_APNAME[] PROGMEM = "</title><style>body{text-align:center;padding:1em;}a:link{text-decoration:none;}a:hover{text-decoration:unerline;}h1,h2,.menu,.footer{font-family:Verdana;margin:0px;padding:.3ex;border-style:solid;text-align:center}.footer{margin-top:1em;}h1{font-size:1.5em;border-width:4px 0px 1px 0px;border-color:#2181d5;margin-bottom:0px;}h2{font-size:2em;border-width:0px 0px 4px 0px;border-color:#2181d5;background-color:#edf8ff;}div.wrap{display:inline-block; width:100%; max-width:700px}.menu{font-size:1em; border-width:0px 0px 1px 0px;border-color:#2181d5; margin-bottom:1em;background-color:#FFF; padding:.4em}.footer{font-size:.9em;border-width:2px 0px 0px 0px;border-color:#2181d5; margin-top:1em;}h4,p,div,form{text-align:left;}h3{margin :.5em 0em .5em 0em; padding:0px; font-size:1.2em; border-width:0px 0px 1px 0px;border-color:#2181d5; border-style:solid; padding:2px;}h4{margin :0em; padding:.5em 0em .5em 0em; font-size:1em;}input, button,textarea,select{border-radius:.3rem;}input:invalid,textarea:invalid,select:invalid,select:invalid{border-color:red; border-style:dashed;}textarea{height:10em;}button,input[type='button'],input[type='submit']{cursor:pointer;border:0px;background-color:#2181d5;color:#fff;line-height:2.4rem;font-size:1.2rem; width :100%; margin-top:.7em; margin-bottom:.3em;}input,textarea,select{padding:5px;font-size:1em;margin:0px;box-sizing:border-box; border-color:#2181d5;border-style:solid;border-width:1px}div.flex{display:flex; flex-wrap:wrap; align-items:center; margin-top:.5em; margin-bottom:.5em}.flex *:first-child{flex:1 0 80px; max-width:160px}.flex *:last-child{flex:1 0 220px;}div.flex2{display:flex; flex-wrap:wrap; align-items:center; margin-top:.5em; margin-bottom:.5em}.flex2 h4{margin:0px; padding:0px}.flex2 *:first-child{flex:1 0 80px; max-width:220px}.flex2 *:last-child{flex:1 0 80px; max-width:220px;} .flex2p>div{ display:flex; flex-wrap:wrap; align-items:center; margin-top:.5em; margin-bottom:.5em}.flex2p h4{ margin:0px; padding:0px}.flex2p>div *:first-child {flex:1 0 80px; max-width:220px}.flex2p>div *:last-child { flex:1 0 80px; max-width:220px}.rB{width:auto; float:right;line-height:14px;font-size:1em; padding:5px 8px 5px 8px; width:auto; margin:0px}.del{width:100px; margin-left:10px; background-color:brown; padding:.5em}.download{width:100px; margin-left:10px; padding:.5em}.box{border-width:1px; border-color:#2181d5; border-style:solid; margin:1em 0em 1em 0em; padding:.2em .5em .2em .5em; border-radius:.3rem}.box ul{padding:0em 0em 0em 1em; margin:.5em}.box li{margin-bottom:.5em}.D{background-color:brown}.Dt{color:brown;}#detailed{background-color:#edf8ff; padding:.5em; margin-bottom:.5em; padding:15px 8px 15px 8px;}</style><script>function el(id){return document.getElementById(id);}</script></head><body><div class='wrap'><h1> con.tinkable.net (";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_LANG[] PROGMEM = ")<sup>";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_TITLE_2[] PROGMEM = "</sup></h1><h2>";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_MENU[] PROGMEM = "</h2><nav class='menu'>";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_BEFORE_BODY[] PROGMEM = "</nav>";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_END_START[] PROGMEM = "<div class='footer'>&copy;Tinkable,<a href='http://data.tinkable.net'>http://data.tinkable.net</a></div><div> <script>function t(){if(el('de').style.display=='block'){el('de').style.display='none'; el('deB').innerHTML='+';}else{el('de').style.display='block'; el('deB').innerHTML='-';}}function clink(url){location.href=url;}function clinkD(url){if(confirm('";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_DELETE[] PROGMEM = "')){location.href=url;}}function clinkE(url){if(confirm('";

static const char TK_PARAMS_SERVER_HTML_TEMPLATE_END_AFTER_FORMAT[] PROGMEM = "')){location.href=url;}}function cLa(val){var search=window.location.search;var regex=new RegExp('([?;&])lang[^&;]*[;&]?');var query=search.replace(regex,'$1').replace(/&$/,'');window.location.href=window.location.pathname+(query.length > 2?query+'&':'?')+(val?'lang='+val:'');}function jsPa(){if(el('sensorsJson').value.length>=3){try{el('sensorsJson').value=JSON.stringify(JSON.parse(el('sensorsJson').value));return true;}catch(e){alert('invalid sensors.json!');el('sensorsJson').focus();return false;}}else return true;}</script></body></html>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_PRE[] PROGMEM = "<div class='box'> <ul> <li>Saved new configuration.</li><li>Exiting Config Server and continue to the next process.</li><li>To enter Config Server again, reboot the Tinkable IoT Kit.</li><li>saved parameters</li>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_AFTER[] PROGMEM = "</ul></div>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_NOT_END_PRE[] PROGMEM = "<div class='box'> <ul> <li>Saved new configuration.</li><li>endAfterConfig is false. You can do another work.</li><li>To end Config Server, click exit in the menu.</li><li>saved parameters</li>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_MESSAGE_NOT_END_AFTER[] PROGMEM = "</ul></div>";

static const char TK_PARAMS_SERVER_CONFIG_TITLE[] PROGMEM = "Configuration";

static const char TK_PARAMS_SERVER_CONFIG_DETAILED_PRE[] PROGMEM = "<h3>detailed <small>(unit: milliseconds)</small> <button id='deB' class='rB' onclick='t();' type='button'>+</button></h3><div id='de' style='display:none'>";

static const char TK_PARAMS_SERVER_CONFIG_DETAILED_AFTER[] PROGMEM = "</div>";

static const char TK_PARAMS_SERVER_CONFIG_FORM_START[] PROGMEM = "<form action='/save' method='post' onsubmit='return jsPa();'>";

static const char TK_PARAMS_SERVER_CONFIG_FORM_END[] PROGMEM = "</form>";

static const char TK_PARAMS_SERVER_CONFIG_INPUT_LANG[] PROGMEM = "<input type='hidden' name='lang' value='en'/>";

static const char TK_PARAMS_SERVER_CONFIG_SAVE_BUTTON[] PROGMEM = "<div><button type='submit'>SAVE Config</button></div>";

static const char TK_PARAMS_SERVER_SAVE_SUCCESS_TITLE[] PROGMEM = "Saved Successfully!";

static const char TK_PARAMS_SERVER_SAVE_ERR_TITLE[] PROGMEM = "Error. No Parameter to Save.";

static const char TK_PARAMS_SERVER_EXIT_TITLE[] PROGMEM = "Closed Config Server.";

static const char TK_PARAMS_SERVER_EXIT_MESSAGE[] PROGMEM = "<div class='box'> <ul> <li>Exiting Config Server and reboot Tinkable Iot Kit soon.</li></ul></div>";

static const char TK_PARAMS_SERVER_EXIT_MESSAGE_NO_REBOOT[] PROGMEM = "<div class='box'> <ul> <li>Exiting Config Server and continue to next jobs without rebooting.</li></ul></div>";

static const char TK_PARAMS_SERVER_FILES_TITLE[] PROGMEM = "Files";

static const char TK_PARAMS_SERVER_FILES_MOUNT_ERR_TITLE[] PROGMEM = "Error. Failed to mount file system.";

static const char TK_PARAMS_SERVER_FILES_NO_FILE_ERR_TITLE[] PROGMEM = "No File.";

static const char TK_PARAMS_SERVER_FILES_NOT_EXIST_ERR_TITLE[] PROGMEM = "Error. File doesn't exist.";

static const char TK_PARAMS_SERVER_FILES_DELETE_SUCCESS_TITLE[] PROGMEM = "Success. File deleted.";

static const char TK_PARAMS_SERVER_FILES_CAN_NOT_DELETE_TITLE[] PROGMEM = "Error. can't delete.";

static const char TK_PARAMS_SERVER_FILES_CAN_NOT_OPEN_TITLE[] PROGMEM = "Error. can't open.";

static const char TK_PARAMS_SERVER_INFO_TITLE[] PROGMEM = "Information";

static const char TK_PARAMS_SERVER_INFO_ESP_HEAD[] PROGMEM = "<h3>esp8266</h3>";

static const char TK_PARAMS_SERVER_INFO_WIFI_HEAD[] PROGMEM = "<h3>WiFi</h3>";

static const char TK_PARAMS_SERVER_INFO_STR_PRE[] PROGMEM = "<div><h4>";

static const char TK_PARAMS_SERVER_INFO_STR_MID[] PROGMEM = "</h4><span>";

static const char TK_PARAMS_SERVER_INFO_STR_END[] PROGMEM = "</span></div>";

static const char TK_PARAMS_SERVER_ERASE_BUTTON[] PROGMEM = "<div><button class='D' onclick='clinkE(\"/erase?lang=en\")'>Erase Flash</button></div>";

static const char TK_PARAMS_SERVER_ERASE_FORMAT_ERR_TITLE[] PROGMEM = "Error while Formatting.";

static const char TK_PARAMS_SERVER_ERASE_CONFIG_ERR_TITLE[] PROGMEM = "Error while Erasing ESP Config.";

static const char TK_PARAMS_SERVER_ERASE_FORMAT_SUCCESS_TITLE[] PROGMEM = "Erased Successfully!";

static const char TK_PARAMS_SERVER_ERASE_SUCCESS_MESSAGE[] PROGMEM = "<div class='box'><ul><li>Formatted flash memory as LittleFS.</li></ul></div>";

static const char TK_PARAMS_SERVER_CONFIG_PARAM_INPUT_TEXT[] PROGMEM = "<div class='flex'><label for='{i}'>{t}</label><input id='{i}' name='{i}' maxlength='{l}' value='{v}' type='text' {p}/></div>";

static const char TK_PARAMS_SERVER_CONFIG_PARAM_SELECT_FALSE[] PROGMEM = "<div class='flex'><label for='{i}'>{t}</label><select id='{i}' name='{i}' {p}><option value='0' selected>false</option><option value='1'>true</option></select></div>";

static const char TK_PARAMS_SERVER_CONFIG_PARAM_SELECT_TRUE[] PROGMEM = "<div class='flex'><label for='{i}'>{t}</label><select id='{i}' name='{i}' {p}><option value='0'>false</option><option value='1' selected>true</option></select></div>";

static const char TK_PARAMS_SERVER_CONFIG_PARAM_TEXTAREA[] PROGMEM = "<div class='flex'><label for='{i}'>{t}</label><textarea id='{i}' name='{i}' maxlength='{l}' {p}>{v}</textarea></div>";

static const char TK_PARAMS_SERVER_ERROR_PRE[] PROGMEM = "<div class='box'><p>";

static const char TK_PARAMS_SERVER_ERROR_AFTER[] PROGMEM = "</p></div>";

#endif
