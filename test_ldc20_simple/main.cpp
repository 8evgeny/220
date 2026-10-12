#include <iostream>
#include <fstream>
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
#include <iomanip>
#include <sstream>
#include <sys/select.h>

// Структура для хранения данных о дистанциях
struct LrfDistanceData {
    uint8_t targets_count = 0;
    std::vector<uint32_t> distances; // Дистанции до целей в метрах
};

// Структура для хранения системной телеметрии/статуса модуля
struct LrfSystemStatus {
    uint8_t error_code = 0;          // 0x00 - Ошибок нет
    int8_t temperature = 0;          // Температура модуля в °C
    bool is_charged = false;         // Статус готовности конденсатора накачки
    bool is_valid = false;           // Флаг того, что данные были успешно получены
};

class LdcLaserDriver {
private:
    int fd = -1;
    std::string port_name;
    std::string log_filename;
    std::ofstream log_file;

    // Потоковые переменные и защита данных
    std::thread worker_thread;
    std::atomic<bool> is_running{false};
    std::mutex data_mutex;
    std::mutex log_mutex; // Отдельный мьютекс для безопасной записи в файл

    LrfDistanceData latest_distances;
    LrfSystemStatus latest_status;

    // Метод получения текущего времени в формате строки (ГГГГ-ММ-ДД ЧЧ:ММ:СС.ммм)
    std::string get_current_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << millis;
        return ss.str();
    }

    // Вспомогательный метод для подсчета контрольной суммы пакета
    uint8_t calculate_checksum(const uint8_t* data, size_t length) {
        uint32_t sum = 0;
        for (size_t i = 0; i < length; ++i) {
            sum += data[i];
        }
        return static_cast<uint8_t>(sum & 0xFF);
    }

    // Запись строки лога в CSV файл
    void write_to_log(const std::string& type, const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        if (log_file.is_open()) {
            log_file << get_current_timestamp() << "," << type << "," << message << "\n";
            log_file.flush(); // Принудительно сбрасываем буфер на диск
        }
    }

    // Универсальный потоковый конечный автомат (State Machine)
    void parse_byte(uint8_t byte) {
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
                std::lock_guard<std::mutex> lock(data_mutex);

                // ВАРИАНТ А: Пакет с дистанцией от импульса (0xA1)
                if (cmd_id == 0xA1 && !payload_buffer.empty()) {
                    latest_distances.targets_count = payload_buffer[0];
                    latest_distances.distances.clear();

                    size_t expected_bytes = 1 + (latest_distances.targets_count * 4);
                    if (payload_buffer.size() == expected_bytes) {
                        std::string dist_log_str = std::to_string(latest_distances.targets_count);
                        for (int i = 0; i < latest_distances.targets_count; ++i) {
                            size_t idx = 1 + (i * 4);
                            uint32_t dist = (payload_buffer[idx] << 24) |
                                            (payload_buffer[idx+1] << 16) |
                                            (payload_buffer[idx+2] << 8) |
                                            payload_buffer[idx+3];
                            latest_distances.distances.push_back(dist);
                            dist_log_str += ";" + std::to_string(dist);
                        }
                        write_to_log("DISTANCE", dist_log_str);
                    }
                }
                // ВАРИАНТ Б: Пакет системной телеметрии/статуса (0x81)
                else if (cmd_id == 0x81 && payload_buffer.size() == 3) {
                    latest_status.error_code = payload_buffer[0];
                    latest_status.temperature = static_cast<int8_t>(payload_buffer[1]);
                    latest_status.is_charged  = (payload_buffer[2] == 0x01);
                    latest_status.is_valid    = true;

                    std::string status_log_str = std::to_string(latest_status.error_code) + "," +
                                                 std::to_string(latest_status.temperature) + "," +
                                                 (latest_status.is_charged ? "1" : "0");
                    write_to_log("TELEMETRY", status_log_str);
                }
            } else {
                std::cerr << "[Внимание] Ошибка CRC пакета!" << std::endl;
                write_to_log("ERROR", "CRC checksum mismatch");
            }
            break;
        }
    }

    // Вспомогательный метод ожидания данных в UART (POSIX select)
    void wait_for_data(int timeout_ms) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        select(fd + 1, &read_fds, nullptr, nullptr, &tv);
    }

    void thread_worker() {
        uint8_t read_buf[256];

        while (is_running) {
            // 1. Запрос телеметрии
            send_status_request_command();
            wait_for_data(100); // Ожидаем ответа устройства до 100 мс

            ssize_t num_bytes = read(fd, read_buf, sizeof(read_buf));
            if (num_bytes > 0) {
                for (ssize_t i = 0; i < num_bytes; ++i) {
                    parse_byte(read_buf[i]);
                }
            }

            // 2. Команда на замер
            send_trigger_command();
            wait_for_data(200); // На замер и вычисление даём больше времени (до 200 мс)

            num_bytes = read(fd, read_buf, sizeof(read_buf));
            if (num_bytes > 0) {
                for (ssize_t i = 0; i < num_bytes; ++i) {
                    parse_byte(read_buf[i]);
                }
            }

            // Засыпаем строго на 5 секунд до следующей итерации
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

public:
    LdcLaserDriver(std::string port, std::string log_file_path = "ldc_telemetry.csv")
        : port_name(port), log_filename(log_file_path) {}

    ~LdcLaserDriver() {
        stop_listen();
        close_port();
        close_log();
    }

    bool open_log() {
        std::lock_guard<std::mutex> lock(log_mutex);
        log_file.open(log_filename, std::ios::out | std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "[Ошибка] Не удалось создать файл лога: " << log_filename << std::endl;
            return false;
        }

        log_file.seekp(0, std::ios::end);
        if (log_file.tellp() == 0) {
            log_file << "Timestamp,PacketType,PayloadData(TargetsCount;Dist1;Dist2 OR ErrCode,Temp,Charged)\n";
        }
        return true;
    }

    void close_log() {
        std::lock_guard<std::mutex> lock(log_mutex);
        if (log_file.is_open()) {
            log_file.close();
        }
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
        options.c_cc[VMIN] = 0;
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
    bool send_status_request_command() {
        if (fd == -1) return false;
        uint8_t cmd[] = { 0xEB, 0x90, 0x01, 0x00, 0x00 };
        cmd[4] = calculate_checksum(&cmd[2], 2);
        ssize_t bytes_written = write(fd, cmd, sizeof(cmd));
        return (bytes_written == sizeof(cmd));
    }
    void start_listen() {
        if (fd == -1 || is_running) return;
        open_log();
        is_running = true;
        worker_thread = std::thread(&LdcLaserDriver::thread_worker, this);
        std::cout << "[Инфо] Поток параллельного опроса и логирования запущен." << std::endl;
    }
    void stop_listen() {
        if (is_running) {
            is_running = false;
            if (worker_thread.joinable()) {
                worker_thread.join();
            }
            close_log();
            std::cout << "[Инфо] Поток опроса и логирования остановлен." << std::endl;
        }
    }
    LrfDistanceData get_latest_distances() {
        std::lock_guard<std::mutex> lock(data_mutex);
        return latest_distances;
    }
    LrfSystemStatus get_latest_status() {
        std::lock_guard<std::mutex> lock(data_mutex);
        return latest_status;
    }
};
int main() {
    LdcLaserDriver lrf("/dev/ttyUSB0", "ldc_telemetry.csv");
    if (!lrf.open_port(B115200)) {
        return -1;
    }
    lrf.start_listen();
    for (int i = 0; i < 6; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        LrfSystemStatus status = lrf.get_latest_status();
        LrfDistanceData dist_data = lrf.get_latest_distances();
        std::cout << "[Главный поток] Лог обновляется в фоне. Найдено целей: "
                  << (int)dist_data.targets_count << " | T: " << (int)status.temperature << "°C" << std::endl;
    }
    lrf.stop_listen();
    std::cout << "[Успех] Работа завершена. Проверьте файл 'ldc_telemetry.csv'." << std::endl;
    return 0;
}