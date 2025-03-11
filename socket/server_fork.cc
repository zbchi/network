#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>
using namespace std;

void recycle(int signo)
{
    while (1)
    {
        pid_t pid = waitpid(-1, NULL, WNOHANG);
        if (pid <= 0)
            break;
    }
}
void set_SIGCHLD()
{
    struct sigaction act;
    act.sa_handler = recycle;
    act.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);
}
void working(int cfd)
{
    char buf[1024];
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        int len = read(cfd, buf, sizeof(buf));
        if (len == 0)
        {
            cout << "客户端断开连接" << endl;
            break;
        }
        else if (len == -1)
        {
            perror("read");
            break;
        }
        else
        {
            cout << "客户端say:" << buf << endl;
            write(cfd, buf, len);
        }
    }
    close(cfd);
}

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
        perror("socket");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8989);
    addr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
        perror("bind");

    ret = listen(lfd, 128);
    if (ret == -1)
        perror("listen");

    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    set_SIGCHLD();

    while (1)
    {
        int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &clilen);
        if (cfd == -1)
        {
            if (errno == EINTR)
                continue;
            perror("accept");
            continue;
        }
        char myip[24];
        cout << "客户端IP:" << inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, myip, sizeof(myip)) << "端口:" << ntohs(cliaddr.sin_port) << endl;
        pid_t pid = fork();
        if (pid == 0)
        {
            close(lfd);

            working(cfd);
            exit(0);
        }
    }
}