#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include "client.h"

void receive_message(const std::string& fifo_path) {
    char buffer[256];

    // Открытие FIFO для чтения
    int fifo_fd = open(fifo_path.c_str(), O_RDONLY);
    if (fifo_fd == -1) {
        std::cerr << "Не удалось открыть FIFO для чтения\n";
        return;
    }

    read(fifo_fd, buffer, sizeof(buffer)); // Чтение сообщения
    std::cout << "Получено сообщение: " << buffer << std::endl;

    close(fifo_fd);
}
