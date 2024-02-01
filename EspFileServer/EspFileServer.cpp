#include "EspFileServer.h"

static const char updateUri[] PROGMEM = "/update";
static const char setupUri[] PROGMEM = "/setup";
static const char editUri[] PROGMEM = "/edit";
static const char default_root[] PROGMEM = "index.html";
static const char settings[] PROGMEM = "/settings.txt";


static const char updateIndex[] PROGMEM = "<html><body><form method='POST' action='' enctype='multipart/form-data'>"
"<input type='file' name='update'><input type='submit' value='Обновить'></form></body></html>";

static const char setupIndex[] PROGMEM = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"</head><body><a href='/'>На главную</a><br><br><script>d('G',1);"
"function i(n){return document.getElementById(n);}function d(p,v){var x=new XMLHttpRequest();"
"x.open('GET','/setup?J=1&'+p+'='+v,true);x.onload=function(){var a=x.responseText.split(':');i('w').value=a[0];i('p').value=a[1];"
"i('h').value=a[2];};x.send();}function s(){d('P',i('w').value+':'+i('p').value+':'+i('h').value);}</script>WiFi SSID<br>"
"<input type='text' size='25' id='w'><br>PassWord<br><input type='text' size='25' id='p'><br>HostName<br>"
"<input type='text' size='25' id='h'><br><button onclick='s();'>Save</button><button onclick=\"d('R',1);\">Reebot</button></body></html>";

static const char startEditHtml[] PROGMEM = "<html><body>"
"<a href='/edit?format=yes'>Форматировать</a><br><br>"
"<form method='POST' action='' enctype='multipart/form-data'>"
"<input type='file' name='data'><input type='submit' value='Загрузить'></form><table>";

static const char endEditHtml[] PROGMEM = "</table></body></html>";

static const char file1Html[] PROGMEM = "<tr><td>";
static const char file2Html[] PROGMEM = "</td><td><a href='/edit?delete=";
static const char file3Html[] PROGMEM = "'>Удалить</a></td><td><a href='";
static const char file4Html[] PROGMEM = "?download=yes' target=_blank>Скачать</a></td></tr>\n";

static const char fileNotFound[] PROGMEM = "Файл не найден: ";
		
static const char application_octet_stream[] PROGMEM = "application/octet-stream";
static const char text_html[] PROGMEM = "text/html; charset=UTF-8";
static const char text_plain[] PROGMEM = "text/plain; charset=UTF-8";
static const char text_css[] PROGMEM = "text/css";
static const char text_json[] PROGMEM = "text/json; charset=UTF-8";
static const char application_javascript[] PROGMEM = "application/javascript; charset=UTF-8";
static const char image_png[] PROGMEM = "image/png";
static const char image_gif[] PROGMEM = "image/gif";
static const char image_jpeg[] PROGMEM = "image/jpeg";
static const char image_x_icon[] PROGMEM = "image/x-icon";
static const char text_xml[] PROGMEM = "text/xml; charset=UTF-8";
static const char application_x_pdf[] PROGMEM = "application/x-pdf";
static const char application_x_zip[] PROGMEM = "application/x-zip";

/*
static const char f_err_space[] PROGMEM = "Нет места для обновления, есть: ";
static const char f_err_write[] PROGMEM = "Ошибка записи обновления";
static const char f_err_end[] PROGMEM = "Ошибка завершения обновления";
static const char f_err_abort[] PROGMEM = "Отмена обновления";
*/

PGM_P EspFileServer::_getContentType(String filename) {
	if( _server->hasArg(String(F("download"))) ) return application_octet_stream;
	else if( filename.endsWith(String(F("htm"))) ) return text_html;
	else if( filename.endsWith(String(F("html"))) ) return text_html;
	else if( filename.endsWith(String(F("css"))) ) return text_css;
	else if( filename.endsWith(String(F("js"))) ) return application_javascript;
	else if( filename.endsWith(String(F("json"))) ) return text_json;
	else if( filename.endsWith(String(F("png"))) ) return image_png;
	else if( filename.endsWith(String(F("gif"))) ) return image_gif;
	else if( filename.endsWith(String(F("jpg"))) ) return image_jpeg;
	else if( filename.endsWith(String(F("ico"))) ) return image_x_icon;
	else if( filename.endsWith(String(F("xml"))) ) return text_xml;
	else if( filename.endsWith(String(F("pdf"))) ) return application_x_pdf;
	else if( filename.endsWith(String(F("zip"))) ) return application_x_zip;
	else if( filename.endsWith(String(F("gz"))) ) return application_x_zip;
	return text_plain;
}

bool EspFileServer::_fileRead(String path) {
	if (path.endsWith("/")) { path += FPSTR(default_root); }
	String contentType = FPSTR(_getContentType(path));
	String pathWithGz = path + String(F(".gz"));
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz)) { path = pathWithGz; }
		File file = SPIFFS.open(path, "r");
		_server->streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

void EspFileServer::_handleUpdate(void) {
	HTTPUpload& upload = _server->upload();

	if (upload.status == UPLOAD_FILE_START) {
		WiFiUDP::stopAll();
		if (!Update.begin((ESP.getFreeSketchSpace()-0x1000) & 0xFFFFF000)) { _result = Update.getError(); }
		else { _result = 0; }
	}
	else if (upload.status == UPLOAD_FILE_WRITE && _result == 0) {
		if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { _result = F_ERR_WRITE; }
	}
	else if (upload.status == UPLOAD_FILE_END && _result == 0) {
		if (!Update.end(true)) { _result = F_ERR_END; }
	}
	else if (upload.status == UPLOAD_FILE_ABORTED) {
		_result = F_ERR_ABORT;
		Update.end();
	}
	delay(0);
}

void EspFileServer::_handleUpdatePost(void) {
	if (Update.hasError()) {
		StreamString str;
		Update.printError(str);
		_server->send(200, FPSTR(text_plain), str);
	}
	else {
		_server->client().setNoDelay(true);
		_server->send_P(200, text_html, PSTR("<META http-equiv=\"refresh\" content=\"10;URL=/update\">Успешное обновление! Перезагрузка..."));
		delay(100);
		_server->client().stop();
		ESP.restart();
	}
}

void EspFileServer::_handleFileUpload(void) {
	HTTPUpload& upload = _server->upload();

	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/")) { filename = "/" + filename; }
		_fsUploadFile = SPIFFS.open(filename, "w");
	}
	else if (upload.status == UPLOAD_FILE_WRITE) { _fsUploadFile.write(upload.buf, upload.currentSize); }
	else if (upload.status == UPLOAD_FILE_END) { _fsUploadFile.close(); }
}

void EspFileServer::_handleEdit(void) {
	String del = String(F("delete"));
	if (_server->hasArg(del)) {
		SPIFFS.remove(_server->arg(del));
		_server->send_P(200, text_html, PSTR("<META http-equiv=\"refresh\" content=\"0;URL=/edit\">"));
	}
	
	else if(_server->hasArg(String(F("format")))) {
		SPIFFS.format();
		_server->send_P(200, text_html, PSTR("<META http-equiv=\"refresh\" content=\"1;URL=/edit\">Успешно отформатировано!"));
	}

	else {
		String ret = String(FPSTR(startEditHtml));
		Dir dir = SPIFFS.openDir("/");
		while (dir.next()) {
			ret += String(FPSTR(file1Html));
			ret += dir.fileName();
			ret += String(FPSTR(file2Html));
			ret += dir.fileName();
			ret += String(FPSTR(file3Html));
			ret += dir.fileName();
			ret += String(FPSTR(file4Html));
		}
		ret += String(FPSTR(endEditHtml));
		_server->send(200, String(FPSTR(text_html)), ret );
	}
}

void EspFileServer::_handleSetup(void) {
	if (_server->hasArg("J")) {
		if (_server->hasArg("G")) {
			File file = SPIFFS.open(FPSTR(settings), "r");
			_server->streamFile(file, String(FPSTR(text_plain)));
			file.close();
		}
		else if (_server->hasArg("P")) {
			File file = SPIFFS.open(FPSTR(settings), "w");
			file.print(_server->arg("P"));
			file.close();
			_server->send(200, String(FPSTR(text_html)), _server->arg("P"));
		}
		else if (_server->hasArg("R")) {
			WiFiUDP::stopAll();
			_server->client().stop();
			ESP.restart();
		}
	}
	else {
		_server->send(200, String(FPSTR(text_html)), String(FPSTR(setupIndex)));
	}
}

void EspFileServer::_handleNotFound(void) {
	if (!_fileRead(_server->uri())) { _server->send(404, String(FPSTR(text_plain)), String(FPSTR(fileNotFound)) + _server->uri()); }
}

EspFileServer::EspFileServer(ESP8266WebServer *server) {
	_server = server;
	_result = true;
	SPIFFS.begin();

	_server->on(String(FPSTR(updateUri)), HTTP_GET, [&](){ _server->send_P(200, text_html, updateIndex); } );
    _server->on(String(FPSTR(updateUri)), HTTP_POST, std::bind(&EspFileServer::_handleUpdatePost, this), std::bind(&EspFileServer::_handleUpdate, this));

	_server->onNotFound(std::bind(&EspFileServer::_handleNotFound, this));

	_server->on(String(FPSTR(editUri)), HTTP_GET, std::bind(&EspFileServer::_handleEdit, this) );
	_server->on(String(FPSTR(editUri)), HTTP_POST, std::bind(&EspFileServer::_handleEdit, this), std::bind(&EspFileServer::_handleFileUpload, this) );

	_server->on(String(FPSTR(setupUri)), HTTP_GET, std::bind(&EspFileServer::_handleSetup, this) );
}

int EspFileServer::WiFiConnect(void) {
	String str;
	File file;

	if (!SPIFFS.exists(FPSTR(settings))) {
		SPIFFS.format();
		file = SPIFFS.open(FPSTR(settings), "w");
		str = String(F("SSID:Pass:Name"));
		file.print(str);
		file.close();
	}
	else {
		file = SPIFFS.open(FPSTR(settings), "r");
		str = file.readString();
		file.close();
	}
	String wifi = str.substring(0,str.indexOf(':'));
	String pass = str.substring(str.indexOf(':')+1,str.lastIndexOf(':'));
	String host = str.substring(str.lastIndexOf(':')+1);

	WiFi.mode(WIFI_STA);
	WiFi.hostname(host);
	WiFi.begin(wifi, pass);

	uint32_t cnt = MAX_CNT;
	while (cnt--) {
		if (WiFi.waitForConnectResult() == WL_CONNECTED) { return 0; }
		else { delay(ONE_CNT); }
	}

	WiFi.mode(WIFI_AP);
	WiFi.softAP(String(F("SETUP_192_168_4_1")));

	return 1;
}
