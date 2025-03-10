#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
using namespace std;

int main()
{
    // 创建监听的套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
        perror("socket");

    // 绑定ip和端口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;         // ipv4
    addr.sin_port = htons(8989);       // 端口网络字节序
    addr.sin_addr.s_addr = INADDR_ANY; // 绑定所有IP
    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
        perror("bind");

    // 设置监听
    ret = listen(lfd, 128);
    if (ret == -1)
        perror("listen");

    // 等待并接受客户端的连接
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &clilen);
    if (cfd == -1)
        perror("accept");

    // 通信
    while (1)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        ssize_t read_len = recv(cfd, buf, sizeof(buf), 0);
        if (read_len == 0)
        {
            cout << "客户端断开连接" << endl;
            break;
        }
        else if (read_len > 0)
        {
            cout << "recv buf:" << buf << endl;
            // 回复数据
            send(cfd, buf, strlen(buf) + 1, 0);
        }
        else
        {
            perror("recv");
            break;
        }
    }

    // 断开连接
    close(cfd);
    close(lfd);
    return 0;
}