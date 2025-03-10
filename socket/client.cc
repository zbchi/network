#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main()
{
    // 创建通信套接字
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1)
        perror("socket");

    // 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8989);

    // 将10.30.1.227->大端int
    inet_pton(AF_INET, "10.30.1.227", &addr.sin_addr.s_addr);
    int ret = connect(cfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
        perror("connect");

    // 通信
    int num = 0;
    while (1)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "hello world,%d", num++);

        send(cfd, buf, strlen(buf) + 1, 0);

        ssize_t len = recv(cfd, buf, sizeof(buf), 0);
        if (len == 0)
        {
            cout << "已经断开连接" << endl;
            break;
        }
        else if (len > 0)
        {
            cout << "recv buf:" << buf << endl;
        }
        else
        {
            perror("recv");
            break;
        }
        sleep(1);
    }
    close(cfd);
    return 0;
}