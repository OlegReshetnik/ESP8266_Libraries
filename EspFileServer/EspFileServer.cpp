#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <StreamString.h>
#include "EspFileServer.h"

static const char updateUri[] PROGMEM = "/update";
static const char editUri[] PROGMEM = "/edit";
static const char default_root[] PROGMEM = "index.html";

static const char updateIndex[] PROGMEM =
R"(<html><body><form method='POST' action='' enctype='multipart/form-data'>
	<input type='file' name='update'>
	<input type='submit' value='Обновить'>
	</form>
	</body></html>)";

static const char startEditHtml[] PROGMEM =
R"(<html><body><form method='POST' action='' enctype='multipart/form-data'>
	<input type='file' name='data'>
	<input type='submit' value='Загрузить'>
	</form>
	<table>)";
static const char endEditHtml[] PROGMEM = "</table></body></html>";

static const char file1Html[] PROGMEM = "<tr><td>";
static const char file2Html[] PROGMEM = "</td><td><a href=\"/edit?delete=";
static const char file3Html[] PROGMEM = "\">Удалить</a></td><td><a href=";
static const char file4Html[] PROGMEM = "?download=yes target=_blank>Скачать</a></td></tr>\n";

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

PGM_P EspFileServer::_getContentType( String filename )
{
	if( _server->hasArg("download") ) return application_octet_stream;
	else if( filename.endsWith(".htm") ) return text_html;
	else if( filename.endsWith(".html") ) return text_html;
	else if( filename.endsWith(".css") ) return text_css;
	else if( filename.endsWith(".js") ) return application_javascript;
	else if( filename.endsWith(".json") ) return text_json;
	else if( filename.endsWith(".png") ) return image_png;
	else if( filename.endsWith(".gif") ) return image_gif;
	else if( filename.endsWith(".jpg") ) return image_jpeg;
	else if( filename.endsWith(".ico") ) return image_x_icon;
	else if( filename.endsWith(".xml") ) return text_xml;
	else if( filename.endsWith(".pdf") ) return application_x_pdf;
	else if( filename.endsWith(".zip") ) return application_x_zip;
	else if( filename.endsWith(".gz") ) return application_x_zip;
	return text_plain;
}

bool EspFileServer::_fileRead( String path )
{

	if( path.endsWith("/") ) path += FPSTR( default_root );
	String contentType = FPSTR( _getContentType( path ) );
	String pathWithGz = path + ".gz";
	if( SPIFFS.exists( pathWithGz ) || SPIFFS.exists( path ) )
	{
		if( SPIFFS.exists( pathWithGz ) ) path += ".gz";
		File file = SPIFFS.open( path, "r" );
		_server->streamFile( file, contentType );
		file.close();
		return true;
	}
	return false;
}

void EspFileServer::_handleUpdate(void)
{
	HTTPUpload& upload = _server->upload();

	if( upload.status == UPLOAD_FILE_START )
	{
		WiFiUDP::stopAll();
		if( !Update.begin( (ESP.getFreeSketchSpace()-0x1000 )&0xFFFFF000) ) _result = Update.getError();
		else _result = 0;
	}
	else if( upload.status == UPLOAD_FILE_WRITE && _result == 0 )
	{
		if( Update.write( upload.buf, upload.currentSize ) != upload.currentSize ) _result = F_ERR_WRITE;
	}
	else if( upload.status == UPLOAD_FILE_END && _result == 0 )
	{
		if( !Update.end(true) ) _result = F_ERR_END;
	}
	else if( upload.status == UPLOAD_FILE_ABORTED )
	{
		_result = F_ERR_ABORT;
		Update.end();
	}
	delay(0);
}

void EspFileServer::_handleUpdatePost(void)
{
	if( Update.hasError() )
	{
		StreamString str;
		Update.printError(str);
		_server->send( 200, FPSTR(text_plain), str );
	}
	else
	{
		_server->client().setNoDelay(true);
		_server->send_P( 200, text_html, PSTR( "<META http-equiv=\"refresh\" content=\"10;URL=/update\">Успешное обновление! Перезагрузка..." ) );
		delay(100);
		_server->client().stop();
		ESP.restart();
	}
}

void EspFileServer::_handleFileUpload(void)
{
	HTTPUpload& upload = _server->upload();

	if( upload.status == UPLOAD_FILE_START )
	{
		String filename = upload.filename;
		if( !filename.startsWith("/")) filename = "/" + filename;
		_fsUploadFile = SPIFFS.open( filename, "w" );
	}
	else if( upload.status == UPLOAD_FILE_WRITE ) _fsUploadFile.write( upload.buf, upload.currentSize );
	
	else if( upload.status == UPLOAD_FILE_END ) _fsUploadFile.close();
}

void EspFileServer::_handleEdit(void)
{
	if( _server->hasArg( "delete" ) )
	{
		String path = _server->arg("delete");
		SPIFFS.remove(path);
		_server->send_P( 200, text_html, PSTR( "<META http-equiv=\"refresh\" content=\"0;URL=/edit\">" ) );
	}

	String ret = FPSTR( startEditHtml );
	Dir dir = SPIFFS.openDir( "/" );

	while( dir.next() )
	{
		ret += FPSTR( file1Html );
		ret += dir.fileName();
		ret += FPSTR( file2Html );
		ret += dir.fileName();
		ret += FPSTR( file3Html );
		ret += dir.fileName();
		ret += FPSTR( file4Html );
	}

	ret += FPSTR( endEditHtml );

	_server->send( 200, FPSTR( text_html ), ret );
}

void EspFileServer::_handleNotFound(void)
{
	if( !_fileRead( _server->uri() ) ) _server->send( 404, FPSTR( text_plain ), String( FPSTR( fileNotFound ) ) + _server->uri() );
}

EspFileServer::EspFileServer( ESP8266WebServer *server )
{
	_server = server;
	_result = true;

	SPIFFS.begin();

	_server->on( updateUri, HTTP_GET, [&](){ _server->send_P( 200, text_html, updateIndex ); } );
    _server->on( updateUri, HTTP_POST, std::bind(&EspFileServer::_handleUpdatePost, this), std::bind(&EspFileServer::_handleUpdate, this) );

	_server->onNotFound( std::bind(&EspFileServer::_handleNotFound, this) );

	_server->on( editUri, HTTP_GET, std::bind(&EspFileServer::_handleEdit, this) );
	_server->on( editUri, HTTP_POST, std::bind(&EspFileServer::_handleEdit, this), std::bind(&EspFileServer::_handleFileUpload, this) );
}
