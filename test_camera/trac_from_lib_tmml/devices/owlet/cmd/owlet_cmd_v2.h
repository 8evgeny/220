#ifndef CMD_V2_H
#define CMD_V2_H

#include <stdint.h>
#include <vector>

/** Команды */
typedef enum {
  /**  Запрос-Запись */
  owlet_vdb_cmd_request_write = 0x00,
  /** Ответ-Запись */
  owlet_vdb_cmd_reply_write   = 0x80,
  /** Запрос-Чтение */
  owlet_vdb_cmd_request_read  = 0x01,
  /** Ответ-Чтение  */
  owlet_vdb_cmd_reply_read    = 0x81
} cmd;

/** Статус */
typedef enum {
  /** Запрос без ответа  */
  owlet_vdb_st_request          = 0x00,
  /** Запрос с ответом / Успешный ответ (0х80) / Ошибка (0х80 + код ошибки) */
  owlet_vdb_st_request_reply    = 0x80
} status;

/** Адреса внешних плат */
typedef enum {
  en_vbd_host        = 0x00,
  en_vbd_board    	 = 0x01,
  en_vbd_GSP         = 0x02,
  en_vbd_tv_camera   = 0x03,
  en_vbd_mwir_camera = 0x0C
} boards_id;

typedef enum{
    telemetry_addr = 0x00
} host_addr;

/***Адресное пространство платы управления***/
typedef enum{
    g_current_stream = 0x010B,
    g_dest_stream    = 0x010C,
    g_agc            = 0x010D
} owl_board_addr;

/********Адресное пространство ГСП**********/
typedef enum{
    gsp_addr_state_z           = 0x0202,
    gsp_addr_state_x           = 0x0203,
    gsp_addr_state             = 0x0204,
    gsp_addr_cur_angle_z       = 0x0205,
    gsp_addr_cur_angle_x       = 0x0206,
    gsp_addr_cur_angle_deg_z   = 0x0207,
    gsp_addr_cur_angle_deg_x   = 0x0208,
    gsp_addr_zero_pos_z        = 0x0209,
    gsp_addr_zero_pos_x        = 0x020A,
    gsp_addr_arretir           = 0x020B,
    gsp_addr_motor             = 0x020C,
    gsp_addr_el_arr            = 0x020D,
    gsp_addr_stab              = 0x020E,
    gsp_addr_target_position_z = 0x020F,
    gsp_addr_target_position_x = 0x0210,
    gsp_addr_target_speed_z    = 0x0211,
    gsp_addr_target_speed_x    = 0x0212,
    gsp_addr_target_angle_z    = 0x0213,
    gsp_addr_target_angle_x    = 0x0214,
    gsp_addr_target_position   = 0x0215,
    gsp_addr_drift_comp        = 0x0216
} owl_gsp_board_addr;

/********Адресное пространство ТПВ камеры**********/
typedef enum{
    lwir_addr_camlink_en        = 0x0C00,
    lwir_addr_snr_dde_base      = 0x0C01,
    lwir_addr_base_threshold    = 0x0C02,
    lwir_addr_dde_alpha         = 0x0C03,
    lwir_addr_dde_gain_max      = 0x0C04,
    lwir_addr_dde_contrast      = 0x0C05,
    lwir_addr_dde_brightness    = 0x0C06,
    lwir_addr_dde_inversion     = 0x0C07,
    lwir_addr_temperature       = 0x0C08,
    lwir_addr_left_table_code   = 0x0C09,
    lwir_addr_right_table_code  = 0x0C0A
} owl_lwir_camera_addr;

typedef enum{
    all_ax = 0,
    x_ax   = 1,
    z_ax   = 2
} owl_gsp_axis;

typedef enum{
    off = 0,
    on  = 1
} owl_gsp_mode;

typedef enum{
    null_position = 0,
    park_position = 1
} owl_gsp_target_position;

typedef enum{
    auto_drift_compensation   = 1,
    manual_drift_compensation = 2,
    horizont_tracking		  = 3
} owl_gsp_talgorithm;

class Cmd_v2
{
public:
    typedef struct _cmd {
        uint8_t  command;
        uint8_t  status;
        uint16_t src_addr;
        uint16_t dst_addr;
        uint16_t data_length;
        uint8_t data[];
    } cmd;
    Cmd_v2(Cmd_v2 *);
    Cmd_v2(const uint16_t & dst_addr = 0,
           const uint8_t  & command  = owlet_vdb_cmd_request_write,
           const uint8_t  & status   = owlet_vdb_st_request,
           const uint16_t & src_addr = (en_vbd_host << 8));
    virtual ~Cmd_v2();

    void set_cmd(const uint8_t &);
    void set_status(const uint8_t &);
    void set_src_addr(const uint16_t &);
    void set_dst_addr(const uint16_t &);
    void set_data(const std::vector<uint8_t> &);
    void clear_data();
    void append_data(const uint8_t *v, const uint32_t & l);
    void append_data(const uint8_t & v);
    void append_data(const uint16_t & v);
    void append_data(const uint32_t &);

    uint8_t get_cmd();
    uint8_t get_status();
    uint16_t get_src_addr();
    uint16_t get_dst_addr();
    std::vector<uint8_t> get_data();
    uint32_t get_data_len();
    uint8_t * get_data(uint32_t & data_len);
    uint32_t to_buffer(uint8_t *);
    virtual bool from_buffer(const uint8_t *, const uint32_t &);
protected:
    uint8_t  command;
    uint8_t  status;
    uint16_t src_addr;
    uint16_t dst_addr;
    std::vector<uint8_t> data;
};

#endif // CMD_V2_H
