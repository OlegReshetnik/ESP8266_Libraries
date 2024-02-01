#ifndef __ESP_FILE_SERVER_H
#define __ESP_FILE_SERVER_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <StreamString.h>
#include <FS.h>

#define F_ERR_SPACE		1
#define F_ERR_WRITE		2
#define F_ERR_END		3
#define F_ERR_ABORT		4

#define WAIT_STA	10 // Connect to Router timeout, 10 sec

#define ONE_CNT		50
#define MAX_CNT		(WAIT_STA*1000/ONE_CNT) 

//class ESP8266WebServer;

class EspFileServer
{
	public:
		EspFileServer(ESP8266WebServer *server);
		int WiFiConnect(void); // 1-Acess Point 192.168.4.1, 0-Connect to WiFi
		void tick(void);

	protected:
		void _handleFileUpload();
		void _handleEdit();
		void _handleNotFound();
		void _handleUpdate();
		void _handleUpdatePost();
		void _handleSetup();
		PGM_P _getContentType(String filename);
		bool _fileRead(String path);

	private:
		ESP8266WebServer *_server;
		File _fsUploadFile;
		uint8_t _result;
};

#endif
