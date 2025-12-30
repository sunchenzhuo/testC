#include<stdio.h>
#include<unistd.h>
int main(int argc,const char *argv[])
{
    int i;
    for(i=0;i<1025;i++)
    {
        printf("a");
    }
    while(1)
    {
        sleep(i);
    }
}