/*
 *
 *      利用fork()  进程替换 等 实现minishell
 *      在2.0的基础上实现了管道和内存共享
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

        //接收命令
        char cmd[1024] = {0};
        fflush(stdout);
        printf("MiniShell:");
        
        //  %[^\n]      接受到\n的地方 将\n之前的内容作为字符串赋给相应的变量
        //              因为要读取输入的所有内容,直接使用scanf 会到空格停止
        //  %*c         从当前读取到的位置开始，到后面所有的内容都不予以保存。
        if (scanf("%[^\n]%*c", cmd) == 0) {
            getchar();
        }

        pid_t pid = fork();
        if (pid == -1) {          //fork出错
            printf("子进程开创失败！\n");
        }else if (pid == 0) {   //子进程

            //到这里，接收命令就完成了
            char *cur = NULL;


            char *pipeargv[32] = {0};
            int pipeargc = 0;
            char *pipecmd = NULL;   //用来保存管道命令
            //解析 是否含有管道 如果有管道 那么 就把总命令分离
            cur = cmd;
            while (*cur != '\0') {
                //遇见了‘|’ 
                if (*cur == '|') {         //遇见管道的第一个标识
                    *cur =  '\0';
                    cur++;
                    if (*cur == '|') {      //遇见管道的第二个标识
                        *cur =  '\0';
                        while (*cur != '\0' && isspace(*cur)) {
                            cur++;
                        }
                        pipecmd = cur;  //pipecmd 中保存的是管道的命令
                    }
                }
                cur++;
            }


            //解析 判断是否有重定向命令
            cur = cmd;
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


            //解析正常命令
            char *argv[32] = {0};
            int argc = 0;
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


            //printf("pipecmd == %p\n", pipecmd);
            //如果有管道的话，应该把输出流重定向为管道
            //然后把第二个的输入流重定向为管道
            if (pipecmd != NULL) {
                int pipefd[2] = {-1}; 
                int piperet = pipe(pipefd);
                if (piperet == -1) {
                    perror("error for pipe");
                    return -1;
                }
                //输出重定向到管道中
                dup2(pipefd[1],1);
                //把第一个命令的输出重定向为管道
                execvp(argv[0], argv);
                //把第二个命令的输入重定向为管道，再把输出换回去
                dup2(pipefd[0], 0);
            }else {
                execvp(argv[0], argv);
            }       



            exit(0);

            
        }

        //父进程, wait 即可
        wait(NULL);                    
    }
    return 0;
}

