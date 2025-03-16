#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>
using namespace std;
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

    // 创建epoll模型
    int epfd = epoll_create(100);
    if (epfd == -1)
        perror("epoll_create");

    // 将要检测的节点添加到epoll模型中
    struct epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1)
        perror("epoll_ctl");

    // 不停地委托内核检测epoll模型中的文件描述符状态
    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(evs[0]);
    while (1)
    {
        int num = epoll_wait(epfd, evs, size, -1);
        cout << "num=" << num << endl;
        // 遍历evs数组，个数就是返回值
        for (int i = 0; i < num; i++)
        {
            // 取出数组原始元素中的文件描述符
            int curfd = evs[i].data.fd;
            if (curfd == lfd)
            {
                int cfd = accept(lfd, NULL, NULL);

                // 将通信的描述符设置为非阻塞
                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                ev.data.fd = cfd;
                ev.events = EPOLLIN | EPOLLET; // 边沿模式
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                if (ret == -1)
                    perror("epoll_ctl");
            }
            else
            {
                char buf[5];
                memset(buf, 0, sizeof(buf));

                while (1)
                {
                    int len = recv(curfd, buf, sizeof(buf), 0);
                    if (len > 0)
                    {
                        cout << "recv:" << buf << endl;
                        send(curfd, buf, len, 0);
                    }
                    else if (len == 0)
                    {
                        cout << "客户端断开连接" << endl;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                        close(curfd);
                        break;
                    }
                    else
                    {
                        if (errno == EAGAIN)
                        {
                            cout << "recv done" << endl;
                            break;
                        }
                        else
                            perror("recv error");
                    }
                }
            }
        }
    }
    close(lfd);
    return 0;
}