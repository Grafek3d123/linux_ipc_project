#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <thread>   // Для многопоточности

struct msg_buffer {
    long msg_type;
    char msg_text[11];  // 10 символов + 1 для '\0'
    pid_t msg_PID;
};

class Server {
private:
    int msgid;

    // Функция для обработки одного сообщения в отдельном потоке
    void process_message(msg_buffer message) {
        pid_t client_pid = message.msg_PID;
        std::string msg_text(message.msg_text);

        std::cout << "Message received: " << msg_text;
        std::cout << " From PID: " << client_pid << std::endl;

        std::reverse(msg_text.begin(), msg_text.end());  // Разворачиваем текст
        std::cout << "Message reversed: " << msg_text << std::endl << std::endl;

        // Отправляем ответ клиенту
        msg_buffer response;
        response.msg_type = 2;  // Тип ответа
        strncpy(response.msg_text, msg_text.c_str(), 10);
        response.msg_text[10] = '\0';
        response.msg_PID = client_pid;

        if (msgsnd(msgid, &response, sizeof(msg_buffer), 0) == -1) {
            std::cerr << "Failed to send response to client" << std::endl;
        }
    }

public:
    Server() {
        key_t key = ftok("progfile", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        if (msgid == -1) {
            throw std::runtime_error("Failed to create message queue");
        } else {
            std::cout << "msgidServer = " << msgid << std::endl;
        }
    }

    ~Server() {
        msgctl(msgid, IPC_RMID, nullptr);
    }

    void run() {
        while (true) {
            msg_buffer message;
            // Получаем сообщение из очереди
            if (msgrcv(msgid, &message, sizeof(msg_buffer), 1, 0) == -1) {
                perror("Failed to receive message");
                continue;
            }

            // Создаем новый поток для обработки этого сообщения
            std::thread(&Server::process_message, this, message).detach();  // detach позволяет не ждать завершения потока
        }
    }
};

int main() {
    try {
        Server server;
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
