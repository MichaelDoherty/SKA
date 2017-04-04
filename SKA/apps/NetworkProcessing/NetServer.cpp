//-----------------------------------------------------------------------------
// PTAnalysis project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// NetServer.cpp
//    Network server for exporting data.
//    Currently Windows specific, since it only supports winsock.
//    Currently only allows for one instance of NetServer, since the 
//    constructor/destructor initalizes/shuts down winsock
//-----------------------------------------------------------------------------
// SKA configuration.
#include <Core/SystemConfiguration.h>

#ifndef UNICODE
#define UNICODE 1
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS	// allow use of inet_addr()
#define _CRT_SECURE_NO_WARNINGS			// allow use of sprintf()

#pragma comment(lib,"Ws2_32.lib")		// winsock library
#include <winsock2.h>
#include <ws2tcpip.h>

#define HAVE_STRUCT_TIMESPEC // prevent redefinition of struct timespec in pthread.hs
#include <pthread.h>

#include <stdio.h>
#include <vector>
using namespace std;
#include <Core/Utilities.h>
#include <Core/SystemLog.h>

#include "NetServer.h"

enum LOCAL_ERROR_CODE {
	ALL_GOOD, ERR_WINSOCK, ERR_SOCKET, ERR_BIND, ERR_LISTEN, ERR_ACCEPT, ERR_SEND, ERR_RECV
};

struct NetServerConnectionData {
	bool winsock_valid;

	SOCKET server_socket;
	vector<SOCKET> client_sockets;

	pthread_t listener_thread_id;
	bool listener_active;
	bool listener_kill_signal;

	int winsock_errorcode;
	LOCAL_ERROR_CODE local_errorcode;

	NetServerConnectionData() :
		winsock_valid(false), server_socket(INVALID_SOCKET),
		listener_active(false), listener_kill_signal(false),
		winsock_errorcode(0), local_errorcode(ALL_GOOD)
		{}
};

//===================================================================
// constructor and destructor

NetServer::NetServer(char* _server_addr, unsigned short _server_port)
	: server_port(_server_port), server_address(NULL)
{
	if (_server_addr == NULL) server_address = strClone("127.0.0.1");
	else server_address = strClone(_server_addr);
	connections = new NetServerConnectionData;
	if (!initWinsock()) reportError(); 

	if (connections->winsock_valid && !openServerSocket()) reportError(); 
}

NetServer::~NetServer()
{
	if (!closeClientSockets()) reportError();
	if (!closeServerSocket()) reportError();
	if (!closeWinsock()) reportError();
	if (server_address != NULL) delete[] server_address;
	if (connections != NULL) delete connections;
}

//===================================================================
// server socket management

bool NetServer::openServerSocket()
{
	// Create a SOCKET to listen for incoming connection requests.
	connections->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//connections->server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (connections->server_socket == INVALID_SOCKET) {
		connections->winsock_errorcode = WSAGetLastError();
		connections->local_errorcode = ERR_SOCKET;
		return false;
	}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(server_address);
	//inet_pton(AF_INET, server_address, &service.sin_addr);
	service.sin_port = htons(server_port);
	if (bind(connections->server_socket,
		(SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR) {
		connections->winsock_errorcode = WSAGetLastError();
		connections->local_errorcode = ERR_BIND;
		return false;
	}

	// Listen for incoming connection requests on the created socket
	if (listen(connections->server_socket, 1) == SOCKET_ERROR) {
		connections->winsock_errorcode = WSAGetLastError();
		connections->local_errorcode = ERR_LISTEN;
		return false;
	}
	startListener();

	return true;
}

bool NetServer::closeServerSocket()
{
	killListener();
	if (connections->server_socket != INVALID_SOCKET) {
		closesocket(connections->server_socket);
	}
	connections->server_socket = INVALID_SOCKET;
	return true;
}

//===================================================================
// client socket management

void* listener(void* p)
{
	NetServer* ns = (NetServer*)p;
	while (!ns->listenerKillSignal()) {
		ns->connectClient();
		Sleep(500);
	}
	pthread_exit(NULL);
	return NULL;
}

bool NetServer::startListener()
{
	if (pthread_create(&connections->listener_thread_id, NULL, listener, (void*)this) < 0) {
		connections->listener_active = false;
	}
	else {
		connections->listener_active = true;
	}
	return connections->listener_active;
}

bool NetServer::listenerKillSignal() {
	return connections->listener_kill_signal;
}

bool NetServer::killListener() {
	if (connections->listener_active) {
		connections->listener_kill_signal = true;
		//pthread_cancel(connections->listener_thread_id);
		pthread_join(connections->listener_thread_id, NULL);
	}
	return true;
}

bool NetServer::connectClient()
{
	// See if connection pending
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(connections->server_socket, &readSet);
	timeval timeout;
	timeout.tv_sec = 0;  // Zero timeout (poll)
	timeout.tv_usec = 0;
	if (select(connections->server_socket, &readSet, NULL, NULL, &timeout) == 1)
	{
		printf("Client connect request detected.\n");

		// Accept the connection.
		SOCKET new_client = accept(connections->server_socket, NULL, NULL);
		if (new_client == INVALID_SOCKET) {
			connections->winsock_errorcode = WSAGetLastError();
			connections->local_errorcode = ERR_ACCEPT;
			printf("Client connection failed.\n");
			return false;
		}
		else {
			// FIXIT! this push needs to be protected.
			connections->client_sockets.push_back(new_client);
			printf("Client connected.\n");
			return true;
		}
	}
	return true;
}

bool NetServer::closeClientSockets()
{
	for (unsigned int s = 0; s < connections->client_sockets.size(); s++) {
		SOCKET sock = connections->client_sockets[s];
		if (sock != INVALID_SOCKET) closesocket(sock);
	}
	connections->client_sockets.clear();
	return true;
}

//===================================================================
// client socket management

// FIXIT! This can block/error on a client that shuts down, 
//        preventing communication with other clients.
bool NetServer::sendToAll(char* sendbuf, int sendbuflen, char* recvbuf, int recvbuflen)
{
	//int recvResult;
	int sendResult;
	for (unsigned int s = 0; s < connections->client_sockets.size(); s++) {
		SOCKET sock = connections->client_sockets[s];
		if (sock != INVALID_SOCKET) {
			sendResult = send(sock, sendbuf, strlen(sendbuf), 0);
			if (sendResult == SOCKET_ERROR) {
				connections->winsock_errorcode = WSAGetLastError();
				connections->local_errorcode = ERR_SEND;
				return false;
			}
			/*
			// expect client to echo our message
			recvResult = recv(sock, recvbuf, recvbuflen, 0);
			if (sendResult > 0) {
				recvbuf[recvResult] = '\0';
				printf("Echo message was %s\n", recvbuf);
			}
			*/
		}
	}
	return true;
}

//===================================================================
// error reporting

void NetServer::reportError()
{
	if (connections->local_errorcode != ALL_GOOD) {
		logout << "Networking error. Local error code " << connections->local_errorcode
			<< ". Winsock error code " << connections->winsock_errorcode << "." << endl;
	}
}

//===================================================================
// winsock management
//
// from https://msdn.microsoft.com/en-us/library/windows/desktop/ms741549(v=vs.85).aspx:
// There must be a call to WSACleanup for each successful call to WSAStartup.
// Only the final WSACleanup function call performs the actual cleanup.
// The preceding calls simply decrement an internal reference count in the WS2_32.DLL.

bool NetServer::initWinsock()
{
	WSAData wsa_data;
	int wsa_start_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (wsa_start_result != NO_ERROR) {
		connections->winsock_valid = false;
		connections->winsock_errorcode = wsa_start_result;
		connections->local_errorcode = ERR_WINSOCK;
		return false;
	}
	connections->winsock_valid = true;
	return true;
}

bool NetServer::closeWinsock()
{
	int wsa_clean_result = WSACleanup();
	if (wsa_clean_result != NO_ERROR) {
		connections->winsock_valid = false;
		connections->winsock_errorcode = wsa_clean_result;
		connections->local_errorcode = ERR_WINSOCK;
		return false;
	}
	connections->winsock_valid = false;
	return true;
}

