#include <arpa/inet.h>
#include <iostream>
#include <cstring>
using namespace std;
// 服务端启动后不发送数据,先接收数据
// 需要手动绑定端口
int main()
{

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        perror("socket");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8989);
    addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
        perror("bind");

    char ip[24];
    char buf[1024];
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    while (1)
    {
        // 接收数据
        int len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &clilen);
        if (len == -1)
            break;

        cout << "client ip:" << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip)) << endl;
        cout << "client port:" << ntohs(cliaddr.sin_port) << endl;
        cout << "client say:" << buf << endl;
        // 回复
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&cliaddr, clilen);
    }
    return 0;
}
