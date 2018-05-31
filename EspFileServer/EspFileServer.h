#ifndef __ESP_FILE_SERVER_H
#define __ESP_FILE_SERVER_H

#include <FS.h>

#define F_ERR_SPACE		1
#define F_ERR_WRITE		2
#define F_ERR_END		3
#define F_ERR_ABORT		4


class ESP8266WebServer;

class EspFileServer
{
	public:
		EspFileServer( ESP8266WebServer *server );

	protected:
		void _handleFileUpload();
		void _handleEdit();
		void _handleNotFound();
		void _handleUpdate();
		void _handleUpdatePost();
		PGM_P _getContentType( String filename );
		bool _fileRead( String path );

	private:
		ESP8266WebServer *_server;
		File _fsUploadFile;
		uint8_t _result;
};

#endif
