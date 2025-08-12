#include "include/Server.hpp"

int main()
{
    Server var(AF_INET, SOCK_STREAM, 0, 8080, 0x7F000001);
    return 1;
}