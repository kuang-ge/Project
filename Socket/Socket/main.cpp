#define _WINSOCK_DEPRECATED_NO_WARNINGS //屏蔽警告（因版本原因）

#include <stdio.h> //标准输入输出头文件
#include <stdlib.h> //标准库头文件 包含常用函数（malloc()、free()等）
#include <iostream>
#include <string>
#include <cstring>

#include <WS2tcpip.h>
#include <WinSock2.h>//一般放在windows.h前面，避免有错误
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")//包含最新版本Socket

using namespace std;

//定义结构体进行传输
typedef struct node {
	int num;
	char name[50];
	int age;
	char classname[10];
}STUDENT;

//网络数据报文 包括包头(大小)和包体(数据)

enum CMD  //消息枚举
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};
//消息头
struct DataHeader
{
	short dataLength;    //数据长度 32767字节
	short cmd;
};

struct Login
{
	char userName[32];
	char Password[32];
};
struct Logout
{
	char userName[32];
};
struct LoginResult
{
	int result;
};
struct LogoutResult
{
	int result;
};

int main() {
	cout << "---------服务器---------" << endl;

	//1、初始化
	WSADATA wsadata;//该数据结构用来存储返回的socket数据
	WSAStartup(MAKEWORD(2, 2), &wsadata);//进行相应的socket库绑定

	//2、创建服务器的套接字
	SOCKET serviceSocket;
	serviceSocket = socket(AF_INET, SOCK_STREAM, 0);//参数(协议族，传输方式，某个协议(默认为0))
	if (SOCKET_ERROR == serviceSocket) {
		cout << "套接字创建失败" << endl;
	}
	else {
		cout << "套接字创建成功" << endl;
	}

	//3、绑定套接字 指定端口号和ip地址
	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//inet_addr将ip地址字符串转换为整数值
	socketAddr.sin_port = htons(1234);//htons网络字节序与主机字节序之间转换的函数，转换为网络字节序(规定的)，便于通信
	int bRes = bind(serviceSocket, (SOCKADDR*)&socketAddr, sizeof(SOCKADDR));//(套接字,结构体存放端口等参数,结构体长度)
	if (SOCKET_ERROR == bRes) {
		cout << "绑定套接字失败" << endl;
	}
	else {
		cout << "绑定套接字成功" << endl;
	}


	//4、服务器监听
	int lLen = listen(serviceSocket, 5);//参数5为可监听的客户端请求
	if (SOCKET_ERROR == lLen) {
		cout << "监听失败" << endl;
	}
	else {
		cout << "监听成功" << endl;
	}

	//select模型 改造（一对多）
	typedef struct fd_set {
		u_int fd_count;//set数量
		SOCKET fd_array[FD_SETSIZE];//SOCKET数组
	}FD_SET;



	//5、接受请求
	sockaddr_in recvClientAddr;
	SOCKET recvClientSocket = INVALID_SOCKET;//初始化一个接受的客户端SOCKET
	int _recvSize = sizeof(sockaddr_in);

	recvClientSocket = accept(serviceSocket, (SOCKADDR*)&recvClientAddr, &_recvSize);//参数(服务器套接字，客户端套接字地址，结构体大小)
	if (INVALID_SOCKET == recvClientSocket) {
		cout << "服务端接受请求失败" << endl;
	}
	else {
		cout << "服务端接受请求成功" << endl;
	}


	//循环接发数据
	while (true) {
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExpect;

		FD_ZERO(&fdRead);		//清空fd集合的数据
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExpect);
		//这个宏的功能是 将服务端的_sock 放到fdRead这个集合中 
		//当socket在listen状态，如果已经接收一个连接请求，这个socket会被标记为可读，例如一个accept会确保不会阻塞的完成
		//对于其他的socket，可读性意味着队列中的数据适合读，当调用recv后不会阻塞。
		FD_SET(_sock, &fdRead);  //将服务端的socket放入可读列表，确保accept不阻塞
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExpect);

		for (size_t n = 0; n < g_clinets.size(); n++)
		{
			FD_SET(g_clinets[n], &fdRead);		//所有连入的客户端放入可读列表 保证recv不阻塞
		}

		//nfds第一个参数 是一个整数值 是指fd_set集合中所有socket值的范围 不是数量 
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExpect, NULL);
		if (ret < 0)
		{
			cout << "select任务结束" << endl;
			break;
		}
		if (FD_ISSET(_sock, &fdRead))	//判断_sock是否在fdRead中
		{
			FD_CLR(_sock, &fdRead);
			//	4. 等待接受客户端连接 accept
			sockaddr_in _clientAddr = {};
			int cliendAddrLen = sizeof(_clientAddr);
			SOCKET _clientSock = INVALID_SOCKET; // 初始化无效的socket 用来存储接入的客户端

			_clientSock = accept(_sock, (sockaddr*)&_clientAddr, &cliendAddrLen);//当客户端接入时 会得到连入客户端的socket地址和长度
			if (INVALID_SOCKET == _clientSock) //接受到无效接入
			{
				cout << "ERROR: 接受到无效客户端SOCKET..." << endl;
			}
			else
			{
				cout << "新Client加入：" << "socket = " << _clientSock << " IP = " << inet_ntoa(_clientAddr.sin_addr) << endl;  //inet_ntoa 将ip地址转换成可读的字符串
			}
			g_clinets.push_back(_clientSock);
		}

		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (processor(fdRead.fd_array[n]) == -1)//processor函数是处理命令的逻辑 recv接到的数据并做出相应的判断和输出日志
			{
				auto it = find(g_clinets.begin(), g_clinets.end(), fdRead.fd_array[n]);
				if (it != g_clinets.end())
					g_clinets.erase(it);
			}
		}
		




		//6、发送接收数据
		DataHeader header = {};

		//STUDENT student = {};//传输结构体

		//char recvBuf[1024] = {};//传输字符

		//int recvLen = recv(recvClientSocket, recvBuf, 1024, 0);//接受消息（接收端套接字,缓冲区,长度，默认0）
		//int stuRecvLen = recv(recvClientSocket, (char*)&student, sizeof(student), 0);
		int hlen= recv(recvClientSocket, (char*)&header, sizeof(header), 0);
		if (hlen <= 0) {
			cout << "客户端退出，任务结束" << endl;
			break;
		}
		/*cout << "收到命令：" << (int)header.cmd << endl;
		cout << "数据长度：" << header.dataLength << endl;*/

		switch (header.cmd) {
		case CMD_LOGIN: {
			//接受客户端用户名和密码
			Login login = {};
			recv(recvClientSocket, (char*)&login, sizeof(login), 0);
			//进行判断

			//匹配之后发送登录结果 包头包体
			LoginResult loginresult = { 1 };
			send(recvClientSocket, (char*)&header, sizeof(DataHeader), 0);
			send(recvClientSocket, (char*)&loginresult, sizeof(LoginResult), 0);
			cout << "登录用户：" << login.userName << endl;

		}break;
		case CMD_LOGOUT: {
			Logout logout = {};
			recv(recvClientSocket, (char*)&logout, sizeof(Logout), 0);
			LogoutResult logoutresult = { 1 };
			send(recvClientSocket, (char*)&header, sizeof(DataHeader), 0);
			send(recvClientSocket, (char*)&logoutresult, sizeof(LogoutResult), 0);
			cout << "用户" << logout.userName << "退出" << endl;
		}break;
		default:
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(recvClientSocket, (char*)&header, sizeof(DataHeader), 0);


		}

		//if (SOCKET_ERROR != stuRecvLen) {
		//	cout << "接受的数据：" << endl;
		//	cout << "学号：" << student.num << endl;
		//	cout << "姓名：" << student.name << endl;
		//	cout << "年龄：" << student.age << endl;
		//	cout << "班级：" << student.classname << endl;
		//}

		//if (SOCKET_ERROR == recvLen) {
		//	cout << "服务端接收数据失败！" << endl;
		//	
		//}
		//else {
		//	cout << "客户端发送的请求：" << recvBuf << endl;
		//	if (0 == strcmp("cls", recvBuf)) {
		//		system(recvBuf);
		//	}
		//	else if (0 == strcmp("获取版本信息", recvBuf)) {
		//		string verdata = "version:1.0.1";
		//		int sLen = send(recvClientSocket, (char*)verdata.c_str(), verdata.length(), 0);//发送消息（发送端套接字，缓冲区，实际发送字节，默认0）
		//	}
		//	else if (0 == strcmp("exit", recvBuf)) {
		//		cout << "退出服务器" << endl;
		//		break;
		//	}
		//	else {
		//		
		//	}
		//}
		cout << endl;
	}
	

	//7、关闭socket
	closesocket(recvClientSocket);
	closesocket(serviceSocket);

	//8、终止
	WSACleanup();
	cout << "服务器停止" << endl;
	cin.get();
	return 0;



}

