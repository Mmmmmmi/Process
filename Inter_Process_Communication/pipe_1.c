/* 
 * 
 *  这是一个管道的测试demo
 *  匿名管道 
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
int main()
{

    int pipefd[2] = {-1};   
    int piperet = pipe(pipefd);  //创建管道
    if (piperet == -1) {           //创建管道失败
        perror("error for pipe");
        return -1;
    }
    int pid = fork();           //创建子进程
    if (pid == -1) {              //子进程创建失败
        perror("error for fork");
        return -1;
    }else if (pid > 0){         //进入子进程
        sleep(2);
        close(pipefd[0]);
        const char *ptr = "write to pipe";
        write(pipefd[1], ptr, strlen(ptr));       //向管道中写入数据
        close(pipefd[1]);
    }else {                     //父进程
        close(pipefd[1]);
        char ret[1024] = {0};
        read(pipefd[0], ret, 1023);
        close(pipefd[0]);
        printf("%s\n", ret);
        wait(NULL);
    }
    return 0;
}
