#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
//using namespace std;
#pragma comment(lib,"ws2_32.lib")
#define PORT 8888
int main(int argc, char *argv[])
{
	//begin 初始化网络环境
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {            //加载winsock2库
		printf("加载失败/n");
		return -1;
	}
	//end  初始化网络环境


	struct sockaddr_in addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(PORT);
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");  //要连接的服务器的ip地址

	
	SOCKET sockc = socket(AF_INET, SOCK_STREAM, 0);                    //建立客户端流式套接口
	connect(sockc, (struct sockaddr *)&addrSrv, sizeof(addrSrv));      //与服务器进行连接
	char receive[250] = { 0 };
	recv(sockc, receive, 250, 0);                                  // 接收客户端数据
	printf("%s\n", receive);
	send(sockc, "hello world", strlen("hello world") + 1, 0);
	closesocket(sockc);
	system("pause");
	return 0;
}