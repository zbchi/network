#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
using namespace std;
// 服务器端就是发送广播的一端,端口自动绑定
int main()
{

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        perror("socket");

    // 设置广播属性
    struct in_addr addr;
    addr.s_addr = inet_pton(AF_INET, "10.30.255.255", &addr.s_addr);
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr));

    // 初始化数据接收端的地址信息
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(8080);
    cliaddr.sin_addr.s_addr = addr.s_addr;

    int num = 0;
    char buf[1024];
    while (1)
    {
        sprintf(buf, "broadcast:%d\n", num++);

        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
        sleep(1);
    }
    close(fd);
    return 0;
}
