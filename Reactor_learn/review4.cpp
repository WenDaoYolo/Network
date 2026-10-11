#include<unistd.h>
#include<iostream>
#include<string.h>

void test1()
{
    char str_buffer[64]={0};
    int nums=read(0,str_buffer,sizeof(str_buffer));
    std::cout<<nums<<std::endl;

    const char* str="hello world";
    nums=write(1,str,strlen(str)+1);
    std::cout<<std::endl<<nums<<std::endl;
}

int main()
{
    test1();

    return 0;
}