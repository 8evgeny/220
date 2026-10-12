#include <iostream>
#include <fcntl.h> 
#include <termios.h> 
#include <unistd.h> 
#include <vector>
#include <cstdint>
#include <cstring>

// Структура для хранения данных телеметрии дальномера
struct LrfTelemetry {
    uint8_t targets_count = 0;
    std::vector<uint32_t> distances; // Дистанции до целей в метрах
};

class LdcLaserDriver {
private:
    int fd = -1;
    std::string port_name;

    // Вспомогательный метод для подсчета контрольной суммы пакета
    uint8_t calculate_checksum(const uint8_t* data, size_t length) {
        uint32_t sum = 0;
        for (size_t i = 0; i < length; ++i) {
            sum += data[i];
        }
        return static_cast<uint8_t>(sum & 0xFF);
    }

public:
    LdcLaserDriver(std::string port) : port_name(port) {}
    
    ~LdcLaserDriver() {
        close_port();
    }

    // Инициализация и настройка UART порта в Linux
    bool open_port(speed_t baud_rate = B115200) {
        fd = open(port_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) {
            std::cerr << "[Ошибка] Не удалось открыть порт: " << port_name << std::endl;
            return false;
        }

        struct termios options;
        tcgetattr(fd, &options);

        // Настройка скорости
        cfsetispeed(&options, baud_rate);
        cfsetospeed(&options, baud_rate);

        // Режим 8N1 (8 бит данных, без четности, 1 стоповый бит)
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;

        // Отключение аппаратного и программного управления потоком
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
        
        // Перевод порта в "сырой" (raw) режим ввода-вывода
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_oflag &= ~OPOST;

        // Установка таймаутов чтения
        options.c_cc[VMIN] = 1;  // Читать минимум по 1 байту
        options.c_cc[VTIME] = 1; // Таймаут 0.1 секунды

        tcsetattr(fd, TCSANOW, &options);
        return true;
    }

    void close_port() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

    // Отправка команды на одиночное измерение дистанции
    bool send_trigger_command() {
        if (fd == -1) return false;

        // Пакет: Header(EB 90), CMD(21), LEN(01), DATA(01), CS(A4)
        uint8_t cmd[] = { 0xEB, 0x90, 0x21, 0x01, 0x01, 0x00 };
        // Вычисляем CS для байт: CMD, LEN, DATA
        cmd[5] = calculate_checksum(&cmd[2], 3); 

        ssize_t bytes_written = write(fd, cmd, sizeof(cmd));
        return (bytes_written == sizeof(cmd));
    }

    // Потоковый конечный автомат (State Machine) для разбора ответов ЛДЦ-20Х
    bool parse_byte(uint8_t byte, LrfTelemetry& out_data) {
        static int state = 0;
        static uint8_t cmd_id = 0;
        static uint8_t payload_len = 0;
        static std::vector<uint8_t> payload_buffer;
        static std::vector<uint8_t> checksum_buffer;

        switch (state) {
            case 0: // Ожидание первого байта заголовка
                if (byte == 0xEB) state = 1;
                break;

            case 1: // Ожидание второго байта заголовка
                if (byte == 0x90) {
                    state = 2;
                    payload_buffer.clear();
                    checksum_buffer.clear();
                } else {
                    state = 0;
                }
                break;

            case 2: // Получение ID Команды/Ответа
                cmd_id = byte;
                checksum_buffer.push_back(byte);
                state = 3;
                break;

            case 3: // Получение Длины Payload
                payload_len = byte;
                checksum_buffer.push_back(byte);
                if (payload_len > 0) {
                    state = 4;
                } else {
                    state = 5; // Если данных нет, сразу ждем контрольную сумму
                }
                break;

            case 4: // Сбор данных (Payload)
                payload_buffer.push_back(byte);
                checksum_buffer.push_back(byte);
                if (payload_buffer.size() >= payload_len) {
                    state = 5;
                }
                break;

            case 5: // Проверка контрольной суммы (Checksum)
                uint8_t calculated_cs = calculate_checksum(checksum_buffer.data(), checksum_buffer.size());
                state = 0; // Сброс автомата для следующего пакета

                if (byte == calculated_cs) {
                    // Если это пакет телеметрии с дистанцией (0xA1)
                    if (cmd_id == 0xA1 && payload_buffer.size() >= 1) {
                        out_data.targets_count = payload_buffer[0];
                        out_data.distances.clear();

                        // Извлекаем по 4 байта (uint32_t) для каждой обнаруженной цели
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
                            return true; // Пакет успешно собран и валиден
                        }
                    }
                } else {
                    std::cerr << "[Внимание] Ошибка CRC пакета!" << std::endl;
                }
                break;
        }
        return false;
    }

    // Основной цикл прослушивания порта (обычно выносится в std::thread)
    void listen_loop() {
        if (fd == -1) return;
        
        uint8_t read_buf[256];
        LrfTelemetry telemetry;

        std::cout << "[Инфо] Слушаем порт дальномера..." << std::endl;
        while (true) {
            ssize_t num_bytes = read(fd, &read_buf, sizeof(read_buf));
            if (num_bytes > 0) {
                for (ssize_t i = 0; i < num_bytes; ++i) {
                    if (parse_byte(read_buf[i], telemetry)) {
                        // Данные успешно разобраны!
                        std::cout << "\n🎯 Зафиксировано целей: " << (int)telemetry.targets_count << std::endl;
                        for (size_t t = 0; t < telemetry.distances.size(); ++t) {
                            std::cout << " └─ Цель №" << t + 1 << ": " << telemetry.distances[t] << " метров" << std::endl;
                        }
                    }
                }
            }
            usleep(1000); // Защита от 100% загрузки процессора (1 мс)
        }
    }
};

int main() {
    // Укажите ваш порт (например, переходник USB-RS422 или аппаратный UART)
    LdcLaserDriver lrf("/dev/ttyUSB0");

    if (!lrf.open_port(B115200)) {
        return -1;
    }

    // Пример: Инициируем выстрел лазера
    std::cout << "[Управление] Отправка команды замера..." << std::endl;
    lrf.send_trigger_command();

    // Запуск бесконечного чтения ответов
    lrf.listen_loop();

    return 0;
}
