/*
 *
 *      利用fork()  进程替换等 实现minishell
 *
 *
*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int main()
{
    while(1) {
        char cmd[1024] = {0};
        fflush(stdout);
        printf("MiniShell:");
         //  %[^\n]     接受到\n的地方 将\n之前的内容作为字符串赋给相应的变量
         //     因为要读取输入的所有内容,直接使用scanf 会到空格停止
         //  %*c        从当前读取到的位置开始，到后面所有的内容都不予以保存。
        if (scanf("%[^\n]%*c", cmd) == 0) {
            getchar();
        }
        char *cur = cmd;
        char *argv[32] = {0};
        int argc = 0;
        while(*cur != '\0') {
            if (!isspace(*cur)){
                argv[argc] = cur;
                argc++;
                while(*cur != '\0' && !isspace(*cur)) {
                    cur++;
                }
                *cur = '\0';
            }
            cur++;
        }
        //刚才输入的输出
        //  for (int i = 0; i < argc; i++) {
        //       printf("%s  ", argv[i]);
        //   }

        pid_t pid = fork();
        if (pid == -1) {          //fork出错
            printf("子进程开创失败！\n");
        }else if (pid == 0) {   //子进程
            execvp(argv[0], argv);
            exit(0);

        }else if (pid > 0) {    //父进程
            wait(NULL);                    
        }
    }
    return 0;
}

