#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<string>
#include<cstring>
#include<WS2tcpip.h>
#include <WinSock2.h>
#include<Windows.h>
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll


using namespace std;

//����ṹ����д���
typedef struct node{
	int num;
	char name[50];
	int age;
	char classname[10];
}STUDENT;

//�������ݱ��� ������ͷ(��С)�Ͱ���(����)

enum CMD  //��Ϣö��
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};
//��Ϣͷ
struct DataHeader
{
	short dataLength;    //���ݳ��� 32767�ֽ�
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
	cout << "-----------�ͻ���-----------" << endl;

	//1����ʼ��
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);


	//2�������׽���
	SOCKET clientSocket = {};
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == clientSocket) {
		cout << "�׽��ִ�����ʧ��!" << endl;
	}
	else {
		cout << "�׽��ִ����ɹ�!" << endl;
	}


	//3�����׽���	ָ���󶨵�IP��ַ�Ͷ˿ں�
	sockaddr_in socketAddr;
	socketAddr.sin_family = PF_INET;
	socketAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	socketAddr.sin_port = htons(1234);
	int cRes = connect(clientSocket, (SOCKADDR*)&socketAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == cRes) {
		cout << "�ͻ���:�����������ʧ��....." << endl;
	}
	else {
		cout << "�ͻ���:����������ӳɹ�....." << endl;
	}

	while (true) {
		//4����������(����)
		char cmdBuf[128] = {};
		cout << "�������";
		cin >> cmdBuf;
		/*string s;
		cout << "��������" ;
		getline(cin, s);
		send(clientSocket, (char*)s.c_str(), s.length(), 0);*/

		/*STUDENT student;
		cout << "����ѧ����Ϣ��(ѧ��-����-����-�༶)" << endl;
		cin >> student.num >> student.name >> student.age >> student.classname;
		send(clientSocket, (char*)&student, sizeof(student), 0);*/

		////5����������
		//if (0 == strcmp("��ȡ�汾��Ϣ", s.c_str())) {
		//	char recvBuf[1024] = {};
		//	recv(clientSocket, recvBuf, 1024, 0);
		//	cout << "�ͻ��˽������ݣ�" << recvBuf << endl;
		//}

		//5����������
		if (0 == strcmp("exit", cmdBuf)) {
			break;
		}
		if (0 == strcmp("��¼", cmdBuf)) {
			Login login = { "kuang","834317210" };//����
			DataHeader header = {};
			header.dataLength = sizeof(Login);
			header.cmd = CMD_LOGIN;

			//����������������ȷ���ͷ�ٷ�����
			send(clientSocket, (char*)&header, sizeof(DataHeader), 0);
			send(clientSocket, (char*)&login, sizeof(Login), 0);

			//���ܷ������������ݣ��ȽӰ�ͷ�ٽӰ���
			DataHeader returnheader = {};
			LoginResult loginresult = {};
			recv(clientSocket, (char*)&returnheader, sizeof(DataHeader), 0);
			recv(clientSocket, (char*)&loginresult, sizeof(LoginResult), 0);
			cout << "��¼�����" << loginresult.result << endl;
		}
		else if (0 == strcmp("�˳�", cmdBuf)) {
			Logout logout = {"kuang"};//����
			DataHeader header = {};
			header.dataLength = sizeof(Logout);
			header.cmd = CMD_LOGOUT;

			//����������������ȷ���ͷ�ٷ�����
			send(clientSocket, (char*)&header, sizeof(DataHeader), 0);
			send(clientSocket, (char*)&logout, sizeof(Login), 0);

			//���ܷ������������ݣ��ȽӰ�ͷ�ٽӰ���
			DataHeader returnheader = {};
			LoginResult logoutresult = {};
			recv(clientSocket, (char*)&returnheader, sizeof(DataHeader), 0);
			recv(clientSocket, (char*)&logoutresult, sizeof(LoginResult), 0);
			cout << "�˳������" << logoutresult.result << endl;
		}
		else {
			cout << "Ŀǰû�и�����....." << endl;
		}

		
	}
	

	

	//6���ر�socket
	closesocket(clientSocket);

	//7����ֹ
	WSACleanup();

	cout << "�ͻ����˳�" << endl;
	cin.get();//�ó���ͣ�������н���
	return 0;
}