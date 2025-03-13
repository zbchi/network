#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
using namespace std;

int fds[1024];
struct sockinfo
{
    int fd;
    struct sockaddr_in addr;
};
struct sockinfo fdss[1024];
void *working(void *arg)
{
    struct sockinfo *info = (struct sockinfo *)arg;
    char buf[1024];

    char myip[24];
    cout << "客户端IP:" << inet_ntop(AF_INET, &info->addr.sin_addr.s_addr, myip, sizeof(myip)) << "端口:" << ntohs(info->addr.sin_port) << endl;

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        int len = read(info->fd, buf, sizeof(buf));
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
            write(info->fd, buf, len);
        }
    }
    close(info->fd);
    info->fd = -1;

    return NULL;
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

    // 设置端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
        perror("bind");

    ret = listen(lfd, 128);
    if (ret == -1)
        perror("listen");

    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    // memset(fds, -1, sizeof(fds));
    for (int i = 0; i < sizeof(fdss) / sizeof(struct sockinfo); i++)
    {
        fdss[i].fd = -1;
    }

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

        struct sockinfo *ptr = NULL;
        for (int i = 0; i < sizeof(fdss) / sizeof(struct sockinfo); i++)
        {
            if (fdss[i].fd == -1)
            {
                fdss[i].fd = cfd;
                fdss[i].addr = cliaddr;
                ptr = &fdss[i];
                break;
            }
        }

        pthread_t tid;
        pthread_create(&tid, NULL, working, ptr);
        pthread_detach(tid);
    }
}