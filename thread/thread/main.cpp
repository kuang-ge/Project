#include <thread>
#include <mutex>
#include <iostream>
using namespace std;

mutex m;

void fun() {
	for (int i = 0; i < 4; i++) {
		lock_guard<mutex> lg(m);//�Խ��� ���캯����������������
		//���߳�����ռʽִ�У����ٽ�����������ֹ����ִ��
		//Ƶ���ӽ��������������ģ������Խ������������÷�Χ�Զ�������
		cout << "hello" << i << endl;
	}
}

int main() {
	thread t[4];
	for (int i = 0; i < 4; i++) {
		t[i] = thread(fun);
	}
	return 0;
}