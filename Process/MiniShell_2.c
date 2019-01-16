/*
 *
 *      利用fork()  进程替换等 实现minishell
 *      在1.0的基础上实现了输出重定向
 *
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
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
        pid_t pid = fork();
        if (pid == -1) {          //fork出错
            printf("子进程开创失败！\n");
        }else if (pid == 0) {   //子进程
            //到这里，接收命令就完成了
            char *cur = cmd;
            char *argv[32] = {0};
            int argc = 0;

            //解析 判断是否有重定向命令
            int flag = 0;         //标记是否有重定向
            char *filename = NULL;
            while(*cur != '\0') {
                //遇见了第一个 '>'
                if (*cur == '>') {
                    flag = 1;
                    *cur = '\0';
                    cur++;
                    //遇见了第二个 ‘>’
                    if (*cur == '>') {
                        flag = 2;
                        cur++;
                    }
                    while (*cur != '\0' && isspace(*cur)) {        //cur 没有走到结束，并且当前字符为空白字符
                        cur++;
                    }
                    filename = cur;            //文件名保存下来
                }
                cur++;
            }
            umask(0);
            if (flag == 1) {
                int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0664);
                dup2(fd, 1);    //这里必须在子程序中进行，不然主程序的输出就有问题了            
            }else if (flag == 2) {
                int fd = open(filename, O_WRONLY|O_CREAT|O_APPEND, 0664);
                dup2(fd, 1);            
            } 
            flag = 0;
            //解析命令
            cur = cmd;
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
            execvp(argv[0], argv);
            exit(0);
        }
        //父进程, wait 即可
        wait(NULL);                    
    }
    return 0;
}

