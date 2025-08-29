#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

int main()
{
    std::vector<pollfd> fds(1);

    fds[0].fd = 10;
    std::cout << fds.data() << std::endl;

    int test = poll(fds.data(), fds.size(), 129)
}