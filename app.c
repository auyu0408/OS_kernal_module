#include<stdlib.h>
#include<stdio.h>
#include <string.h>

int main()
{
    char* option = NULL;//save input string from stdin
    const char* filename = "/proc/my_info";//file name
    char* contents = NULL;//save the input from the file
    size_t len = 0;//save the length of string

    while(1)
    {
        int flag = 0;//use to check your position
        FILE *fp = fopen(filename, "r");//save the opened filed
        if(!fp)
        {
            puts("Proc file doesn't exist\n");
            exit(0);
        }
        printf("Which information do you want?\n");
        printf("Version(v),CPU(c),Memory(m),Time(t),All(a),Exit(e)?\n");
        if(getline(&option, &len, stdin) == 1);
        printf("\n");
        if(*option =='v')
        {
            while (getline(&contents, &len, fp) != -1)
            {
                if(*contents == '=')
                    flag++;
                else if(flag < 1)
                    continue;
                else
                {
                    printf("Version: %s", contents);
                    printf("------------------------------------------------------------\n");
                    break;
                }
            }
        }
        else if(*option=='c')
        {
            while (getline(&contents, &len, fp) != -1)
            {
                if(*contents == '=')
                {
                    flag++;
                    if(flag == 2)
                    {
                        printf("Cpu information:\n");
                        continue;
                    }
                }
                if(flag < 2)
                    continue;
                else if(flag == 2)
                    printf("%s", contents);
                else
                {
                    printf("------------------------------------------------------------\n");
                    break;
                }
            }
        }
        else if(*option=='m')
        {
            while (getline(&contents, &len, fp) != -1)
            {
                if(*contents == '=')
                {
                    flag++;
                    if(flag == 3)
                    {
                        printf("Memory information:\n");
                        continue;
                    }
                }
                if(flag < 3)
                    continue;
                else if(flag == 3)
                    printf("%s", contents);
                else
                {
                    printf("------------------------------------------------------------\n");
                    break;
                }
            }
        }
        else if(*option=='t')
        {
            while (getline(&contents, &len, fp) != -1)
            {
                if(*contents == '=')
                {
                    flag++;
                    if(flag == 4)
                    {
                        printf("Time information:\n");
                        continue;
                    }
                }
                if(flag < 4)
                    continue;
                else
                    printf("%s", contents);
            }
            printf("------------------------------------------------------------\n");
        }
        else if(*option=='a')
        {
            while (getline(&contents, &len, fp) != -1)
                printf("%s", contents);
            printf("------------------------------------------------------------\n");
        }
        else if(*option=='e')
            break;
        else
        {
            printf("Wrong input word.\n");
            printf("------------------------------------------------------------\n");
        }
        fclose(fp);
    }
    return 0;
}
