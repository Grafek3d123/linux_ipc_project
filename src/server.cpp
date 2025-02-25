#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <fstream>  // Для записи в файл

struct msg_buffer {
    long msg_type;
    char msg_text[11];  // 10 символов + 1 для '\0'
    pid_t msg_PID;
};

class Server {
private:
    int msgid;
    std::mutex log_mutex;  // Мьютекс для синхронизации доступа к файлу

    // Функция для записи логов в файл
    void log_to_file(const std::string& timestamp, const std::string& original_msg, const std::chrono::nanoseconds& duration) {
        std::lock_guard<std::mutex> lock(log_mutex);  // Автоматически блокирует и разблокирует мьютекс

        std::ofstream log_file("server_log.txt", std::ios_base::app);
        if (log_file.is_open()) {
            log_file << "Timestamp: " << timestamp << ", ";
            log_file << "Original Message: " << original_msg << ", ";
            log_file << "Processing Time: " << duration.count() << " ns\n";
        } else {
            std::cerr << "Failed to open log file" << std::endl;
        }
    }

    // Функция для обработки одного сообщения
    void process_message(msg_buffer message) {
        using namespace std::chrono;

        // Получаем время получения сообщения
        auto start_time = high_resolution_clock::now();
        time_t current_time = system_clock::to_time_t(system_clock::now());
        std::string timestamp = ctime(&current_time);
        timestamp.pop_back(); // Убираем лишний символ новой строки

        pid_t client_pid = message.msg_PID;
        std::string msg_text(message.msg_text);

        std::cout << "Message received: " << msg_text;
        std::cout << " From PID: " << client_pid << std::endl;

        // Разворачиваем строку
        std::reverse(msg_text.begin(), msg_text.end());
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

        // Получаем время окончания обработки
        auto end_time = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end_time - start_time);

        // Логируем информацию о сообщении в файл
        std::reverse(msg_text.begin(), msg_text.end());
        log_to_file(timestamp, msg_text, duration);
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
