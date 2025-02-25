#!/bin/zsh

# Определяем путь к директории, где лежит скрипт
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Путь к исполняемым файлам относительно директории скрипта
SERVER_EXEC="$SCRIPT_DIR/../build/server"  # Путь к серверному приложению
CLIENT_EXEC="$SCRIPT_DIR/../build/client"  # Путь к клиентскому приложению

# Функция для проверки, запущено ли приложение
is_running() {
    pgrep -f "$1" > /dev/null 2>&1
}

# Запуск серверного и клиентского приложений в новых терминалах
start_apps() {
    if is_running "$SERVER_EXEC"; then
        echo "Server is already running."
    else
        osascript -e "tell application \"Terminal\" to do script \"cd '$SCRIPT_DIR/../build' && ./server\"" &
        echo "Server started in a new terminal."
    fi

    if is_running "$CLIENT_EXEC"; then
        echo "Client is already running."
    else
        osascript -e "tell application \"Terminal\" to do script \"cd '$SCRIPT_DIR/../build' && ./client\"" &
        echo "Client started in a new terminal."
    fi
}

# Остановка серверного и клиентского приложений
stop_apps() {
    pkill -f "$SERVER_EXEC"
    pkill -f "$CLIENT_EXEC"
    echo "Server and Client stopped."
}

# Проверка аргумента и выполнение соответствующего действия
if [[ "$1" == "start" ]]; then
    start_apps
elif [[ "$1" == "stop" ]]; then
    stop_apps
else
    echo "Usage: $0 {start|stop}"
    exit 1
fi
