#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define FIFO_PATH "/tmp/ipc_fifo"

// Функция для удаления FIFO при завершении программы
void cleanup(int sig) {
    if (unlink(FIFO_PATH) == -1) {
        std::cerr << "Ошибка при удалении FIFO: " << strerror(errno) << std::endl;
    } else {
        std::cout << "FIFO файл удален.\n";
    }
    exit(0);
}

int main() {
    // Установка обработчика сигнала SIGINT (Ctrl+C)
    signal(SIGINT, cleanup);

    // Создание FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        std::cerr << "Ошибка создания FIFO\n";
        return 1;
    }

    std::cout << "Сервер запущен, ждет клиентов...\n";

    // Открытие FIFO для записи
    int fifo_fd = open(FIFO_PATH, O_WRONLY);
    if (fifo_fd == -1) {
        std::cerr << "Не удалось открыть FIFO для записи\n";
        return 1;
    }

    std::string message = "Hello from server!";
    if (write(fifo_fd, message.c_str(), message.size() + 1) == -1) {
        std::cerr << "Ошибка при отправке сообщения\n";
        close(fifo_fd);
        return 1;
    }

    std::cout << "Сообщение отправлено\n";

    close(fifo_fd);

    // Программа будет ждать SIGINT (Ctrl+C) для удаления FIFO
    while (true) {
        pause();  // Ожидаем сигналы
    }

    return 0;
}
