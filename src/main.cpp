#include <iostream>
#include <cstring>
#include "client.h"

#define FIFO_PATH "/tmp/ipc_fifo"

int main() {
    std::cout << "Клиент запущен, ждем сообщения от сервера...\n";
    receive_message(FIFO_PATH);
    return 0;
}
