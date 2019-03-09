头文件     #include <WinSock.h>

             #include <Winsock2.h> 用这个，这个是升级版

各个函数解释

1、WSAStartup：

初始化套接字环境，本函数必须是应用程序或DLL调用的第一个Windows Sockets函数.它允许应用程序或DLL指明Windows Sockets API的版本号及获得特定Windows Sockets实现的细节.应用程序或DLL只能在一次成功的WSAStartup()调用之后才能调用进一步的Windows Sockets API函数.


int WSAStartup(WORD  wVersionRequested,  LPWSADATA  lpWSAData);

实际用例

复制代码
//begin 初始化网络环境
int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
if ( err != 0)
{
    printf("WSAStartup failed with error: %d\n", err);
    return -1;
}//end
复制代码
2、WSACleanup：

清理套接字环境，和上面的WSAStartup相反，该函数是在程序不在对任何Windows Sockets函数调用后，用其来清理套接字环境的
int  WSACleanup (void);

 

3、SOCKET socket(int af, int type, int protocol);
建立套接字 参数af用于指定网络地址类型，一般取AF_INET，表示该套接字在Internet域中，进行通信。 参数type用于知道套接字的类型，若取SOCK_STREAM表示创建的套接字是流套接字，而取SOCK_DGRAM创建数字报套接字。参数protocol用于指定网络协议，一般取0，表示默认为TCP/IP协议。 若套接字创建成功则该函数返回创建的套接字句柄SOCKET，否则产生INVALID_SOCKET错误。
实际用例
复制代码
//begin socket 一个套接字
hTcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (INVALID_SOCKET == hTcpSocket)
{
MJS_LOG_ERROR("socket failed with error: \n");
WSACleanup();
return -1 ;
}//end
复制代码
4、int listen(SOCKET s, int backlog);

将套接字置入监听模式并准备接受连接请求。其中，参数s是服务器端套接字，于指定正在等待连接的最大队列长度。如无错误发生，listen函数返回0，失败则返回SOCKET_ERROR错误，应用程序可通过WSAGetLastError()获取相应错误代码。

 
5、SOCKET accept( SOCKET s, struct sockaddr* addr, int* addrlen );
参数s同上，addr是一个有效的SOCKADDR_IN结构的地址，而addrlen是sockaddr_in结果的长度。accept函数返回后，addr参数变量中会包含发出连接请求的那个客户机的IP地址信息，而addrlen参数则指出该结构的长度，并返回一个新的套接字描述符，它对应于已经接受的那个客户机连接。
 

6、 int bind( SOCKET s, const struct sockaddr* name, int namlen );
绑定到本地，name中指定的IP应该是当前运行该程序机器的IP。
 
 

7、int connect( SOCKET s, const struct sockaddr FAR* name, int namelen );
连接到服务器
实际用例
复制代码
int ServePort=20000;
char ServeIP[32]="192.168.1.200";
sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(ServePort);
addr.sin_addr.S_un.S_addr = inet_addr(ServeIP);
//连接服务器
if (connect(hTcpSocket, (sockaddr*)&addr, sizeof(sockaddr)) == -1)
{
    closesocket(hTcpSocket);
    printf("connect Tradesys failed with error%d,%s,%d: \n",ServePort,ServeIP,GetLastError());
    WSACleanup();
    return -1;
}
else
{
    printf("connect Tradesys sucessed,%d,%s: \n",ServePort,ServeIP);
}
复制代码
若无错误发生，则connect()返回0。否则的话，返回SOCKET_ERROR错误，应用程序可通过WSAGetLastError()获取相应错误代码。对阻塞套接口而言，若返回值为SOCKET_ERROR则应用程序调用WSAGetLsatError()。如果它指出错误代码为WSAEWOULDBLOCK，则您的应用程序可以：

1.用select()，通过检查套接口是否可写，来确定连接请求是否完成。或者，
2.如果您的应用程序使用基于消息的WSAAsynSelect()来表示对连接事件的兴趣，则当连接操作完成后，您会收到一个FD_CONNECT消息。
 
8、 int send( SOCKET s, const char* buf, int len, int flags );
失败时返回 -1/SOCKET_ERROR

       sockfd：已建立连接的发送端套接字描述符（非监听描述符）

       buf：应用要发送数据的缓存

       len：实际要发送的数据长度

       flags：一般设置为0。  flags可取的值有：0、MSG_DONTROUTE或MSG_OOB或这些标志的按位或运算。

 

9、int recv( SOCKET s, char* buf, int len, int flags );
s是准备接收数据的套接字，buf是即将收到数据的字符缓冲区，而len则是准备接受的字节数或buf缓冲的长度。flags一般指定为0.参数可以是0、MSG_PEEK或MSG_OOB或这些标志的按位“或”运算。
 表示从接收缓冲区拷贝数据。成功时，返回拷贝的字节数，失败返回-1。阻塞模式下，recv/recvfrom将会阻塞到缓冲区里至少有一个字节(TCP)/至少有一个完整的UDP数据报才返回，没有数据时处于休眠状态。若非阻塞，则立即返回，有数据则返回拷贝的数据大小，否则返回错误-1，置错误码为EWOULDBLOCK。

 

10、int shutdown( SOCKET s, int how );
其中，how参数用于描述禁止哪些操作，它可取的值有：SD_RECEIVE、SD_SEND或SD_BOTH。 如果是SD_RECEIVE，就表示不允许再调用接收函数， 如果选择SＤ_SEND，表示不允许再调用发送函数， 如果是SD_BOTH, 则表示取消连接两端的收发操作。 如果没有错误发生，shutdown()返回0，否则返回SOCKET_ERROR错误。
 
11、 int closesocket（SOCKET s ）;
s是要关闭的套接字描述字，再利用套接字执行调用就会失败。
 
12 int gethostname(char *name, size_t len):
这个函数，调用后，会将主机名保存在name里面。而len是name的大小。该函数返回0表示成功，否则失败。
 
13、struct hostent *gethostbyname(const char *name);
hostent结构： 
 复制代码
struct hostent 
{
     char *h_name;           //*h_name 表示的是主机的规范名
     char **h_aliases;       //h_aliases 表示的是主机的别名
     int h_addrtype;          //地址类型AF_INET，还是AF_INET6
     int h_length;             //IP地址占字节数
     char **h_addr_list;   //IP地址列表
};
复制代码
int   **h_addr_lisst 表示的是主机的ip地址 注意 是以网络字节序存储的。

 


14、const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt) ;

这个函数，是将类型为af的网络地址结构src，转换成主机序的字符串形式，存放在长度为cnt的字符串中。这个函数，其实就是返回指向dst的一个指针。如果函数调用错误，返回值是NULL。

 

15、struct hostent FAR *PASCAL FAR gethostbyaddr(const char FAR * addr, int len, int type);
参数addr：指向网络字节顺序地址的指针。 参数len： 地址的长度，在PF_INET类型地址中为4。 参数type：指地址类型AF_INET、IF_INET6
 

16、inet_ntoa( char FAR* PASCAL FAR inet_ntoa( struct in_addr in);
将网络地址转换成“.”点隔的字符串格式。本函数将一个用in参数所表示的Internet地址结构转换成以“.” 间隔的诸如“a.b.c.d”的字符串形式。请注意inet_ntoa()返回的字符串存放在WINDOWS套接口实现所分配的内存中。应用程序不应假设该内存是如何分配的。在同一个线程的下一个WINDOWS套接口调用前，数据将保证是有效。
参数in：一个表示Internet主机地址的结构。

返回值：若无错误发生，inet_ntoa()返回一个字符指针。否则的话，返回NVLL。其中的数据应在下一个WINDOWS套接口调用前复制出来。

 

17、unsigned long inet_addr(  const char   FAR *cp);
将点格式的IP地址转换为无符号类型存储。
 
18、u_short PASCAL FAR htons( u_short hostshort);
作用是将主机的无符号短整形数转换成网络字节顺序。
 
19、 u_short PASCAL FAR ntohs( u_short netshort);
 
20、u_long PASCAL FAR htonl( u_long hostlong);
本函数将一个32位数从主机字节顺序转换成网络字节顺序。
实际用例
复制代码
//将主机的unsigned long值转化为网络字节顺序（32位），使用函数htonl()
//参数hostlong标识主机字节顺序的数字，函数返回一个网络字节顺序的数字
#include <winsock.h>
#include <stdio.h>
#pragma comment (lib,"ws2_32.lib")
void main()
{
u_long a = 0x12345678;
u_long b = htonl(a);
printf("%u/n",a);
printf("%x/n",a);
printf("%u/n",b);
printf("%x/n",b);
}
复制代码
 

21、u_long PASCAL FAR ntohl( u_long netlong);
将一个无符号长整形数从网络字节顺序转换为主机字节顺序。
参数netlong：一个以网络字节顺序表达的32位数。
返回值：ntohl()返回一个以主机字节顺序表达的数。

复制代码
//将32位网络字节转换为主机字节，使用函数ntohl()
//定义如下 
u_long ntohl(u_long netlong);
#include <WINSOCK2.H>
#include 
<stdio.h>
#pragma comment(lib,"ws2_32.lib")
void 
main()
{
u_long a = 0x12345678;
u_long b = 
ntohl(a);
printf("%u/n",a);
printf("%x/n",a);
printf("%u/n",b);
printf("%x/n",b);
}
复制代码
 

 

 

PS：

错误代码：
WSANOTINITIALISED：在使用此API之前应首先成功地调用WSAStartup()。
WSAENETDOWN：WINDOWS套接口实现检测到网络子系统失效。
WSAEINVAL：超时时间值非法。
WSAEINTR：通过一个WSACancelBlockingCall()来取消一个（阻塞的）调用。
WSAEINPROGRESS：一个阻塞的WINDOWS套接口调用正在运行中。
WSAENOTSOCK：描述字集合中包含有非套接口的元素。
WSAEADDRINUSE：所指的地址已在使用中。
WSAEADDRNOTAVAIL：在本地机器上找不到所指的地址。
WSAENOTSUPPORT：所指族中地址无法与本套接口一起使用。
WSAECONNREFUSED：连接尝试被强制拒绝。
WSAEDESTADDREQ：需要目的地址。
WSAEFAULT：namelen参数不正确。
WSAEINVAL：套接口没有准备好与一地址捆绑。
WSAEISCONN：套接口早已连接。
WSAEMFILE：无多余文件描述字。
WSAENETUNREACH：当前无法从本主机访问网络。
WSAENOBUFS：无可用缓冲区。套接口未被连接。
WSAENOTSOCK：描述字不是一个套接口。
WSAETIMEOUT：超时时间到。
WSAEWOULDBLOCK：套接口设置为非阻塞方式且连接不能立即建立。可用select()调用对套接口写，因为select()时会进行连接。