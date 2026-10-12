#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <cstdint>
#include <cstring>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

// Структура для хранения данных телеметрии дальномера
struct LrfTelemetry {
    uint8_t targets_count = 0;
    std::vector<uint32_t> distances; // Дистанции до целей в метрах
};

class LdcLaserDriver {
private:
    int fd = -1;
    std::string port_name;

    // Потоковые переменные
    std::thread worker_thread;
    std::atomic<bool> is_running{false};
    std::mutex data_mutex;
    LrfTelemetry latest_telemetry;

    // Вспомогательный метод для подсчета контрольной суммы пакета
    uint8_t calculate_checksum(const uint8_t* data, size_t length) {
        uint32_t sum = 0;
        for (size_t i = 0; i < length; ++i) {
            sum += data[i];
        }
        return static_cast<uint8_t>(sum & 0xFF);
    }

    // Потоковый конечный автомат (State Machine) для разбора ответов ЛДЦ-20Х
    bool parse_byte(uint8_t byte, LrfTelemetry& out_data) {
        static int state = 0;
        static uint8_t cmd_id = 0;
        static uint8_t payload_len = 0;
        static std::vector<uint8_t> payload_buffer;
        static std::vector<uint8_t> checksum_buffer;

        switch (state) {
        case 0:
            if (byte == 0xEB) state = 1;
            break;
        case 1:
            if (byte == 0x90) {
                state = 2;
                payload_buffer.clear();
                checksum_buffer.clear();
            } else {
                state = 0;
            }
            break;
        case 2:
            cmd_id = byte;
            checksum_buffer.push_back(byte);
            state = 3;
            break;
        case 3:
            payload_len = byte;
            checksum_buffer.push_back(byte);
            if (payload_len > 0) state = 4;
            else state = 5;
            break;
        case 4:
            payload_buffer.push_back(byte);
            checksum_buffer.push_back(byte);
            if (payload_buffer.size() >= payload_len) state = 5;
            break;
        case 5:
            uint8_t calculated_cs = calculate_checksum(checksum_buffer.data(), checksum_buffer.size());
            state = 0;

            if (byte == calculated_cs) {
                if (cmd_id == 0xA1 && payload_buffer.size() >= 1) {
                    out_data.targets_count = payload_buffer[0];
                    out_data.distances.clear();

                    size_t expected_bytes = 1 + (out_data.targets_count * 4);
                    if (payload_buffer.size() == expected_bytes) {
                        for (int i = 0; i < out_data.targets_count; ++i) {
                            size_t idx = 1 + (i * 4);
                            uint32_t dist = (payload_buffer[idx] << 24) |
                                            (payload_buffer[idx+1] << 16) |
                                            (payload_buffer[idx+2] << 8) |
                                            payload_buffer[idx+3];
                            out_data.distances.push_back(dist);
                        }
                        return true;
                    }
                }
            } else {
                std::cerr << "[Внимание] Ошибка CRC пакета!" << std::endl;
            }
            break;
        }
        return false;
    }

    // Внутренний рабочий цикл потока
    void thread_worker() {
        uint8_t read_buf[256];
        LrfTelemetry local_telemetry;

        while (is_running) {
            // Отправляем команду замера (запрос) перед чтением порта каждую итерацию
            send_trigger_command();

            // Читаем все накопившиеся байты из буфера порта
            // Так как порт открыт в режиме O_NDELAY, read вернет управление сразу
            ssize_t num_bytes = read(fd, read_buf, sizeof(read_buf));
            if (num_bytes > 0) {
                for (ssize_t i = 0; i < num_bytes; ++i) {
                    if (parse_byte(read_buf[i], local_telemetry)) {
                        // Защищаем запись актуальных данных мьютексом
                        std::lock_guard<std::mutex> lock(data_mutex);
                        latest_telemetry = local_telemetry;
                    }
                }
            }

            // Засыпаем ровно на 5 секунд до следующего опроса
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

public:
    LdcLaserDriver(std::string port) : port_name(port) {}

    ~LdcLaserDriver() {
        stop_listen();
        close_port();
    }

    bool open_port(speed_t baud_rate = B115200) {
        fd = open(port_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) {
            std::cerr << "[Ошибка] Не удалось открыть порт: " << port_name << std::endl;
            return false;
        }

        struct termios options;
        tcgetattr(fd, &options);
        cfsetispeed(&options, baud_rate);
        cfsetospeed(&options, baud_rate);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_oflag &= ~OPOST;
        options.c_cc[VMIN] = 0;  // 0 для неблокирующего режима совмещенного с O_NDELAY
        options.c_cc[VTIME] = 0;

        tcsetattr(fd, TCSANOW, &options);
        return true;
    }

    void close_port() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

    bool send_trigger_command() {
        if (fd == -1) return false;
        uint8_t cmd[] = { 0xEB, 0x90, 0x21, 0x01, 0x01, 0x00 };
        cmd[5] = calculate_checksum(&cmd[2], 3);

        ssize_t bytes_written = write(fd, cmd, sizeof(cmd));
        return (bytes_written == sizeof(cmd));
    }

    // Запуск потока опроса
    void start_listen() {
        if (fd == -1 || is_running) return;
        is_running = true;
        worker_thread = std::thread(&LdcLaserDriver::thread_worker, this);
        std::cout << "[Инфо] Поток опроса дальномера запущен (интервал 5с)." << std::endl;
    }

    // Безопасная остановка потока
    void stop_listen() {
        if (is_running) {
            is_running = false;
            if (worker_thread.joinable()) {
                worker_thread.join();
            }
            std::cout << "[Инфо] Поток опроса дальномера остановлен." << std::endl;
        }
    }

    // Метод для безопасного получения последних данных из основного потока
    LrfTelemetry get_latest_data() {
        std::lock_guard<std::mutex> lock(data_mutex);
        return latest_telemetry;
    }
};

int main() {
    LdcLaserDriver lrf("/dev/ttyUSB0");

    if (!lrf.open_port(B115200)) {
        return -1;
    }

    // Запускаем асинхронное чтение в фоне
    lrf.start_listen();

    // Демонстрация работы главного потока: каждые 2 секунды проверяем, появились ли данные
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        LrfTelemetry data = lrf.get_latest_data();
        std::cout << "[Главный поток] Проверка данных... Найдено целей в буфере: " << (int)data.targets_count << std::endl;

        for (size_t t = 0; t < data.distances.size(); ++t) {
            std::cout << "  └─ Расстояние до цели " << t + 1 << ": " << data.distances[t] << " м" << std::endl;
        }
    }

    // Плавное завершение при выходе из main
    lrf.stop_listen();
    return 0;
}