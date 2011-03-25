#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <pthread\pthread.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "pthreadVC2.lib")

#include "KinectController.h"
#include "as3Skeleton.h"


PCSTR DEFAULT_PORT = "6001";

SOCKET listenSocket = INVALID_SOCKET, clientSocket = INVALID_SOCKET;
pthread_t waitForClientThread, connectionThread;
bool die, isConnected;
KinectController* kinect;
int dataLen = 0;

unsigned short depthBuffer[640*480];
unsigned char depthImageBuffer[4*640*480];
unsigned char imageBuffer[4*640*480];

#define MAX_PACKET_SIZE (640*480*4)+2
#define MAX_USERS 15
#define DEFAULT_BUFLEN 64

unsigned char *msg = new unsigned char[MAX_PACKET_SIZE];
As3Skeleton* skeletons;

int sendData(unsigned char *buffer, int length){
	if(clientSocket != INVALID_SOCKET) {
		return send(clientSocket, (char*)buffer, length, 0);
	}
	return 0;
}

void sendMessage(const char *data) {
	std::string _msg = data;
	int len = sizeof(_msg);
	int first = 3;
	int second = 0;
	int m_len = 1 + 1 + sizeof(int);
	memcpy(msg, &first, 1);
	memcpy(msg + 1, &second, 1);
	memcpy(msg + 2, &len, sizeof(int));
	memcpy(msg + m_len, data, len);
	sendData(msg,len+m_len);
}

void sendMessage(int first, int second, int value) {
	unsigned char buff[2 + sizeof(int) * 2];
	buff[0] = first;
	buff[1] = second;
	int size = sizeof(int);
	memcpy(buff+2, &size, sizeof(int));
	memcpy(buff+6, &value, sizeof(int));
	sendData(buff, 2 + sizeof(int) * 2);
}

void sendMessage(int first, int second, unsigned char *data, int len) {
	dataLen = 1 + 1 + sizeof(int);
	memcpy(msg, &first, 1);
	memcpy(msg + 1, &second, 1);
	memcpy(msg + 2, &len, sizeof(int));
	memcpy(msg + dataLen, data, len);
	sendData(msg,dataLen + len);
}

void sendDepthData(int first, int second, unsigned short *data, int len) {
	unsigned char buf[(640*480*sizeof(short))+2+sizeof(int)];
	dataLen = 1 + 1 + sizeof(int);
	memcpy(buf, &first, 1);
	memcpy(buf + 1, &second, 1);
	memcpy(buf + 2, &len, sizeof(int));
	memcpy(buf + dataLen, data, len);
	sendData(buf,dataLen + len);
}

void* connectionHandler(void* arg)
{
	int len = 0;
	char buff[DEFAULT_BUFLEN];

	while(isConnected) {
		len = recv(clientSocket, buff, DEFAULT_BUFLEN, 0);
		
		if(len > 0 && len % 8 == 0){
			//Get the number of commands received
			int max = len / 8;
			int i;
			//For each command received
			for(i = 0; i < max; i++){
				switch(buff[0 + (i*8)]){
					case 0: //CAMERA
						switch(buff[1 + (i*8)]){
							case 0: //GET DEPTH
								kinect->getDepthBuffer(depthBuffer, depthImageBuffer);
								sendDepthData(0,0,depthBuffer, sizeof(depthBuffer));
							break;
							case 1: //GET RGB
								kinect->getColorBuffer(imageBuffer);
								sendMessage(0,1,imageBuffer, sizeof(imageBuffer));
							break;
							case 2: //GET SKEL
								skeletons = kinect->getSkeletons();
								for (int j = 0; j <= MAX_USERS; j++)
								{
									if(skeletons[j].isTracking == FALSE)
										sendMessage(0, 2, skeletons[j].skel, skeletons[j].size);
								}
							break;
							case 3: //GET DEPTH IMAGE
								kinect->getDepthBuffer(depthBuffer, depthImageBuffer);
								sendMessage(0,3,depthImageBuffer, sizeof(depthImageBuffer));
								break;
							case 4: // UPDATE USER GENERATOR
								kinect->updateUserGenerator();
								break;
						}
					break;
					case 1: //MOTOR
					break;
					case 2: //MIC
					break;
				}
			}
		} else {
			printf("[~] Got bad command (%d)\n", len);
			isConnected = false;
			kinect->stop();
			closesocket(clientSocket);
			break;
		}
	}

	printf("[-] Disconnecting client\n\n");
	printf("[+] Listening for client\n");

	return NULL;
}

void* waitForClient(void* arg)
{
	while(!die)
	{
		clientSocket = accept(listenSocket, NULL, NULL);
		if(clientSocket == INVALID_SOCKET) {
			printf("[~] Accept failed: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			die = true;
			return NULL;
		}

		printf("[+] Connection established\n");
		
		isConnected = true;
		kinect->setClientSocket(clientSocket);
		kinect->resume();

		if(pthread_create(&connectionThread, NULL, connectionHandler, NULL))
		{
			fprintf(stderr, "[~] Error on pthread_create() connectionThread.\n");
			WSACleanup();
			die = true;
			return NULL;
		}
	}

	return NULL;
}

int createSocketServer()
{
	printf("[+] Creating socket server\n");

	WSADATA wsaData;

	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		printf("[~] WSAStartup failed.\n");
		return 1;
	}

	struct addrinfo *result = NULL,	*ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
		printf("[~] getaddrinfo failed.\n");
		WSACleanup();
		return 1;
	}

	listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(listenSocket == INVALID_SOCKET) {
		printf("[~] Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

    if (bind(listenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        printf("[~] Bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

	freeaddrinfo(result);

	if (listen(listenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf("[~] Listen failed with error: %ld\n", WSAGetLastError() );
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("[+] Socket server created\n");

	return 0;
}

int main(int argc, char **argv)
{
	printf("----------------------------------\n");
	printf("- AS3Kinect                      -\n");
	printf("----------------------------------\n\n");

	die = false;

	kinect = new KinectController();
	kinect->initialise();

	if(createSocketServer() == 1)
		return 1;

	printf("[+] Listening for client\n");
	if(pthread_create(&waitForClientThread, NULL, waitForClient, NULL))
	{
		fprintf(stderr, "[~] Error on pthread_create() waitForClient.\n");
		return -1;
	}


	while(!die)
	{
		if(kinect->isReady() && isConnected)
			kinect->update();
	}

	if ( listenSocket != INVALID_SOCKET )
		closesocket(listenSocket);

	delete kinect;

	return 0;
}
