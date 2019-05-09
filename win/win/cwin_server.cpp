
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
	//end  初始化网络环境
	//int listenfd, maxfd, sockfd;
	//int maxfd, sockfd;
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

int main(int argc, char *argv[])
{
	//begin 初始化网络环境
	win_socket_init();
	//end  初始化网络环境
	//int listenfd, maxfd, sockfd;
	//int maxfd, sockfd;
	SOCKET listenfd, maxfd, sockfd;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len = sizeof(cliaddr);
	char buf[256], strip[16];
	int i, optval = 1;
	//const char *optval = 1;
	int client[FD_SETSIZE];
	fd_set rset, allset;
	int nready = 0; // select
	int maxindex = -1;
	//1,创建socket
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	//2，绑定ip和port
	//bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = ::htons(SERV_PORT);
	servaddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	//设置端口复用
	::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
	::bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	//3，监听Listen
	listen(listenfd, 128);
	//4，设置描述符集合
	FD_ZERO(&rset);
	FD_SET(listenfd, &rset);
	set_nonblocking(listenfd, true);
	//副本
	maxfd = listenfd;
	allset = rset;
	for (i = 0; i < FD_SETSIZE; i++)
	{
		client[i] = -1;
	}
	while (1)
	{
		rset = allset;
		//5，阻塞select事件
		struct timeval tem_timeout;
		tem_timeout.tv_sec = 3;
		tem_timeout.tv_usec = 0;
		nready = ::select(maxfd + 1, &rset, NULL, NULL, (const timeval *)&tem_timeout);
		if (nready < 0)
		{
			perror("select error");
		}
		//5.1 判断是否client链接
		if (::FD_ISSET(listenfd, &rset))
		{
			int connfd = ::accept(listenfd, (struct sockaddr *)&cliaddr, &len);
			printf("client ip:%s, port:%d\n",
				::inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, strip, sizeof(strip)),
				::ntohs(cliaddr.sin_port));
			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					client[i] = connfd; //记录client描述符
					break;
				}
			}
			if (i == FD_SETSIZE)
			{
				fputs("too manys clients\n", stderr);
				exit(1);
			}
			printf("i:%d\n", i);
			FD_SET(connfd, &allset); //client的描述符添加到集合中
			if (i > maxindex)
			{
				maxindex = i;
			}

			if (maxfd < connfd)
			{
				maxfd = connfd; //最大描述符数量
			}
			printf("maxindex:%d\n", maxindex);
			printf("maxfd:%d\n", maxfd);
			printf("nready:%d\n", nready);
			if (--nready == 0)
			{
				continue;
			}
		}
		//5.2 read信息的发送
		for (i = 0; i <= maxindex; i++)
		{
			printf("client[%d] = %d\n", i, client[i]);
			if ((sockfd = client[i]) < 0)
			{
				continue;
			}
			if (FD_ISSET(sockfd, &rset)) //有client发送信息
			{
				memset(buf, 0, 256);
				int ret = recv(sockfd, buf, sizeof(buf), 0);
				if (ret < 0)
				{
					perror("Read error");
					client[i] = -1;
					closesocket(sockfd);
					FD_CLR(sockfd, &allset); ///去除描述符
				}
				else if (ret == 0) //client 退出
				{
					client[i] = -1;
					closesocket(sockfd);
					FD_CLR(sockfd, &allset);
				}
				else
				{
					printf("client --> [%s] \n", buf);
					int j;
					/*for (j = 0; j < ret; j++)
					{
						buf[j] = toupper(buf[j]);
					}*/
					send(sockfd, buf, ret, 0);
				}
				if (--nready == 0)  //处理完成返回
				{
					break;
				}
			}
		}
	}
	closesocket(listenfd);
	win_socket_clear();
	
	return 0;
}







