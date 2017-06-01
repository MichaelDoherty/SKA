//-----------------------------------------------------------------------------
// NetworkProcessing project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// NetServer.h
//    Network server for exporting data.
//    Should this simply be an output stream manager, with multiple streams?
//-----------------------------------------------------------------------------
#ifndef NETSERVER_DOT_H
#define NETSERVER_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>

// Socket details hidden in NetServer.cpp, so that the 
// socket header files do not need to be included here.
struct NetServerConnectionData;

class NetServer
{
public:

	// constructor and destructor
	NetServer(char* _server_addr = NULL, // server_addr default is 127.0.0.1
		unsigned short _server_port = 12345);
	~NetServer();

	// server socket management
	bool openServerSocket();
	bool closeServerSocket();
	
	// client socket management
	// (Client connection needs to be threaded and allow for multiple connections.)
	bool startListener();
	bool killListener();
	bool listenerKillSignal();
	bool connectClient();
	bool closeClientSockets();

	// communication
	bool sendToAll(char* sendbuf, int sendbuflen, char* recvbuf, int recvbuflen);

private:
	char* server_address;
	unsigned short  server_port;
	NetServerConnectionData* connections;

	// winsock management
	bool initWinsock();
	bool closeWinsock();

	// error reporting
	void reportError();
};

#endif // NETSERVER_DOT_H
