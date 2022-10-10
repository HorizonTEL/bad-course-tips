// Lexical_Analysis.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "iostream"
using namespace std;
//�ʷ���������
//���ȶ����ֱ���
/*
��һ�ࣺ��ʶ��  letter(letter | digit)* ���
�ڶ��ࣺ���� 	(digit)+ ���
�����ࣺ������(32)
auto 		break		case		char		const		continue
default 	do 			double		else		enum 		extern
float		for			goto		if			int			long
register	return		short		signed		sizeof		static
struct		switch		typedef		union		unsigned	void
volatile	while

�����ࣺ��� ��/*������//���� () { } [ ] " " ?'
�����ࣺ����� <��<=��>��>=��=��+��-��*��/��^��

�����п������Ž��б��룺
<$,0>
<auto,1>
...
<while,32>
<+��33>
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
<<<,58>����
<>>,59>����
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
<����99,��ֵ>
<��ʶ��100 ����ʶ��ָ��>

*/

/****************************************************************************************/
//ȫ�ֱ����������ֱ�
static char reserveWord[32][20] = {
	"auto", "break", "case", "char", "const", "continue",
	"default", "do", "double", "else", "enum", "extern",
	"float", "for", "goto", "if", "int", "long",
	"register", "return", "short", "signed", "sizeof", "static",
	"struct", "switch", "typedef", "union", "unsigned", "void",
	"volatile", "while"
};
//����������,������Ҫ������������
static char operatorOrDelimiter[43][10] = {
	"+", "-", "*", "/", "<", "<=", ">", ">=", "=", "==",
	"!=", ";", "(", ")", "^", ",", "\"", "\'", "#", "&",
	" &&", "|", "||", "%", "~", "<<", ">>", "[", "]", "{",
	"}", "\\", ".", "\?", ":", "!","+=","-=","*=","/=","&=","++","--"
};

static char IDentifierTbl[1000][50] = { "" };//��ʶ����
/****************************************************************************************/

/********�����Ƿ��Ǳ�����*****************/
int searchReserve(char reserveWord[][20], char s[]) {
	//�Լ�ʵ��
	for (int i = 0; i < 32; i++)
	{
		if(strcmp(s,reserveWord[i])==0) {
			return i+1;
		}
	}
	return 0;
}
/********���ұ�����*****************/

/*********************�ж��Ƿ�Ϊ��ĸ********************/
bool IsLetter(char letter)
{//ע��C���������»���ҲΪ��ʶ����һ���ֿ��Է����ײ��������ط�
	if (letter >= 'a'&&letter <= 'z' || letter >= 'A'&&letter <= 'Z'|| letter=='_')
		return true;
	else
		return false;
}
/*********************�ж��Ƿ�Ϊ��ĸ********************/

/*****************�ж��Ƿ�Ϊ����************************/
bool IsDigit(char digit)
{
	if (digit >= '0'&&digit <= '9')
		return true;
	else
		return false;
}
/*****************�ж��Ƿ�Ϊ����************************/

/********************����Ԥ����ȡ�����õ��ַ���ע��**********************/
void filterResource(char r[], int pProject)
{
	char tempString[10000];
	int count = 0;
	for (int i = 0; i <= pProject; i++)
	{//�Լ�ʵ�֣���Դ������ɾ�����з�\n���Ʊ��\t���س���\r��ɾ��/* ?*/���� //������ע�͡�
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
	 strcpy(r, tempString);//��������֮���Դ����
}
/********************����Ԥ����ȡ�����õ��ַ���ע��**********************/

/****************************�����ӳ����㷨����***********************/
int row = 1, col = 1;
void GetToken(int &syn, char resourceProject[], char token[], int &pProject)
{//����DFA��״̬ת��ͼ���
	 int i, count = 0;//count������token[]��ָʾ�����ռ������ַ�
	 char ch;//��Ϊ�ж�ʹ��
	 ch = resourceProject[pProject];
	 if(ch == '\0')
	 {
	 	 exit(0);
	 }
	 while (ch == ' ')
	 {//���˿ո񣬷�ֹ������ʶ���˿ո������
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
	 {//ÿ���ռ�ǰ������
	 	 token[i] = '\0';
	 }
	 if (IsLetter(resourceProject[pProject]))
	 {
	 	 bool flag = false;
	 	 if(resourceProject[pProject-1]=='\"')
	 	 {
	 	 	 flag = true;
	 	 }
	 	 //��ͷΪ��ĸ�����������ַ����ж��Ƿ�Ϊ��ʶ���������ַ����洢��token�У��Լ�ʵ�֡�
	 	 while(IsLetter(resourceProject[pProject]) || IsDigit(resourceProject[pProject])) {
	 	 	 token[count++] = resourceProject[pProject];
	 	 	 pProject++;
	 	 	 col++;
	 	 }
	 	 token[count] = '\0';
	 	 int t = searchReserve(reserveWord, token);
	 	 //����ҵ��ؼ��ֵ��ֱ��룬�Լ�ʵ��
	 	 if(t) {
	 	 	 syn = t;
	 	 }
	 	 if (syn == -1)
	 	 {//�����ǹؼ������Ǳ�ʶ��
	 	 	 syn = 100;//��ʶ���ֱ���
	 	 }
	 	 if(flag)
	 	 {
	 	 	 syn = 102;
	 	 }
	 	 return;
	 }
	 else if (IsDigit(resourceProject[pProject]))
	 {//���ַ�Ϊ����
	 	 bool flag = false, flag0 = false;
	 	 if (resourceProject[pProject] == '0' && resourceProject[pProject + 1] == 'x')
	 	 {
	 	 	 token[count++] = '0';
	 	 	 token[count++] = 'x';
	 	 	 pProject += 2;
	 	 	 col += 2;
	 	 	 while(IsDigit(resourceProject[pProject]) || (resourceProject[pProject]>='a'&&resourceProject[pProject]<='f') || (resourceProject[pProject]>='A'&&resourceProject[pProject]<='F')) 
	 	 	 {
	 	 	 	 token[count++] = resourceProject[pProject];//�ռ�
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 }
	 	 	 if((resourceProject[pProject]>'f'&&resourceProject[pProject]<='z')||(resourceProject[pProject]>'F'&&resourceProject[pProject]<='Z'))
	 	 	 {
	 	 	 	 flag0 = true;
	 	 	 	 FILE *fp;
	 	 	 	 if ((fp = fopen("E:\\˽��\\ʵ�鱨��\\����ԭ��\\ʵ��һ\\error.txt", "a+")) == NULL)
	 	 	 	 {//��Դ����
	 	 	 	 	 cout << "can't open this file";
	 	 	 	 	 exit(0);
	 	 	 	 }
	 	 	 	 printf("error��Դ�ļ� (%d,%d) λ�÷�������16���Ƹ�ʽ���ԣ�there is no exist %c \n", row, col, resourceProject[pProject]);
	 	 	 	 fprintf(fp, "error��Դ�ļ� (%d,%d) λ�÷�������16���Ƹ�ʽ���ԣ�there is no exist %c \n", row, col, resourceProject[pProject]);
	 	 	 	 fclose(fp);
	 	 	 }
	 	 	 while(IsDigit(resourceProject[pProject]) || IsLetter(resourceProject[pProject]))
	 	 	 {
	 	 	 	 token[count++] = resourceProject[pProject];//�ռ�
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 }
	 	 } 
	 	 else 
	 	 {
	 	 	 while (IsDigit(resourceProject[pProject]) || resourceProject[pProject]=='.') 
	 	 	 {//�������
	 	 	 	 token[count++] = resourceProject[pProject];//�ռ�
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 }//�����һ���ַ������´ν�Ҫ��ʼ��ָ��λ��
	 	 	 if((resourceProject[pProject]=='e'||resourceProject[pProject]=='E') && (resourceProject[pProject+1]=='+'||resourceProject[pProject+1]=='-'))
	 	 	 {
	 	 	 	 token[count++] = resourceProject[pProject];//�ռ�
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 	 token[count++] = resourceProject[pProject];//�ռ�
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 	 flag = true;
	 	 	 }
	 	 	 while (IsDigit(resourceProject[pProject]))
	 	 	 {//�������
	 	 	 	 token[count++] = resourceProject[pProject];//�ռ�
	 	 	 	 pProject++;
	 	 	 	 col++;
	 	 	 }
	 	 }
	 	 if(flag0)
	 	 	 return;
	 	 token[count] = '\0';
	 	 syn = 99;//�����ֱ���
	 	 if (flag)
	 	 	 syn = 101;
	 }
	 else if (ch == ';' || ch == '(' || ch == ')' || ch == '^'
	 	 || ch == ',' || ch == '\"' || ch == '\'' || ch == '~' || ch == '#' || ch == '%' || ch == '['
	 	 || ch == ']' || ch == '{' || ch == '}' || ch == '\\' || ch == '.' || ch == '\?' || ch == ':')
	 {//��Ϊ��������߽�������õ����
	 	 token[0] = resourceProject[pProject];
	 	 token[1] = '\0';//�γɵ��ַ���
	 	 for (i = 0; i<36; i++)
	 	 {//������������
	 	 	 if (strcmp(token, operatorOrDelimiter[i]) == 0)
	 	 	 {
	 	 	 	 syn = 33 + i;//����ֱ��룬ʹ����һ�㼼�ɣ�ʹ֮������ӳ��
	 	 	 	 break;//�鵽���Ƴ�
	 	 	 }
	 	 }
	 	 pProject++;//ָ�����ƣ�Ϊ��һɨ����׼��
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '+')
	 {//+,+=,++
	 	 pProject++;//���ƣ���ǰ����
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
	 	 pProject++;//ָ������
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '-')
	 {//-,-=,--
	 	 pProject++;//���ƣ���ǰ����
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
	 	 pProject++;//ָ������
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '*')
	 {//*,*=
	 	 pProject++;//���ƣ���ǰ����
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
	 	 pProject++;//ָ������
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '/')
	 {///,/=,/*
	 	 pProject++;//���ƣ���ǰ����
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 72;
	 	 }
	 	 else if(resourceProject[pProject] == '*')
	 	 {
	 	 	 FILE *fp;
	 	 	 if ((fp = fopen("E:\\˽��\\ʵ�鱨��\\����ԭ��\\ʵ��һ\\error.txt", "a+")) == NULL)
	 	 	 {//��Դ����
	 	 	 	 cout << "can't open this file";
	 	 	 	 exit(0);
	 	 	 }
	 	 	 printf("error��Դ�ļ� (%d,%d) λ�÷�������ע��δ��ԣ�there is no exist %c%c \n", row, col, resourceProject[pProject-1], resourceProject[pProject]);
	 	 	 fprintf(fp, "error��Դ�ļ� (%d,%d) λ�÷�������ע��δ��ԣ�there is no exist %c%c \n", row, col, resourceProject[pProject-1], resourceProject[pProject]);
	 	 	 fclose(fp);
	 	 	 syn = 0;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 36;
	 	 }
	 	 pProject++;//ָ������
	 	 col++;
	 	 return;
	 }
	 else if (resourceProject[pProject] == '<')
	 {//<,<=,<<
	 	 pProject++;//���ƣ���ǰ����
	 	 col++;
	 	 if (resourceProject[pProject] == '=')
	 	 {
	 	 	 syn = 38;
	 	 }
	 	 else if (resourceProject[pProject] == '<')
	 	 {//����
	 	 	 syn = 58;
	 	 }
	 	 else
	 	 {
	 	 	 pProject--;
	 	 	 col--;
	 	 	 syn = 37;
	 	 }
	 	 pProject++;//ָ������
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
	 {//������
	 	 syn = 0;//�ֱ���Ϊ0
	 }
	 else
	 {//���ܱ����ϴʷ�����ʶ�������
	 	 FILE *fp;
	 	 if ((fp = fopen("E:\\˽��\\ʵ�鱨��\\����ԭ��\\ʵ��һ\\error.txt", "a+")) == NULL)
	 	 {//��Դ����
	 	 	 cout << "can't open this file";
	 	 	 exit(0);
	 	 }
	 	 printf("error��Դ�ļ� (%d,%d) λ�÷��������зǷ��ַ���there is no exist %c \n", row, col, resourceProject[pProject]);
	 	 fprintf(fp, "error��Դ�ļ� (%d,%d) λ�÷��������зǷ��ַ���there is no exist %c \n", row, col, resourceProject[pProject]);
	 	 fclose(fp);
	 	 pProject++;
	 	 col++;
	 }
}

int main()
{
	 //��һ���ļ�����ȡ���е�Դ����
	 char resourceProject[10000];
	 char token[20] = { 0 };
	 int syn = -1, i;//��ʼ��
	 int pProject = 0;//Դ����ָ��
	 FILE *fp, *fp1;
	 if ((fp = fopen("E:\\˽��\\ʵ�鱨��\\����ԭ��\\ʵ��һ\\testHard.c", "r")) == NULL)
	 {//��Դ����
	 	 cout << "can't open this file";
	 	 exit(0);
	 }
	 resourceProject[pProject] = fgetc(fp);//��ȡһ���ַ�
	 while (resourceProject[pProject] != EOF)
	 {//��Դ�������resourceProject[]����
	 	 pProject++;
	 	 resourceProject[pProject] = fgetc(fp);
	 }
	 resourceProject[++pProject] = '\0';
	 fclose(fp);
	 cout << endl << "Դ����Ϊ:" << endl;
	 for (int i = 0, k = 1; i <= pProject;i++)
	 {
	 	 if(i==0 || resourceProject[i-1]=='\n')
	 	 {
	 	 	 cout << k++ << " ";
	 	 }
	 	 cout << resourceProject[i];
	 }
	 cout << endl;
	 //��Դ������й���
	 filterResource(resourceProject, pProject);
	 cout << endl << "����֮��ĳ���:" << endl;
	 cout << resourceProject << endl;
	 pProject = 0;//��ͷ��ʼ��

	 if ((fp1 = fopen("E:\\˽��\\ʵ�鱨��\\����ԭ��\\ʵ��һ\\test_compile.txt", "w+")) == NULL)
	 {//��Դ����
	 	 cout << "can't open this file";
	 	 exit(0);
	 }
	 cout << endl;
	 int num = 0;
	 while (syn != 0)
	 {
	 	 //����ɨ��
	 	 syn = -1;
	 	 GetToken(syn, resourceProject, token, pProject);
	 	 if (syn == 100)
	 	 {//�ж��Ƿ������б�ʶ�����У��Լ�ʵ��
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
	 	 	 printf("(��ʶ�� ,%s)\n", token);
	 	 	 fprintf(fp1, "(��ʶ�� , %s)\n", token);
	 	 }
	 	 else if (syn >= 1 && syn <= 32)
	 	 {//������
	 	 	 printf("(%s , ������)\n", reserveWord[syn - 1]);
	 	 	 fprintf(fp1, "(%s , ������)\n", reserveWord[syn - 1]);
	 	 }
	 	 else if (syn == 99)
	 	 {//const ����
	 	 	 printf("(���� , %s)\n", token);
	 	 	 fprintf(fp1, "(���� , %s)\n", token);
	 	 }
	 	 else if (syn >= 33 && syn <= 74)
	 	 {
	 	 	 printf("(%s , ����)\n", operatorOrDelimiter[syn - 33]);
	 	 	 fprintf(fp1, "(%s , ����)\n", operatorOrDelimiter[syn - 33]);
	 	 } 
	 	 else if (syn == 101)
	 	 {
	 	 	 printf("(������ , %s)\n", token);
	 	 	 fprintf(fp1, "(������ , %s)\n", token);
	 	 }
	 	 else if (syn == 102)
	 	 {
	 	 	 printf("(�ַ������� , %s)\n", token);
	 	 	 fprintf(fp1, "(�ַ������� , %s)\n", token);
	 	 }
	 }
	 cout << endl;
	 for (i = 0; i<100; i++)
	 {//�����ʶ������
	 	 if(IDentifierTbl[i][0]=='\0')
	 	 {
	 	 	 break;
	 	 }
	 	 printf("��%d����ʶ���� %s\n", i + 1, IDentifierTbl[i]);
	 	 fprintf(fp1, "��%d����ʶ���� %s\n", i + 1, IDentifierTbl[i]);
	 }
	 fclose(fp1);
	 return 0;
}
