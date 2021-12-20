#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<string>
#include<cstring>
#include<WS2tcpip.h>
#include <WinSock2.h>
#include<Windows.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll


using namespace std;

//定义结构体进行传输
typedef struct node{
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




int main()
{
	cout << "-----------客户端-----------" << endl;

	//1、初始化
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);


	//2、创建套接字
	SOCKET clientSocket = {};
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == clientSocket) {
		cout << "套接字闯创建失败!" << endl;
	}
	else {
		cout << "套接字创建成功!" << endl;
	}


	//3、绑定套接字	指定绑定的IP地址和端口号
	sockaddr_in socketAddr;
	socketAddr.sin_family = PF_INET;
	socketAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	socketAddr.sin_port = htons(1234);
	int cRes = connect(clientSocket, (SOCKADDR*)&socketAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == cRes) {
		cout << "客户端:与服务器连接失败....." << endl;
	}
	else {
		cout << "客户端:与服务器连接成功....." << endl;
	}

	while (true) {
		//4、发送请求(数据)
		char cmdBuf[128] = {};
		cout << "输入命令：";
		cin >> cmdBuf;
		/*string s;
		cout << "输入请求：" ;
		getline(cin, s);
		send(clientSocket, (char*)s.c_str(), s.length(), 0);*/

		/*STUDENT student;
		cout << "输入学生信息：(学号-姓名-年龄-班级)" << endl;
		cin >> student.num >> student.name >> student.age >> student.classname;
		send(clientSocket, (char*)&student, sizeof(student), 0);*/

		////5、接受数据
		//if (0 == strcmp("获取版本信息", s.c_str())) {
		//	char recvBuf[1024] = {};
		//	recv(clientSocket, recvBuf, 1024, 0);
		//	cout << "客户端接收数据：" << recvBuf << endl;
		//}

		//5、处理请求
		if (0 == strcmp("exit", cmdBuf)) {
			break;
		}
		if (0 == strcmp("登录", cmdBuf)) {
			Login login = { "kuang","834317210" };//包体
			DataHeader header = {};
			header.dataLength = sizeof(Login);
			header.cmd = CMD_LOGIN;

			//向服务器发送请求，先发包头再发包体
			send(clientSocket, (char*)&header, sizeof(DataHeader), 0);
			send(clientSocket, (char*)&login, sizeof(Login), 0);

			//接受服务器返回数据，先接包头再接包体
			DataHeader returnheader = {};
			LoginResult loginresult = {};
			recv(clientSocket, (char*)&returnheader, sizeof(DataHeader), 0);
			recv(clientSocket, (char*)&loginresult, sizeof(LoginResult), 0);
			cout << "登录结果：" << loginresult.result << endl;
		}
		else if (0 == strcmp("退出", cmdBuf)) {
			Logout logout = {"kuang"};//包体
			DataHeader header = {};
			header.dataLength = sizeof(Logout);
			header.cmd = CMD_LOGOUT;

			//向服务器发送请求，先发包头再发包体
			send(clientSocket, (char*)&header, sizeof(DataHeader), 0);
			send(clientSocket, (char*)&logout, sizeof(Login), 0);

			//接受服务器返回数据，先接包头再接包体
			DataHeader returnheader = {};
			LoginResult logoutresult = {};
			recv(clientSocket, (char*)&returnheader, sizeof(DataHeader), 0);
			recv(clientSocket, (char*)&logoutresult, sizeof(LoginResult), 0);
			cout << "退出结果：" << logoutresult.result << endl;
		}
		else {
			cout << "目前没有该命令....." << endl;
		}

		
	}
	

	

	//6、关闭socket
	closesocket(clientSocket);

	//7、终止
	WSACleanup();

	cout << "客户端退出" << endl;
	cin.get();//让程序停留在运行界面
	return 0;
}