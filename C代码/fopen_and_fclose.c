#include<stdio.h>
#include<string.h>
#include<errno.h>

int main(int argc, const char *argv[])
{
    FILE *p=fopen("1.txt","w+");
    int q;
    if (p==NULL)
    {
        printf("fopen:%s\n",strerror(errno));
    }
    else
    {
        perror("fopen:");
        q=fclose(p);
        if (q==0)
        
            printf("file close succes !\n");
        else
        {
            printf("fopen:%s\n",strerror(errno));
        }
        
        
    }
    return 0;
    
}