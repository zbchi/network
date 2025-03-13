#include <iostream>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <cstring>
using namespace std;

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
        perror("socket");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8989);

    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
        perror("bind");

    ret = listen(lfd, 128);
    if (ret == -1)
        perror("listen");

    fd_set reads, tmp;
    FD_ZERO(&reads);
    FD_SET(lfd, &reads);
    int nfds = lfd;

    // 委托内核检测集合中的文件描述符
    while (1)
    {
        tmp = reads;
        int num = select(nfds + 1, &tmp, NULL, NULL, NULL);
        cout << "num=" << num << endl;
        for (int i = 0; i <= nfds; i++)
        {
            if (i == lfd && FD_ISSET(lfd, &tmp))
            {
                // 建立新连接
                int cfd = accept(lfd, NULL, NULL);
                // cfd添加到检测的原始集合中
                FD_SET(cfd, &reads);
                nfds = nfds < cfd ? cfd : nfds;
            }
            else
            {
                // 通信
                if (FD_ISSET(i, &tmp))
                {
                    char buf[1024];
                    memset(buf, 0, sizeof(buf));
                    int len = recv(i, buf, sizeof(buf), 0);
                    if (len == 0)
                    {
                        cout << "客户端断开连接" << endl;
                        FD_CLR(i, &reads);
                        close(i);
                    }
                    else if (len > 0)
                    {
                        cout << "recv data:" << buf << endl;
                        send(i, buf, len, 0);
                    }
                    else
                    {
                        perror("recv");
                        break;
                    }
                }
            }
        }
    }
    close(lfd);
    return 0;
}
