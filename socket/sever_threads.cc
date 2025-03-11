#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
using namespace std;

int fds[1024];

void *working(void *arg)
{
    int cfd = *(int *)arg;
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
    *(int *)arg = -1;

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

    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
        perror("bind");

    ret = listen(lfd, 128);
    if (ret == -1)
        perror("listen");

    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    memset(fds, -1, sizeof(fds));

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

        int *ptr = NULL;
        for (int i = 0; i < sizeof(fds) / sizeof(int); i++)
        {
            if (fds[i] == -1)
            {
                fds[i] = cfd;
                ptr = &fds[i];
                break;
            }
        }

        pthread_t tid;
        pthread_create(&tid, NULL, working, ptr);
        pthread_detach(tid);
    }
}