#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <sys/locking.h>
#include <sys/stat.h>
#include <sys/types.h>
//using namespace std;
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
//#include "wrap.h"
//#include <sys/select.h>
#include <ctype.h>
#   include <windows.h>
//#include <WinSock2.h>
# include <winsock2.h>
# include <ws2tcpip.h>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#define SERV_PORT 8888


void win_socket_init()
{
	//begin 初始化网络环境
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{            //加载winsock2库
		printf("加载失败./n");
		//return -1;
		return;
	}
	
}

void win_socket_clear()
{
	::WSACleanup();
}



bool set_nonblocking(SOCKET sockfd, bool on)
{

	//	int flags = ::fcntl(sockfd, F_GETFL, 0);
	/***设套接字为非阻塞模式***/
	unsigned long ul = 1;
	int	flags = ::ioctlsocket(sockfd, FIONBIO, &ul); //设置套接字非阻塞模式
	if (flags == -1)
	{
		return false;
	}

	
	return true;
}




void headler_async_send(SOCKET sock)
{
	while (true)
	{
		printf("[%s][%d]\n", __FUNCTION__, __LINE__);
		std::string name;
		std::cout << "table ID ";
		std::getline(std::cin, name);
		printf("client -> [%s]\n", name.c_str());
		::send(sock, name.c_str(), name.length(), 0);
	}
}

int main(int argc, char *argv[])
{
	win_socket_init();



	struct sockaddr_in addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = ::htons(SERV_PORT);
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");  //要连接的服务器的ip地址


	SOCKET listenfd, maxfd;
	listenfd = ::socket(AF_INET, SOCK_STREAM, 0);                    //建立客户端流式套接口
	connect(listenfd, (struct sockaddr *)&addrSrv, sizeof(addrSrv));      //与服务器进行连接
	struct sockaddr_in localaddr;
	char cli_ip[20];
	socklen_t local_len = sizeof(localaddr);
	memset(&localaddr, 0, sizeof(localaddr));
	if (getsockname(listenfd, (struct sockaddr *)&localaddr, &local_len) != 0)
	{
		printf("getsockname error\n");
	}
	inet_ntop(AF_INET, &localaddr.sin_addr, cli_ip, sizeof(cli_ip));
	printf("host %s:%d\n", cli_ip, ntohs(localaddr.sin_port));

	fd_set rset;
	FD_ZERO(&rset);
	int nready;
	maxfd = listenfd;
	
	char recvbuf[1024] = { 0 };
	set_nonblocking(listenfd, 1);
	std::thread ptrhead(std::bind(&headler_async_send, listenfd));
	while (true)
	{
		FD_SET(listenfd, &rset);
		struct timeval tem_timeout;
		tem_timeout.tv_sec = 3;
		tem_timeout.tv_usec = 0;
		nready = select(maxfd + 1, &rset, NULL, NULL, (const struct timeval*) &tem_timeout); //select返回表示检测到可读事件
		if (nready == -1)
		{
			printf("select error\n");
		}
		if (nready == 0)
		{
			printf("nredady = %u\n", nready);
			continue;
		}

		if (FD_ISSET(listenfd, &rset))
		{

			int ret = ::recv(listenfd, recvbuf, sizeof(recvbuf), 0);
			if (ret == -1)
			{
				printf("read error\n");
			}
			else if (ret == 0)   //服务器关闭
			{
				printf("server close\n");
				break;
			}

			printf("server-> %s\n", recvbuf);
			memset(recvbuf, 0, sizeof(recvbuf));
		}
	}

	if (ptrhead.joinable())
	{
		ptrhead.join();
	}
	closesocket(listenfd);


	win_socket_clear();
	return EXIT_SUCCESS;
}


