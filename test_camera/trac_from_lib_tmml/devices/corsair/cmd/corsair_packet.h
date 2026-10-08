#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <vector>
#include <stddef.h>

#define CMD_WR_REQ  (0x00u)
#define CMD_WR_RES  (0x01u)

#define CMD_RD_REQ  (0x80u)
#define CMD_RD_RES  (0x81u)

#define ST_NEED_RES (0x80u)

/** Максимальное количество добавленных модулей (размер массива адресов) */
#define MAX_MODULES 10
/** Максимальное количество добавленных плат (размер массива адресов) */
#define MAX_BOARDS 13
/** Максимальное количество добавленных буферов  */
#define MAX_BUFFERS 4
/** Размер заголовка сообщения в байтах */
#define VDB_MSG_HEADER_LENGTH 8
/** Максимальный размер сообщения */
#define MAX_MSG_SIZE 512

/** Команды */
enum vdb_cmd : uint8_t {
  /**  Запрос-Запись */
  vdb_cmd_request_write = 0x00,
  /** Ответ-Запись */
  vdb_cmd_reply_write   = 0x80,
  /** Запрос-Чтение */
  vdb_cmd_request_read  = 0x01,
  /** Ответ-Чтение  */
  vdb_cmd_reply_read    = 0x81
} ;

/** Статус */
enum vdb_status : uint8_t {
  /** Запрос без ответа  */
  vdb_st_request          = 0x00,
  /** Запрос с ответом / Успешный ответ (0х80) / Ошибка (0х80 + код ошибки) */
  vdb_st_request_reply    = 0x80
} ;

/** Ошибки приёма/передачи сообщений */
enum vdb_er : uint8_t {
  vdb_er_null                  = 0x00, // работает заглушка
  vdb_er_success               = 0x01, // всё хорошо
  vdb_er_false_send            = 0x02, // всё плохо
  vdb_er_buffer_size_exceeded  = 0x4,  // превышен размер буфера сообщения
  vdb_er_module_not_exist      = 0x8,  // модуль не существует
  vdb_er_board_not_exist       = 0x16  // платы не существует
};

class Packet
{
public:
    typedef struct _tpacket {
        uint8_t cmd;
        uint8_t status;
        uint16_t src_addr;
        uint16_t dst_addr;
        uint16_t data_length;
        uint8_t data[];
    } tpacket;
    Packet(Packet *);
    Packet(const uint8_t & _cmd = 0, const uint8_t & _status = 0, const uint16_t & _dst_addr = 0, const uint16_t & _data_length = 0);
    virtual ~Packet();
    void set_cmd(const uint8_t & v) {_cmd = v;}
    void set_status(const uint8_t & v) {_status = v;}
    void set_src_addr(const uint16_t & v) {_src_addr = v;}
    void set_dst_addr(const uint16_t & v) {_dst_addr = v;}
    void set_data_length(const uint16_t & v) {_data_length = v;}
    void set_data(const std::vector<uint8_t> & v) {_data = v;}
    void clear_data() {_data.clear();}
    void append_data(const uint8_t *, const size_t &);
    void append_data(const bool &);
    void append_data(const float &);
    void append_data(const double &);
    void append_data(const int8_t &);
    void append_data(const int16_t &);
    void append_data(const int32_t &);
    void append_data(const int64_t &);
    void append_data(const uint8_t &);
    void append_data(const uint16_t &);
    void append_data(const uint32_t &);
    void append_data(const uint64_t &);
    void append_data(const std::vector<uint8_t>& data);
    uint8_t cmd() const {return _cmd;}
    uint8_t status() const {return _status;}
    uint16_t src_addr() const {return _src_addr;}
    uint16_t dst_addr() const {return _dst_addr;}
    std::vector<uint8_t> data() const {return _data;}
    uint8_t * ptr_data(size_t & data_len) {data_len = _data.size(); return _data.data();}
    virtual uint32_t to_buffer(uint8_t *);
    virtual bool from_buffer(const uint8_t *, const uint32_t &);
protected:
    uint8_t _cmd;
    uint8_t _status;
    uint16_t _src_addr;
    uint16_t _dst_addr;
    uint16_t _data_length;
    std::vector<uint8_t> _data;
};

#endif // PACKET_H
