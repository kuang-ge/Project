#define _WINSOCK_DEPRECATED_NO_WARNINGS //���ξ��棨��汾ԭ��

#include <stdio.h> //��׼�������ͷ�ļ�
#include <stdlib.h> //��׼��ͷ�ļ� �������ú�����malloc()��free()�ȣ�
#include <iostream>
#include <string>
#include <cstring>

#include <WS2tcpip.h>
#include <WinSock2.h>//һ�����windows.hǰ�棬�����д���
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")//�������°汾Socket

using namespace std;

//����ṹ����д���
typedef struct node {
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

int main() {
	cout << "---------������---------" << endl;

	//1����ʼ��
	WSADATA wsadata;//�����ݽṹ�����洢���ص�socket����
	WSAStartup(MAKEWORD(2, 2), &wsadata);//������Ӧ��socket���

	//2���������������׽���
	SOCKET serviceSocket;
	serviceSocket = socket(AF_INET, SOCK_STREAM, 0);//����(Э���壬���䷽ʽ��ĳ��Э��(Ĭ��Ϊ0))
	if (SOCKET_ERROR == serviceSocket) {
		cout << "�׽��ִ���ʧ��" << endl;
	}
	else {
		cout << "�׽��ִ����ɹ�" << endl;
	}

	//3�����׽��� ָ���˿ںź�ip��ַ
	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//inet_addr��ip��ַ�ַ���ת��Ϊ����ֵ
	socketAddr.sin_port = htons(1234);//htons�����ֽ����������ֽ���֮��ת���ĺ�����ת��Ϊ�����ֽ���(�涨��)������ͨ��
	int bRes = bind(serviceSocket, (SOCKADDR*)&socketAddr, sizeof(SOCKADDR));//(�׽���,�ṹ���Ŷ˿ڵȲ���,�ṹ�峤��)
	if (SOCKET_ERROR == bRes) {
		cout << "���׽���ʧ��" << endl;
	}
	else {
		cout << "���׽��ֳɹ�" << endl;
	}


	//4������������
	int lLen = listen(serviceSocket, 5);//����5Ϊ�ɼ����Ŀͻ�������
	if (SOCKET_ERROR == lLen) {
		cout << "����ʧ��" << endl;
	}
	else {
		cout << "�����ɹ�" << endl;
	}

	//selectģ�� ���죨һ�Զࣩ
	typedef struct fd_set {
		u_int fd_count;//set����
		SOCKET fd_array[FD_SETSIZE];//SOCKET����
	}FD_SET;



	//5����������
	sockaddr_in recvClientAddr;
	SOCKET recvClientSocket = INVALID_SOCKET;//��ʼ��һ�����ܵĿͻ���SOCKET
	int _recvSize = sizeof(sockaddr_in);

	recvClientSocket = accept(serviceSocket, (SOCKADDR*)&recvClientAddr, &_recvSize);//����(�������׽��֣��ͻ����׽��ֵ�ַ���ṹ���С)
	if (INVALID_SOCKET == recvClientSocket) {
		cout << "����˽�������ʧ��" << endl;
	}
	else {
		cout << "����˽�������ɹ�" << endl;
	}


	//ѭ���ӷ�����
	while (true) {
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExpect;

		FD_ZERO(&fdRead);		//���fd���ϵ�����
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExpect);
		//�����Ĺ����� ������˵�_sock �ŵ�fdRead��������� 
		//��socket��listen״̬������Ѿ�����һ�������������socket�ᱻ���Ϊ�ɶ�������һ��accept��ȷ���������������
		//����������socket���ɶ�����ζ�Ŷ����е������ʺ϶���������recv�󲻻�������
		FD_SET(_sock, &fdRead);  //������˵�socket����ɶ��б�ȷ��accept������
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExpect);

		for (size_t n = 0; n < g_clinets.size(); n++)
		{
			FD_SET(g_clinets[n], &fdRead);		//��������Ŀͻ��˷���ɶ��б� ��֤recv������
		}

		//nfds��һ������ ��һ������ֵ ��ָfd_set����������socketֵ�ķ�Χ �������� 
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExpect, NULL);
		if (ret < 0)
		{
			cout << "select�������" << endl;
			break;
		}
		if (FD_ISSET(_sock, &fdRead))	//�ж�_sock�Ƿ���fdRead��
		{
			FD_CLR(_sock, &fdRead);
			//	4. �ȴ����ܿͻ������� accept
			sockaddr_in _clientAddr = {};
			int cliendAddrLen = sizeof(_clientAddr);
			SOCKET _clientSock = INVALID_SOCKET; // ��ʼ����Ч��socket �����洢����Ŀͻ���

			_clientSock = accept(_sock, (sockaddr*)&_clientAddr, &cliendAddrLen);//���ͻ��˽���ʱ ��õ�����ͻ��˵�socket��ַ�ͳ���
			if (INVALID_SOCKET == _clientSock) //���ܵ���Ч����
			{
				cout << "ERROR: ���ܵ���Ч�ͻ���SOCKET..." << endl;
			}
			else
			{
				cout << "��Client���룺" << "socket = " << _clientSock << " IP = " << inet_ntoa(_clientAddr.sin_addr) << endl;  //inet_ntoa ��ip��ַת���ɿɶ����ַ���
			}
			g_clinets.push_back(_clientSock);
		}

		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (processor(fdRead.fd_array[n]) == -1)//processor�����Ǵ���������߼� recv�ӵ������ݲ�������Ӧ���жϺ������־
			{
				auto it = find(g_clinets.begin(), g_clinets.end(), fdRead.fd_array[n]);
				if (it != g_clinets.end())
					g_clinets.erase(it);
			}
		}
		




		//6�����ͽ�������
		DataHeader header = {};

		//STUDENT student = {};//����ṹ��

		//char recvBuf[1024] = {};//�����ַ�

		//int recvLen = recv(recvClientSocket, recvBuf, 1024, 0);//������Ϣ�����ն��׽���,������,���ȣ�Ĭ��0��
		//int stuRecvLen = recv(recvClientSocket, (char*)&student, sizeof(student), 0);
		int hlen= recv(recvClientSocket, (char*)&header, sizeof(header), 0);
		if (hlen <= 0) {
			cout << "�ͻ����˳����������" << endl;
			break;
		}
		/*cout << "�յ����" << (int)header.cmd << endl;
		cout << "���ݳ��ȣ�" << header.dataLength << endl;*/

		switch (header.cmd) {
		case CMD_LOGIN: {
			//���ܿͻ����û���������
			Login login = {};
			recv(recvClientSocket, (char*)&login, sizeof(login), 0);
			//�����ж�

			//ƥ��֮���͵�¼��� ��ͷ����
			LoginResult loginresult = { 1 };
			send(recvClientSocket, (char*)&header, sizeof(DataHeader), 0);
			send(recvClientSocket, (char*)&loginresult, sizeof(LoginResult), 0);
			cout << "��¼�û���" << login.userName << endl;

		}break;
		case CMD_LOGOUT: {
			Logout logout = {};
			recv(recvClientSocket, (char*)&logout, sizeof(Logout), 0);
			LogoutResult logoutresult = { 1 };
			send(recvClientSocket, (char*)&header, sizeof(DataHeader), 0);
			send(recvClientSocket, (char*)&logoutresult, sizeof(LogoutResult), 0);
			cout << "�û�" << logout.userName << "�˳�" << endl;
		}break;
		default:
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(recvClientSocket, (char*)&header, sizeof(DataHeader), 0);


		}

		//if (SOCKET_ERROR != stuRecvLen) {
		//	cout << "���ܵ����ݣ�" << endl;
		//	cout << "ѧ�ţ�" << student.num << endl;
		//	cout << "������" << student.name << endl;
		//	cout << "���䣺" << student.age << endl;
		//	cout << "�༶��" << student.classname << endl;
		//}

		//if (SOCKET_ERROR == recvLen) {
		//	cout << "����˽�������ʧ�ܣ�" << endl;
		//	
		//}
		//else {
		//	cout << "�ͻ��˷��͵�����" << recvBuf << endl;
		//	if (0 == strcmp("cls", recvBuf)) {
		//		system(recvBuf);
		//	}
		//	else if (0 == strcmp("��ȡ�汾��Ϣ", recvBuf)) {
		//		string verdata = "version:1.0.1";
		//		int sLen = send(recvClientSocket, (char*)verdata.c_str(), verdata.length(), 0);//������Ϣ�����Ͷ��׽��֣���������ʵ�ʷ����ֽڣ�Ĭ��0��
		//	}
		//	else if (0 == strcmp("exit", recvBuf)) {
		//		cout << "�˳�������" << endl;
		//		break;
		//	}
		//	else {
		//		
		//	}
		//}
		cout << endl;
	}
	

	//7���ر�socket
	closesocket(recvClientSocket);
	closesocket(serviceSocket);

	//8����ֹ
	WSACleanup();
	cout << "������ֹͣ" << endl;
	cin.get();
	return 0;



}

