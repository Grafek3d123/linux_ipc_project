// client.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct msg_buffer {
    long msg_type;  // PID клиента
    char msg_text[11];  // 10 символов + 1 для '\0'
    pid_t msg_PID;
};

class Client {
private:
    int msgid;
    pid_t client_pid;

public:
    Client() : client_pid(getpid()) {
        key_t key = ftok("progfile", 65);
        msgid = msgget(key, 0666);
        if (msgid == -1) {
            throw std::runtime_error("Failed to access message queue");
        } else {
            std::cout << "msgid Client = " << msgid << std::endl;
            std::cout << "pid Client = " << client_pid << std::endl;
        }
    }

    void sendMessage(const std::string& message) {
        if (message.length() > 10) {
            throw std::runtime_error("Message too long (max 10 characters)");
        }

        msg_buffer request;
        request.msg_PID = client_pid;
        strncpy(request.msg_text, message.c_str(), 10);
        request.msg_text[10] = '\0';
        request.msg_type = 1;  // request type

        if (msgsnd(msgid, &request, sizeof(msg_buffer), 0) == -1) {
            throw std::runtime_error("Failed to send message");
        }
    }

    void receiveMessage() {
        msg_buffer response;
        if (msgrcv(msgid, &response, sizeof(msg_buffer), 2, 0) == -1) {
            throw std::runtime_error("Failed to receive response");
        }

        std::cout << "Received response: " << response.msg_text << std::endl;
    }
};

int main() {
    try {
        Client client;
        std::string input;
        
        std::cout << "Enter string (up to 10 characters): ";
        std::getline(std::cin, input);

        client.sendMessage(input);
        client.receiveMessage();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
