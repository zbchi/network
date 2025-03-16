#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
using namespace std;
// 客户端启动后主动发送数据
// 自动随即绑定端口
int main()
{

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        perror("socket");

#if 0
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8989);
    addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
        perror("bind");
#endif

    char buf[1024];

    // 服务器地址
    struct sockaddr_in seraddr;
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(8989);
    inet_pton(AF_INET, "10.30.1.227", &seraddr.sin_addr.s_addr);

    int num = 0;
    while (1)
    {
        // 发送数据，发送给服务器
        sprintf(buf, "hello world,%d\n", num++);
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&seraddr, sizeof(seraddr));

        int len = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        if (len == -1)
            break;
        cout << "server say:" << buf << endl;
        sleep(1);
    }
    return 0;
}
