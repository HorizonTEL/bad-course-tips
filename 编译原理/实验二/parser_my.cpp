#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include <iomanip>
#include <stack>
#include <cstring>
using namespace std;
const int maxnlen = 1e4;
class Grammar {
private:
	set<char> Vn;  // 非终结符集合
	set<char> Vt;  // 终结符集合
	char S;        // 开始符合
	map<char, set<string> > P;  // 产生式集合，key为非终结符，value为产生式集合
	map<char,set<char> > FIRST;
	map<char,set<char> > FOLLOW;
	map<string, string> Table;
public:
	Grammar(string filename) 
	{
		Vn.clear();
		Vt.clear();
		P.clear();
		FIRST.clear();
		FOLLOW.clear();
		ifstream in(filename);
		if (!in.is_open()) {
			cout << "文法文件打开失败" << endl;
			exit(1);
		}
		char *buffer = new char[maxnlen];
		in.getline(buffer, maxnlen, '#');
		string temps = "";
		bool is_sethead = 0;
		for (int i = 0; i < strlen(buffer); i++) 
		{
			if (buffer[i] == '\n' || buffer[i] == ' ')continue;
			if (buffer[i] == ';') 
			{
				if (!is_sethead) 
				{
					this->setHead(temps[0]);
					is_sethead = 1;
				}
				this->add(temps);
				temps = "";
			}
			else
				temps += buffer[i];
		}
		delete buffer;
		/*
			输出Vn，Vt，set			
		*/
	}

	// 设置文法的开始符号
	void setHead(char c) 
	{
		S = c;
	}

	// 加入Vn、Vt、P
	void add(string s) 
	{
		char s1 = s[0];
		string s2 = "";
		int num = 0;
		for (int i = 0; i < s.length() ; i++) 
		{
			if (s[i] == '>') num=i;
			if (num == 0) continue;
			if (i > num) s2 += s[i];
		}
		s2 += ';';
		Vn.insert(s1);
		string temp = "";
		for (int i = 0; i < s2.length() ; i++) 
		{
			char s = s2[i];
			if (!isupper(s) && s != '|' && s != ';' && s!='@') Vt.insert(s);
			if (s == '|' || s == ';') 
			{
				P[s1].insert(temp);
				temp = "";
			}
			else 
			{
				temp += s;
			}
		}
	}

	/*
		打印所有产生式
	*/
	void print() {
		cout << "当前分析文法为：" << endl << endl;
		for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
			char cur_s = *it;
			for (set<string>::iterator it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) {
				string cur_string = *it1;
				cout << cur_s << "->" << cur_string << endl;
			}
		}
	}

	/*
		计算FIRST集
	*/
	void getFirst() {
		FIRST.clear();
		// 判断迭代次数
		int iter = 4;
		while (iter--) {
			for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
				char cur_s = *it;
				/*请编程实现以下功能
				***************************************************************************************
				cur_s->cur_string[0]
					a加到A的FIRST集
				cur_s->cur_string[0]
					B的FITRST集加到A的FIRST集
				*/
				for (auto it0 = P[cur_s].begin(); it0 != P[cur_s].end(); it0++)
				{
					string left = *it0;
					if(!isupper(left[0])) 
					{
						FIRST[cur_s].insert(left[0]);
					}
					else 
					{
						for (auto it1 = FIRST[left[0]].begin(); it1 != FIRST[left[0]].end();it1++)
						{
							FIRST[cur_s].insert(*it1);
						}
					}
				}
			}
		}
		// 输出FIRST集
		cout << "FIRST集为：" << endl << endl;
		for (set<char>::iterator it = Vn.begin(); it != Vn.end();it++) 
		{
			char cur_s = *it;
			cout << "FIRST( " << cur_s << " ) = { ";
			for (set<char>::iterator it1 = FIRST[cur_s].begin(); it1 != FIRST[cur_s].end(); it1++) 
			{
				cout << *it1;
				it1++;
				auto t = it1;
				it1--;
				if(t != FIRST[cur_s].end())
				{
					cout << " , ";
				}
			}
			cout << " }" << endl;
		}
	}

	/*
		计算FOLLOW集
	*/
	void getFollow() 
	{
		FOLLOW.clear();
		FOLLOW[S].insert('#');
		// 判断迭代次数
		int iter = 4;
		while (iter--) {
			for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++)
			{
				char cur_s = *it;
				/*请编程实现以下功能
				***************************************************************************************
				cur_s->cur_string[0]
				a加到A的FIRST集
				cur_s->cur_string[0]
				B的FITRST集加到A的FIRST集
				*/
				for (set<string>::iterator it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) 
				{
					string cur_string = *it1;
					for (int i = 0; i < cur_string.length(); i++) 
					{
						if(!isupper(cur_string[i])) 
						{
							continue;
						}
						if(i == cur_string.length()-1)
						{
							/* 第四步：
								AC/ACK为最后两个或者三个
								B->AC
								B->ACK(K的first集含有@)
								将B的follow集加入到C的follow集
							*/				
							for (auto it2 = FOLLOW[cur_s].begin(); it2 != FOLLOW[cur_s].end();it2++)
							{
								FOLLOW[cur_string[i]].insert(*it2);
							}
						}
						else
						{
							int j;
							for (j = i + 1; j < cur_string.length(); j++)
							{
								char next = cur_string[i + 1];
								/*第一步：
									B->Ac
									将c加到A的follow集
								*/
								if (!isupper(next))
								{
									FOLLOW[cur_string[i]].insert(next);
									break;
								}						
								/*第二步：
									B->AC
									将C的first集加到A的follow集
								*/
								else if(FIRST[next].count('@') == 0)
								{
									for (auto it2 = FIRST[next].begin(); it2 != FIRST[next].end(); it2++)
									{
										FOLLOW[cur_string[i]].insert(*it2);
									}
									break;
								}
								/*第三步：
									遍历C的first去除@，加到A的follow集
								*/
								else
								{
									for (auto it2 = FIRST[next].begin(); it2 != FIRST[next].end();it2++)
									{
										if((*it2)!='@')
										{
											FOLLOW[cur_string[i]].insert(*it2);
										}
									}
								}
							}
							if(j == cur_string.length())
							{
								for (auto it2 = FOLLOW[cur_s].begin(); it2 != FOLLOW[cur_s].end();it2++)
								{
									FOLLOW[cur_string[i]].insert(*it2);
								}
							}
						}
					}
				}
			}
		}
		//输出FOLLOW集
		cout << "FOLLOW集为：" << endl << endl;
		for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) 
		{
			char cur_s = *it;
			cout << "FOLLOW( " << cur_s << " ) =  { ";
			for (set<char>::iterator it1 = FOLLOW[cur_s].begin(); it1 != FOLLOW[cur_s].end(); it1++) 
			{
				cout << *it1;
				it1++;
				auto t = it1;
				it1--;
				if(t != FOLLOW[cur_s].end())
				{
					cout << " , ";
				}
			}
			cout << " }" << endl;
		}
	}

	/*
		打印表格信息
	*/
	void getTable() 
	{
		set<char> Vt_temp;
		for (set<char>::iterator iter1 = Vt.begin(); iter1 != Vt.end(); iter1++)
		{
			Vt_temp.insert(*iter1);
		}
		Vt_temp.insert('#');
		for (auto it = Vn.begin(); it != Vn.end(); it++)
		{
			char cur_s = *it;
			for (auto it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) 
			{
				/*
					产生式为
					cur_s->cur_string
				*/
				string cur_string = *it1;
				if (isupper(cur_string[0])) 
				{
					char first_s = cur_string[0];
					for (auto it2 = FIRST[first_s].begin(); it2 != FIRST[first_s].end(); it2++)
					{
						string TableLeft = "";
						TableLeft = TableLeft + cur_s + *it2;
						if(Table[TableLeft].size())
						{
							Table[TableLeft] += "|" + cur_string;
						}
						else
						{
							Table[TableLeft] = cur_string;
						}
					}	
				}
				else 
				{
					string TableLeft = "";
					TableLeft = TableLeft + cur_s + cur_string[0];
					if(Table[TableLeft].size())
					{
						Table[TableLeft] += "|" + cur_string;
					}
					else
					{
						Table[TableLeft] = cur_string;
					}
				}	
			}
			if (FIRST[cur_s].count('@') > 0) 
			{
				for (auto it1 = FOLLOW[cur_s].begin(); it1 != FOLLOW[cur_s].end(); it1++) 
				{
					string TableLeft = "";
					TableLeft = TableLeft + cur_s + *it1;
					if(Table[TableLeft].size())
					{
						Table[TableLeft] += "|@";
					}
					else
					{
						Table[TableLeft] = "@";
					}
				}
			}
		}
		/*
			检查出错信息：即表格中没有出现过的
		*/	
		for (auto it = Vn.begin(); it != Vn.end(); it++) 
		{
			for (auto it1 = Vt_temp.begin(); it1 != Vt_temp.end(); it1++) 
			{
				string TableLeft = "";
				TableLeft = TableLeft + *it + *it1;
				if (!Table.count(TableLeft)) 
				{
					Table[TableLeft] = "error";
				}
			}
		}
		/*请编程实现以下功能
		***************************************************************************************				
			显示Table，例如格式打印：cout << *it << "->" << setw(7) << Table[iter];
		*/
		cout << "显示table表：" << endl << endl;
		int n = Vt_temp.size()+1;
		string line = "-";
		for (int i = 0; i < n;i++)
		{
			line += "-----------------";
		}
		cout << line << endl;
		cout << "|                ";
		for (auto it = Vt_temp.begin(); it != Vt_temp.end();it++)
		{
			cout << "|       "<< *it << "        ";
		}
		cout << "|" << endl;
		cout << line << endl;
		for (auto it = Vn.begin(); it != Vn.end(); it++)
		{
			cout << "|       "<< *it << "        ";
			for (auto it1 = Vt_temp.begin(); it1 != Vt_temp.end();it1++)
			{
				string TableLeft = "";
				TableLeft = TableLeft + *it + *it1;
				int express_len = Table[TableLeft].size() + 3;
				int left_remain = (16 - express_len) / 2;
				int right_remain = 16 - express_len - left_remain;
				cout << "|" << string(left_remain, ' ') << *it << "->" + Table[TableLeft] << string(right_remain, ' ');
			}
			cout << "|" << endl;
			cout << line << endl;
		}
	}

	/*
		每一次分析一个输入串
		Sign为符号栈,出栈字符为x
		输入字符串当前字符为a
	*/
	bool AnalyzePredict(string inputstring)
	{
		stack<char> Sign;
		Sign.push('#');
		Sign.push(S);
		string st = inputstring;
		bool flag = true;
		string stk = "";
		stk += '#';
		stk += S;
		cout << endl << "栈" << string(8, ' ') << "剩余输入" << string(2, ' ') << "动作" << endl;
		string op = "";
		while (flag)
		{
			int stack_len = stk.size();
			int string_len = st.size();
			cout << stk << string(10 - stack_len, ' ') << st << string(10 - string_len, ' ') << op << endl;
			op.clear();
			char x = Sign.top();
			Sign.pop();
			string stk1 = "";
			for (int i = 0; i < stk.length() - 1;i++)
			{
				stk1 += stk[i];
			}
			stk = stk1;
			//如果是终结符,直接移出符号栈
			if (Vt.count(x))
			{
				if (x == st[0])
				{
					string st1 = "";
					for (int i = 1; i < st.length(); i++)
					{
						st1 += st[i];
					}
					st = st1;
				}
				else
				{
					return false;
				}
			}
			else
			{
				/*请编程实现以下功能
				***************************************************************************************
				*/
				//第一步：如果不是终结符，如果是末尾符号
				if (x == '#')
				{
					flag = false;
				}
				//第二步：如果是非终结符，需要移进操作
				else if(x != '@')
				{
					string TableLeft = "";
					TableLeft = TableLeft + x + st[0];
					string temp = Table[TableLeft];
					for (int i = temp.length() - 1; i >= 0; i--)
					{
						Sign.push(temp[i]);
						stk += temp[i];
					}
					op += x;
					op += '-';
					op += '>';
					op += temp;
				}
			}		
		}
		return true;
	}

	/*
		消除左递归算法蓝书P124
	*/
	void remove_left_recursion()
	{
		string tempVn = "";
		for (auto it = Vn.begin(); it != Vn.end(); it++) 
		{
			tempVn += *it;
		}
		
		for (int i = 0; i < tempVn.length(); i++) 
		{
			char pi = tempVn[i];
			/*请编程实现消除左递归的替换代入操作
				***************************************************************************************
			*/
			set<string> s;
			for (auto it = P[pi].begin(); it != P[pi].end(); it++)
			{
				string temp = *it;
				bool flag = false;
				for (int j = 0; j < i; j++)
				{
					if(tempVn[j] == temp[0])
					{
						flag = true;
						char head = tempVn[j];
						string remain = temp.substr(1);
						for (auto it0 = P[head].begin(); it0 != P[head].end(); it0++)
						{
							s.insert((*it0) + remain);
						}
						break;
					}
				}
				if (!flag)
				{
					s.insert(temp);
				}
			}
			P[pi] = s;
			remove_left_gene(pi);
		}
	}

	/*
		消除直接左递归
	*/
	void remove_left_gene(char c) 
	{
		char NewVn;
		for (int i = 0; i < 26; i++) 
		{
			NewVn = i + 'A';
			if (!Vn.count(NewVn)) 
			{
				break;
			}
		}
		bool isaddNewVn = 0;
		for (auto it = P[c].begin(); it != P[c].end(); it++) 
		{
			string right = *it;
			
			if (right[0] == c) 
			{
				isaddNewVn = 1;	
				break;
			}
		}
		if (isaddNewVn) 
		{
			set<string> NewPRight; // 加入了新非终结符NewVn的右部
			set<string> NewPNewVn; // 新非终结符的右部
			for (auto it = P[c].begin(); it != P[c].end(); it++) 
			{
				string right = *it;
				if (right[0] != c) 
				{
					right += NewVn; // TA
					NewPRight.insert(right);
				}
				else 
				{
					right = right.substr(1);//+TA
					right += NewVn;
					NewPNewVn.insert(right);
				}
			}
			Vn.insert(NewVn);
			NewPNewVn.insert("@");
			P[NewVn] = NewPNewVn;
			P[c] = NewPRight;
		}
	}

	// 输出产生式
	void ShowByTogether() 
	{
		for (auto it = Vn.begin(); it != Vn.end(); it++) {
			cout << *it << "->";
			char c = *it;
			for (auto it1 = P[c].begin(); it1 != P[c].end(); it1++) 
			{
				if (it1 == P[c].begin())
					cout << *it1;
				else
					cout << "|" << *it1;		
			}
			cout << endl;
		}
	}
};

int main() {
	/*
	文法测试
	E->T|E+T;
	T->F|T*F;
	F->i|(E);

	A->+TA|@;
	B->*FB|@;
	E->TA;
	F->(E)|i;
	T->FB;
	直接将上面两个测试样例放在parse_test1.txt和parse_test2.txt中
	*/
	string filename_gramer = "parse_test1.txt";
	Grammar *grammar = new Grammar(filename_gramer);
	cout << "/-------------------------没有消除左递归-----------------------------/" << endl;
	cout << "规格显示：" << endl;
	cout << endl;
	grammar->ShowByTogether();
	cout << endl;
	grammar->getFirst();
	cout << endl;
	grammar->getFollow();
	cout << endl;
	grammar->getTable();
	
	cout << "/--------------------------------------------------------------------/" << endl<<endl<<endl;


	cout << "/-------------------------已经消除左递归-----------------------------/" << endl;
	/*
	消除左递归之后的判断
	*/
	grammar = new Grammar(filename_gramer);
	grammar->remove_left_recursion();
	cout << "规格显示：" << endl;
	cout << endl;
	grammar->ShowByTogether();
	cout << endl;
	grammar->getFirst();
	cout << endl;
	grammar->getFollow();
	cout << endl;
	grammar->getTable();
	
	
	cout << "/--------------------------------------------------------------------/" << endl << endl << endl;
	cout << "/-------------------------对词法进行分析----------------------------/" << endl;
	/*
		目前的想法是使用第一个实验lex.cpp文件分离出不同的单词，然后对单词进行操作：
		如果单词为+，*，等于他本身，否则等于i;
		以下直接使用lex.cpp运行后的输出文本out1.txt（见Parsing文件夹内）
	*/
	string filename = "string1_out.txt";
	ifstream in(filename);
	if (!in.is_open()) {
		cout << "文法文件打开失败" << endl;
		exit(1);
	}
	char buffer[200];
	string inf = "";
	int num = 0;
	cout << "文法分析结果为：" << endl << endl;
	if (in.is_open()) 
	{
		while (!in.eof()) 
		{
			in.getline(buffer, 100);
			cout << buffer << endl;
			inf += buffer;
			num++;
		}
	}
	string row = "";
	for (int i = 0; i < num-1; i++) 
	{
		int ptr = i * 13;
		string temp = "";
		for (int j = 1; j <= 5; j++)
		{
			if (inf[j+ptr] == ' ')
				continue;
			else
				temp += inf[ptr+j];
		}
		if (temp == "+" || temp == "-" || temp == "*" || temp == "/" || temp == ">" || temp == "<" || temp == "=" || temp == "(" || temp == ")") 
		{
			row += temp;
		}
		else 
		{
			if (temp == ";") 
			{
				row += "#";
				cout << "当前row为:   " << row << endl;

				if (grammar->AnalyzePredict(row)) 
				{
					cout << "                                         正确" << endl;
				}
				else
					cout << "                                         错误" << endl;
				row = "";
			}
			else 
			{
				row += "i";
			}
		}
	}
	cout << "/--------------------------------------------------------------------/" << endl << endl << endl;
	system("pause");
	return 0;
}

