#include <iostream>

using namespace std;

void transform(char& c, char& C) {
	C = c - 32;
}
int main() {
	while (1) {
	char char1;
	char char2;
	cout << "Сд��ĸ��";
	cin >> char1;
	transform(char1, char2);
	cout << char2 << endl;
	}

}