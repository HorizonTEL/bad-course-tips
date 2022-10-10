// Lexical_Analysis.cpp : 定义控制台应用程序的入口点。
//
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "iostream"
using namespace std;
//词法分析程序
//首先定义种别码
/*
第一类：标识符  letter(letter | digit)* 无穷集
第二类：常数 	(digit)+ 无穷集
第三类：保留字(32)
auto 		break		case		char		const		continue
default 	do 			double		else		enum 		extern
float		for			goto		if			int			long
register	return		short		signed		sizeof		static
struct		switch		typedef		union		unsigned	void
volatile	while

第四类：界符 ‘/*’、‘//’、 () { } [ ] " " ?'
第五类：运算符 <、<=、>、>=、=、+、-、*、/、^、

对所有可数符号进行编码：
<$,0>
<auto,1>
...
<while,32>
<+，33>
<-,34>
<*,35>
</,36>
<<,37>
<<=,38>
<>,39>
<>=,40>
<=,41>
<==,42>
<!=,43>
<;,44>
<(,45>
<),46>
<^,47>
<,,48>
<",49>
<',50>
<#,51>
<&,52>
<&&,53>
<|,54>
<||,55>
<%,56>
<~,57>
<<<,58>左移
<>>,59>右移
<[,60>
<],61>
<{,62>
<},63>
<\,64>
<.,65>
<?,66>
<:,67>
<!,68>
"[","]","{","}"
<常数99,数值>
<标识符100 ，标识符指针>

*/

/****************************************************************************************/
//全局变量，保留字表
static char reserveWord[32][20] = {
	"auto", "break", "case", "char", "const", "continue",
	"default", "do", "double", "else", "enum", "extern",
	"float", "for", "goto", "if", "int", "long",
	"register", "return", "short", "signed", "sizeof", "static",
	"struct", "switch", "typedef", "union", "unsigned", "void",
	"volatile", "while"
};
//界符运算符表,根据需要可以自行增加
static char operatorOrDelimiter[43][10] = {
	"+", "-", "*", "/", "<", "<=", ">", ">=", "=", "==",
	"!=", ";", "(", ")", "^", ",", "\"", "\'", "#", "&",
	" &&", "|", "||", "%", "~", "<<", ">>", "[", "]", "{",
	"}", "\\", ".", "\?", ":", "!","+=","-=","*=","/=","&=","++","--"
};

static char IDentifierTbl[1000][50] = { "" };//标识符表
/****************************************************************************************/

/********查找是否是保留字*****************/
int searchReserve(char reserveWord[][20], char s[]) {
	//自己实现
	for (int i = 0; i < 32; i++)
	{
		if(strcmp(s,reserveWord[i])==0) {
			return i+1;
		}
	}
	return 0;
}
/********查找保留字*****************/

/*********************判断是否为字母********************/
bool IsLetter(char letter)
{//注意C语言允许下划线也为标识符的一部分可以放在首部或其他地方
	if (letter >= 'a'&&letter <= 'z' || letter >= 'A'&&letter <= 'Z'|| letter=='_')
		return true;
	else
		return false;
}
/*********************判断是否为字母********************/

/*****************判断是否为数字************************/
bool IsDigit(char digit)
{
	if (digit >= '0'&&digit <= '9')
		return true;
	else
		return false;
}
/*****************判断是否为数字************************/

/********************编译预处理，取出无用的字符和注释**********************/
void filterResource(char r[], int pProject)
{
	char tempString[10000];
	int count = 0;
	for (int i = 0; i <= pProject; i++)
	{//自己实现，从源程序中删除换行符\n、制表符\t、回车符\r，删除/* ?*/或者 //包含的注释。
		if(r[i]=='\t' || r[i]=='\r') {
			continue;
		}
		if(r[i]=='/' && r[i+1]=='/') {
			i += 2;
			while(r[i] != '\n') {
				i++;
			}
			continue;
		}
		int temp = i;
		bool flag = true;
		if(r[i]=='/' && r[i+1]=='*') {
			i += 2;
	 	 	while(r[i] != '*' || r[i+1] != '/') {
	 	 	 	i++;
	 	 	}
	 	 	if(i > pProject)
	 	 	{
	 	 	 	flag = false;
	 	 	}
	 	 	if(flag)
	 	 	{
	 	 	 	 i++;
	 	 	 	 continue;
	 	 	}
	 	 	else
	 	 	{
	 	 	 	i = temp;
	 	 	} 	 	 
	 	}
	 	tempString[count++] = r[i];
	 }
	 tempString[count] = '\0';
	 strcpy(r, tempString);//产生净化之后的源程序
}
/********************编译预处理，取出无用的字符和注释**********************/

/****************************分析子程序，算法核心***********************/
int row = 1, col = 1;
void GetToken(int &syn, char resourceProject[], char token[], int &pProject)
{//根据DFA的状态转换图设计
	 int i, count = 0;//count用来做token[]的指示器，收集有用字符
	 char ch;//作为判断使用
	 ch = resourceProject[pProject];
	 if(ch == '\0')
	 {
	 	 exit(0);
	 }
	 while (ch == ' ')
	 {//过滤空格，防止程序因识别不了空格而结束
	 	 pProject++;
	 	 col++;
	 	 ch = resourceProject[pProject];
	 }
	 if(ch == '\n')
	 {
	 	 pProject++;
	 	 row++;
	 	 col = 1;
	 	 ch = resourceProject[pProject];
	 }
	 for (i = 0; i<20; i++)
	 {//每次收集前先清零
	 	 token[i] = '\0';
	 }
	 if (IsLetter(resourceProject[pProject]))
	 {
	 	 bool flag = false;
	 	 if(resourceProject[pProject-1]=='\"')
	 	 {
	 	 	 flag = true;
	 	 }
	 	 //开头为字母，读入后面的字符，判断是否为标识符，并将字符串存储在token中，自己实现。
	 	 while(IsLetter(resourceProject[pProject]) || IsDigit(resourceProject[pProject])) {
	 	 	 token[count++] = resourceProject[pProject];
	 	 	 pProject++;
	 	 	 col++;
	 	 }
	 	 token[count] = '\0';
	 	 int t = searchReserve(reserveWord, token);
	 	 //查表找到关键字的种别码，自己实现
	 	 if(t) {
	 	 	 syn = t;
	 	 }
	 	 if (syn == -1)
	 	 {//若不是关键字则是标识符
	 	 	 syn = 100;//标识符种别码
	 	 }
	 	 if(flag)
	 	 {
	 	 	 syn = 102;
	 	 }
	 	 return;
	 }
	 else if (IsDigit(resourceProject[pProject]))
	 {//首字符为数字
	 	 bool flag = false, flag0 = false;
	 	 if (resourceProject[pProject] == '0' && resourceProject[pProject + 1] == 'x')
	 	 {
	 	 	 token[count++] = '0';
	 	 	 token[count++] = 'x';
	 	 	 pProject += 2;
	 	 	 col += 2;
	 	 	 while(IsDigit(resourceProject[pProject]) || (resourceProject[pProject]>='a'&&resourceProject[pProject]<='f') || (resourceProject[pProject]>='A'&&resourceProject[pProject]<='F')) 
	 	 	 {
	 	 	 	 token[count++] = resourceProject[pProject];//收集
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 }
	 	 	 if((resourceProject[pProject]>'f'&&resourceProject[pProject]<='z')||(resourceProject[pProject]>'F'&&resourceProject[pProject]<='Z'))
	 	 	 {
	 	 	 	 flag0 = true;
	 	 	 	 FILE *fp;
	 	 	 	 if ((fp = fopen("E:\\私人\\实验报告\\编译原理\\实验一\\error.txt", "a+")) == NULL)
	 	 	 	 {//打开源程序
	 	 	 	 	 cout << "can't open this file";
	 	 	 	 	 exit(0);
	 	 	 	 }
	 	 	 	 printf("error：源文件 (%d,%d) 位置发生错误，16进制格式不对，there is no exist %c \n", row, col, resourceProject[pProject]);
	 	 	 	 fprintf(fp, "error：源文件 (%d,%d) 位置发生错误，16进制格式不对，there is no exist %c \n", row, col, resourceProject[pProject]);
	 	 	 	 fclose(fp);
	 	 	 }
	 	 	 while(IsDigit(resourceProject[pProject]) || IsLetter(resourceProject[pProject]))
	 	 	 {
	 	 	 	 token[count++] = resourceProject[pProject];//收集
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 }
	 	 } 
	 	 else 
	 	 {
	 	 	 while (IsDigit(resourceProject[pProject]) || resourceProject[pProject]=='.') 
	 	 	 {//后跟数字
	 	 	 	 token[count++] = resourceProject[pProject];//收集
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 }//多读了一个字符既是下次将要开始的指针位置
	 	 	 if((resourceProject[pProject]=='e'||resourceProject[pProject]=='E') && (resourceProject[pProject+1]=='+'||resourceProject[pProject+1]=='-'))
	 	 	 {
	 	 	 	 token[count++] = resourceProject[pProject];//收集
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 	 token[count++] = resourceProject[pProject];//收集
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 	 flag = true;
	 	 	 }
	 	 	 while (IsDigit(resourceProject[pProject]))
	 	 	 {//后跟数字
	 	 	 	 token[count++] = resourceProject[pProject];//收集
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 }
	 	 }
	 	 if(flag0)
	 	 	 return;
	 	 token[count] = '\0';
	 	 syn = 99;//常数种别码
	 	 if (flag)
	 	 	 syn = 101;
	 }
	 else if (ch == ';' || ch == '(' || ch == ')' || ch == '^'
	 	 || ch == ',' || ch == '\"' || ch == '\'' || ch == '~' || ch == '#' || ch == '%' || ch == '['
	 	 || ch == ']' || ch == '{' || ch == '}' || ch == '\\' || ch == '.' || ch == '\?' || ch == ':')
	 {//若为运算符或者界符，查表得到结果
	 	 token[0] = resourceProject[pProject];
	 	 token[1] = '\0';//形成单字符串
	 	 for (i = 0; i<36; i++)
	 	 {//查运算符界符表
	 	 	 if (strcmp(token, operatorOrDelimiter[i]) == 0)
	 	 	 {
	 	 	 	 syn = 33 + i;//获得种别码，使用了一点技巧，使之呈线性映射
	 	 	 	 break;//查到即推出
	 	 	 }
	 	 }
	 	 pProject++;//指针下移，为下一扫描做准备
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '+')
	 {//+,+=,++
	 	 pProject++;//后移，超前搜索
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 69;
	 	 }
	 	 else if (resourceProject[pProject] == '+')
	 	 {
	 	 	 syn = 74;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 33;
	 	 }
	 	 pProject++;//指针下移
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '-')
	 {//-,-=,--
	 	 pProject++;//后移，超前搜索
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 70;
	 	 }
	 	 else if (resourceProject[pProject] == '-')
	 	 {
	 	 	 syn = 75;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 34;
	 	 }
	 	 pProject++;//指针下移
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '*')
	 {//*,*=
	 	 pProject++;//后移，超前搜索
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 71;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 35;
	 	 }
	 	 pProject++;//指针下移
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '/')
	 {///,/=,/*
	 	 pProject++;//后移，超前搜索
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 72;
	 	 }
	 	 else if(resourceProject[pProject] == '*')
	 	 {
	 	 	 FILE *fp;
	 	 	 if ((fp = fopen("E:\\私人\\实验报告\\编译原理\\实验一\\error.txt", "a+")) == NULL)
	 	 	 {//打开源程序
	 	 	 	 cout << "can't open this file";
	 	 	 	 exit(0);
	 	 	 }
	 	 	 printf("error：源文件 (%d,%d) 位置发生错误，注释未配对，there is no exist %c%c \n", row, col, resourceProject[pProject-1], resourceProject[pProject]);
	 	 	 fprintf(fp, "error：源文件 (%d,%d) 位置发生错误，注释未配对，there is no exist %c%c \n", row, col, resourceProject[pProject-1], resourceProject[pProject]);
	 	 	 fclose(fp);
	 	 	 syn = 0;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 36;
	 	 }
	 	 pProject++;//指针下移
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '<')
	 {//<,<=,<<
	 	 pProject++;//后移，超前搜索
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 38;
	 	 }
	 	 else if (resourceProject[pProject] == '<')
	 	 {//左移
	 	 	 syn = 58;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 37;
	 	 }
	 	 pProject++;//指针下移
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '>')
	 {//>,>=,>>
	 	 pProject++;
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 40;
	 	 }
	 	 else if (resourceProject[pProject] == '>')
	 	 {
	 	 	 syn = 59;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 39;
	 	 }
	 	 pProject++;
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '=')
	 {//=.==
	 	 pProject++;
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 42;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 41;
	 	 }
	 	 pProject++;
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '!')
	 {//!,!=
	 	 pProject++;
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 43;
	 	 }
	 	 else
	 	 {
	 	 	 syn = 68;
	 	 	 pProject--;
	 	 	 col--;
	 	 }
	 	 pProject++;
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '&')
	 {//&,&&
	 	 pProject++;
	 	 col++;
	 	 if (resourceProject[pProject] == '&')
	 	 {
	 	 	 syn = 53;
	 	 }
	 	 else if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 73;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 52;
	 	 }
	 	 pProject++;
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '|')
	 {//|,||
	 	 pProject++;
	 	 col++;
	 	 if (resourceProject[pProject] == '|')
	 	 {
	 	 	 syn = 55;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 54;
	 	 }
	 	 pProject++;
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '$')
	 {//结束符
	 	 syn = 0;//种别码为0
	 }
	 else
	 {//不能被以上词法分析识别，则出错。
	 	 FILE *fp;
	 	 if ((fp = fopen("E:\\私人\\实验报告\\编译原理\\实验一\\error.txt", "a+")) == NULL)
	 	 {//打开源程序
	 	 	 cout << "can't open this file";
	 	 	 exit(0);
	 	 }
	 	 printf("error：源文件 (%d,%d) 位置发生错误，有非法字符，there is no exist %c \n", row, col, resourceProject[pProject]);
	 	 fprintf(fp, "error：源文件 (%d,%d) 位置发生错误，有非法字符，there is no exist %c \n", row, col, resourceProject[pProject]);
	 	 fclose(fp);
	 	 pProject++;
	 	 col++;
	 }
}

int main()
{
	 //打开一个文件，读取其中的源程序
	 char resourceProject[10000];
	 char token[20] = { 0 };
	 int syn = -1, i;//初始化
	 int pProject = 0;//源程序指针
	 FILE *fp, *fp1;
	 if ((fp = fopen("E:\\私人\\实验报告\\编译原理\\实验一\\testHard.c", "r")) == NULL)
	 {//打开源程序
	 	 cout << "can't open this file";
	 	 exit(0);
	 }
	 resourceProject[pProject] = fgetc(fp);//读取一个字符
	 while (resourceProject[pProject] != EOF)
	 {//将源程序读入resourceProject[]数组
	 	 pProject++;
	 	 resourceProject[pProject] = fgetc(fp);
	 }
	 resourceProject[++pProject] = '\0';
	 fclose(fp);
	 cout << endl << "源程序为:" << endl;
	 for (int i = 0, k = 1; i <= pProject;i++)
	 {
	 	 if(i==0 || resourceProject[i-1]=='\n')
	 	 {
	 	 	 cout << k++ << " ";
	 	 }
	 	 cout << resourceProject[i];
	 }
	 cout << endl;
	 //对源程序进行过滤
	 filterResource(resourceProject, pProject);
	 cout << endl << "过滤之后的程序:" << endl;
	 cout << resourceProject << endl;
	 pProject = 0;//从头开始读

	 if ((fp1 = fopen("E:\\私人\\实验报告\\编译原理\\实验一\\test_compile.txt", "w+")) == NULL)
	 {//打开源程序
	 	 cout << "can't open this file";
	 	 exit(0);
	 }
	 cout << endl;
	 int num = 0;
	 while (syn != 0)
	 {
	 	 //启动扫描
	 	 syn = -1;
	 	 GetToken(syn, resourceProject, token, pProject);
	 	 if (syn == 100)
	 	 {//判断是否在已有标识符表中，自己实现
	 	 	 bool flag = false;
	 	 	 for (int i = 0; i < num;i++)
	 	 	 {
	 	 	 	 if(strcmp(token,IDentifierTbl[i]) == 0)
	 	 	 	 {
	 	 	 	 	 flag = true;
	 	 	 	 	 break;
	 	 	 	 }
	 	 	 }
	 	 	 if(!flag)
	 	 	 {
	 	 	 	 for (int i = 0; i < strlen(token);i++)
	 	 	 	 {
	 	 	 	 	 IDentifierTbl[num][i] = token[i];
	 	 	 	 }
	 	 	 	 num++;
	 	 	 }
	 	 	 printf("(标识符 ,%s)\n", token);
	 	 	 fprintf(fp1, "(标识符 , %s)\n", token);
	 	 }
	 	 else if (syn >= 1 && syn <= 32)
	 	 {//保留字
	 	 	 printf("(%s , 保留字)\n", reserveWord[syn - 1]);
	 	 	 fprintf(fp1, "(%s , 保留字)\n", reserveWord[syn - 1]);
	 	 }
	 	 else if (syn == 99)
	 	 {//const 常数
	 	 	 printf("(常数 , %s)\n", token);
	 	 	 fprintf(fp1, "(常数 , %s)\n", token);
	 	 }
	 	 else if (syn >= 33 && syn <= 74)
	 	 {
	 	 	 printf("(%s , 符号)\n", operatorOrDelimiter[syn - 33]);
	 	 	 fprintf(fp1, "(%s , 符号)\n", operatorOrDelimiter[syn - 33]);
	 	 } 
	 	 else if (syn == 101)
	 	 {
	 	 	 printf("(浮点数 , %s)\n", token);
	 	 	 fprintf(fp1, "(浮点数 , %s)\n", token);
	 	 }
	 	 else if (syn == 102)
	 	 {
	 	 	 printf("(字符串常量 , %s)\n", token);
	 	 	 fprintf(fp1, "(字符串常量 , %s)\n", token);
	 	 }
	 }
	 cout << endl;
	 for (i = 0; i<100; i++)
	 {//插入标识符表中
	 	 if(IDentifierTbl[i][0]=='\0')
	 	 {
	 	 	 break;
	 	 }
	 	 printf("第%d个标识符： %s\n", i + 1, IDentifierTbl[i]);
	 	 fprintf(fp1, "第%d个标识符： %s\n", i + 1, IDentifierTbl[i]);
	 }
	 fclose(fp1);
	 return 0;
}
