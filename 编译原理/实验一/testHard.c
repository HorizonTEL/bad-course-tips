#include <studio.h>
int main(int argc, char const *argv[])
{
	char *str = "String520#";
	int num1=0x45f7,num2=0187,num3=0x1g35;//num3 16进制格式不对
	//This is comment
	/**
	printf("NULL\n");
	*/
	int floatnum = 123.456;# //出现错误字符
	if(6.4 <= 5.8)
		printf("Yes");	
	if(floatn>5) //floatn未定义而使用，属于语法分析错误，先跳过。
		printf("No 0x1314L");
	for(int i=6;i>=0;i--){
		num2-=10;	//正确识别-=这个运算符
		printf("%d",num2);
	}
	/*注释未配对
	printf("NULL\n");
	
	return 0;
}