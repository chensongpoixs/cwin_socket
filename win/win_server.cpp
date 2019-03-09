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
		printf("加载失败./n");
		return -1;
	}
	//end  初始化网络环境

	struct sockaddr_in servaddr;                      //服务器端地址
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;                    //填写要连接的服务器地址信息
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);     //inet_addr()将命令行中输入的点分IP地址转换为二进制表示的网络字节序IP地址
	servaddr.sin_port = htons(PORT);

	SOCKET socks = socket(AF_INET, SOCK_STREAM, 0);      //建立客户端流式套接口
	if (socks == INVALID_SOCKET) {
		printf("建立接口失败/n");
		return -1;
	}

	bind(socks, (struct sockaddr *)&servaddr, sizeof(servaddr)); //与服务器端建立连接
	listen(socks, 10);                                         //将套接字置入监听模式并准备接受连接请求,请求10是连接队列的最大长度
	printf("正在连接......\n");

	struct sockaddr_in addrClient;

	int addrClient_len = sizeof(addrClient);
	while (1) {
		SOCKET sockc = accept(socks, (struct sockaddr *)&addrClient, &addrClient_len);        //建立套接字用于通信
		char sen[100] = { 0 };
		sprintf(sen, "%s", inet_ntoa(addrClient.sin_addr));          // 将客户端的IP地址存入c数组中
		send(sockc, sen, strlen(sen) + 1, 0);                        // 发送数据到客户端

		char receive[100] = { 0 };
		recv(sockc, receive, 100, 0);                                  // 接收客户端数据
		printf("%s\n", receive);

		closesocket(sockc);
	}
	closesocket(socks);
	system("pause");
	return 0;
}