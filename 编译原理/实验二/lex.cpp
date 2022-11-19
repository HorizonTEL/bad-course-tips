#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iomanip>
using namespace std;
const int maxn = 1e4;
const int maxlen = 1e4;
char operate[11][10] = { "+","-","*","/",">","<","=","(",")",";" ,"|"};
char keyword[5][10] = { "if","then","else","while","do" };

class lexical {
private:
	static const int rowope = 11;//操作符个数	
	static const int rowkey = 5;//关键字个数
	struct Twotuples {
		char kind[10];
		char proper[10];
	}tuples[maxlen];
	string filename;
public:
	bool isCal(char *s, int &length) {
		char sub[100];
		for (int i = 0; i < rowope; i++) {
			int len = strlen(operate[i]);
			strncpy(sub, s, len);
			sub[len] = '\0';
			if (strcmp(sub, operate[i]) == 0) {
				length = len;
				return true;
			}
		}
		return false;
	}
	bool isKey(char *s, int &length) {
		char sub[100];
		int num = 0;
		while (isalpha(*(s + num))) {
			num++;
		}
		length = num;
		for (int i = 0; i < rowkey; i++) {
			strncpy(sub, s, length);
			sub[length] = '\0';
			if (strcmp(sub, keyword[i]) == 0 && !isalpha(*(s + length))) {
				return true;
			}
		}
		return false;
	}
	bool isTen(char *s, int &length) {
		if (s[0] == '0'&&!isdigit(s[1]) && s[1] != 'x') {
			length = 1;
			return true;
		}
		else
			if (s[0] >= '1'&&s[0] <= '9') {
				int num = 0;
				while (isdigit(*(s + num))) {
					num++;
				}
				length = num;
				return true;
			}
		return false;
	}
	bool isEight(char *s, int &length) {
		if (s[0] == '0' && (s[1] >= '1'&&s[1] <= '7')) {
			int num = 0;
			while (s[num] >= '0'&&s[num] <= '7') {
				num++;
				cout<<"1";
			}
			length = num;
			return true;
		}
		return false;
	}
	bool isSixteen(char *s, int &length) {
		if (s[0] == '0' && s[1] == 'x' && ((s[2] >= '0'&&s[2] <= '9') || (s[2] >= 'a'&&s[2] <= 'f'))) {
			int num = 2;
			while ((s[num] >= '0'&&s[num] <= '9') || (s[num] >= 'a'&&s[num] <= 'f')) {
				num++;
				cout<<"2";
			}
			length = num;
			return true;
		}
		return false;
	}
	void scan(char *str, int &p1, int &p2) {
		int len = 0;
		char str_[] = "-";
		char sub[100];
		//运算符和界符
		if (isCal(str + p1, len)) {

			strncpy(sub, str + p1, len);
			sub[len] = '\0';
			strcpy(tuples[p2].kind, sub);
			strcpy(tuples[p2].proper, str_);
			p1 += len;
			p2++;
		}
		//关键字
		if (isKey(str + p1, len)) {
			strncpy(sub, str + p1, len);
			sub[len] = '\0';

			strcpy(tuples[p2].kind, sub);
			strcpy(tuples[p2].proper, str_);
			p1 += len;
			p2++;
		}
		//标识符
		if (isalpha(*(str + p1))) {
			int len = 0;
			while (isalpha(*(str + p1 + len)) || isdigit(*(str + p1 + len))) {
				len++;
				cout<<"3";
			}
			strncpy(sub, str + p1, len);
			sub[len] = '\0';
			strcpy(tuples[p2].kind, "0");
			strcpy(tuples[p2].proper, sub);
			p1 += len;
			p2++;
		}
		//十进制数字

		if (isTen(str + p1, len)) {
			strncpy(sub, str + p1, len);
			sub[len] = '\0';

			strcpy(tuples[p2].kind, "1");
			strcpy(tuples[p2].proper, sub);
			p1 += len;
			p2++;
		}
		if (isEight(str + p1, len)) {
			strncpy(sub, str + p1 + 1, len - 1);
			sub[len - 1] = '\0';
			strcpy(tuples[p2].kind, "2");
			strcpy(tuples[p2].proper, sub);
			p1 += len;
			p2++;
		}
		if (isSixteen(str + p1, len)) {
			strncpy(sub, str + p1 + 2, len - 2);
			sub[len - 2] = '\0';

			strcpy(tuples[p2].kind, "3");
			strcpy(tuples[p2].proper, sub);
			p1 += len;
			p2++;
		}

	}
	lexical(string inputfile , string outputfile) {
		this->filename = inputfile;
		char *buffer = new char[maxlen];
		ifstream in(filename);
		if (!in.is_open()) {
			cout << "文件打开失败" << endl;
			exit(1);
		}
		in.getline(buffer, maxlen, '#');
		int len = strlen(buffer);
		bool flagend = false;
		for (int i = 0; i < strlen(buffer); i++) {
			if (buffer[i] == '#') {
				flagend = true;
				break;
			}
		}
		if (!flagend)buffer[len++] = '#';
		buffer[len] = '\0';
		cout << buffer << endl;
		int buf_ptr = 0;
		int tup_ptr = 0;


		while (true) {
			if (buffer[buf_ptr] == '#')break;
			if (buffer[buf_ptr] == ' ' || buffer[buf_ptr] == '\n') {
				buf_ptr++;
				continue;
			}
			if (buffer[buf_ptr] == '\t') {
				buf_ptr += 4;
				continue;
			}
			scan(buffer, buf_ptr, tup_ptr);
		}
		cout.setf(std::ios::left);
		ofstream out(outputfile);
		for (int i = 0; i < tup_ptr; i++) {
			out  << "<" << setw(5) << tuples[i].kind << "," << setw(5) << tuples[i].proper << ">" << endl;
			cout << "<" << setw(5) << tuples[i].kind << "," << setw(5) << tuples[i].proper << ">" << endl;
		}
		
		
	}
};
int main()
{
	string filename1 = "C:\\Users\\Administrator\\Desktop\\实验二\\parse_test1.txt";
	string filename2= "DC:\\Users\\Administrator\\Desktop\\实验二\\out1.txt";
	lexical *text = new lexical(filename1,filename2);
	system("pause");
	return 0;
}

