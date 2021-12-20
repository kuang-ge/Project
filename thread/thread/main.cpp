#include <thread>
#include <mutex>
#include <iostream>
using namespace std;

mutex m;

void fun() {
	for (int i = 0; i < 4; i++) {
		lock_guard<mutex> lg(m);//自解锁 构造函数加锁，析构解锁
		//多线程是抢占式执行，对临界区加锁，防止乱序执行
		//频繁加解锁对性能有消耗，故用自解锁（超出作用范围自动解锁）
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