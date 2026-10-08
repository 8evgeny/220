#ifndef VDB_REGISTERS_H
#define VDB_REGISTERS_H

namespace corsair_vdb_registers {

/** Адреса внешних плат */
typedef enum {
    en_vbd_host           = 0x00,
    en_vbd_fixed_board    = 0x01,
    en_vbd_GSP            = 0x02,
    en_vbd_cpu            = 0x03,
    en_vbd_mobile_board   = 0x04,
    en_vbd_tv_objective   = 0x05,
    en_vbd_tv_camera      = 0x06,
    en_vbd_swir_objective = 0x07,
    en_vbd_swir_camera    = 0x08,
    en_vbd_mwir_objective = 0x09,
    en_vbd_mwir_camera    = 0x0A,
    en_vbd_LRF            = 0x0B
} t_vdb_boards_id;

/** Адреса модуля "Хост" */
typedef enum {
    en_vdb_host_telemetry      = 0x00,
    en_vdb_host_autofocus_view = 0x01
} t_host_registers;

/** Адреса внутренних модулей (0x0100 -> 0x00) */
typedef enum {
    en_vdb_fb_service_info           = 0x00,
    en_vdb_fb_telemtry               = 0x02,
    en_vdb_fb_command_handler        = 0x03,
    en_vdb_fb_flash                  = 0x04,
    en_vdb_fb_video_correction       = 0x08,
    en_vdb_fb_gsp_controller         = 0x11,
    en_vdb_fb_tracking               = 0x1F,
    en_vdb_fb_video_streams_control  = 0x30,
} t_fixed_board_registers;

/** Registers Video correction module */
typedef enum {
    en_vdb_vc_tv_gray_contrast = en_vdb_fb_video_correction,
    en_vdb_vc_tv_gray_brightness,
    en_vdb_vc_tv_color_contrast,
    en_vdb_vc_tv_color_brightness,
    en_vdb_vc_sw_contrast,
    en_vdb_vc_sw_brightness,
    en_vdb_vc_mw_contrast,
    en_vdb_vc_mw_brightness,
    en_vdb_vc_mw_inversion,
} t_fb_video_correction_registers;

/** Registers GSP controller module */
typedef enum {
    en_vdb_gc_kp_x = en_vdb_fb_gsp_controller,
    en_vdb_gc_ki_x,
    en_vdb_gc_kd_x,
    en_vdb_gc_integral_limit_x,
    en_vdb_gc_kp_z,
    en_vdb_gc_ki_z,
    en_vdb_gc_kd_z,
    en_vdb_gc_integral_limit_z,
    en_vdb_gc_angle_discrepancy_z,
    en_vdb_gc_angle_discrepancy_x,
    en_vdb_gc_drift_compensation_z,
    en_vdb_gc_drift_compensation_x,
    en_vdb_gc_max_speed_z,
    en_vdb_gc_max_speed_x
} t_fb_gsp_controller_registers;

/** Registers Tracking module */
typedef enum {
    en_vdb_tr_target_filter_k_x = en_vdb_fb_tracking,
    en_vdb_tr_target_filter_k_y,
    en_vdb_tr_target_filter_state,
    en_vdb_tr_speed_limit
} t_fb_tracking_registers;

/** Registers GSP */
typedef enum {
    en_vdb_gsp_device_id                 = 0x00,
    en_vdb_gsp_software_version          = 0x01,
    en_vdb_gsp_state_z                   = 0x02,
    en_vdb_gsp_state_x                   = 0x03,
    en_vdb_gsp_state                     = 0x04,
    en_vdb_gsp_curr_angle_z              = 0x05,
    en_vdb_gsp_curr_angle_x              = 0x06,
    en_vdb_gsp_curr_angle_z_deg          = 0x07,
    en_vdb_gsp_curr_angle_x_deg          = 0x08,
    en_vbd_gsp_zero_pos_z                = 0x09,
    en_vdb_gsp_zero_pos_x                = 0x0A,
    en_vdb_gsp_arretir                   = 0x0B,
    en_vdb_gsp_motor                     = 0x0C,
    en_vdb_gsp_el_arr                    = 0x0D,
    en_vdb_gsp_stab                      = 0x0E,
    en_vdb_gsp_target_pos_z              = 0x0F,
    en_vdb_gsp_target_pos_x              = 0x10,
    en_vbd_gsp_target_speed_z            = 0x11,
    en_vdb_gsp_target_speed_x            = 0x12,
    en_vbd_gsp_target_angle_z            = 0x13,
    en_vdb_gsp_target_angle_x            = 0x14,
    en_vdb_gsp_target_pos                = 0x15,
    en_vdb_gsp_manual_drift_compensation = 0x16,
    en_vdb_gsp_target_angle_z_f          = 0x17,
    en_vdb_gsp_target_angle_x_f          = 0x18,
    en_vbd_gsp_service_msg               = 0xff
} t_vdb_gsp_registers;

/** Registers cpu */
typedef enum {
    en_vdb_cpu_system_reset       = 0x00,
    en_vdb_cpu_system_version     = 0x01,
    en_vdb_cpu_telemetry_enable   = 0x02,
    en_vdb_cpu_telemetry_address  = 0x03,
    en_vdb_cpu_video_out_enable   = 0x04,
    en_vdb_cpu_tracker_enable     = 0x05,
    en_vdb_cpu_tracker_init       = 0x06, // 0x0306 - 0x0308
    en_vdb_cpu_tracker_deinit     = 0x09,
    en_vdb_cpu_telemetry_default  = 0x0A, // 0x030A - 0x0316
    en_vdb_cpu_telemetry          = 0xff
} t_vdb_cpu_registers;

/** Mobile board's modules */
typedef enum {
    en_vdb_mb_service_info           = 0x00,
    en_vdb_mb_telemtry               = 0x03,
    en_vdb_mb_autofocus              = 0x04,
    en_vdb_mb_field_sync             = 0x16,
    en_vdb_mb_serdes                 = 0x2A,
    en_vdb_mb_stream_mem_bridge      = 0x2C,
    en_vdb_mb_stream_mux             = 0x32,
    en_vdb_mb_video_statistics       = 0x33,
    en_vbd_mb_service_msg            = 0xff
} t_mobile_board_registers;

///** Адреса модуля "ЛДЦ" */
typedef enum {
    en_vdb_lrf_status             = 0x00,
    en_vdb_lrf_telemetry_enable   = 0x01,
    en_vdb_lrf_telemetry_address  = 0x02,
    en_vdb_lrf_telemetry_period   = 0x03,
    en_vdb_lrf_radiation_cntr     = 0x04,
    en_vdb_lrf_freq               = 0x05,
    en_vdb_lrf_test_mode          = 0x06,
    en_vdb_lrf_vavd               = 0x07,
    en_vdb_lrf_save_all_in_erom   = 0x08,
    en_vdb_lrf_power_on           = 0x09,
    en_vdb_lrf_distance           = 0x80,
    en_vdb_lrf_measure_statistics = 0x81,
    en_vdb_lrf_measure_amplitude  = 0x82,
    en_vdb_lrf_temperature        = 0x83,
    en_vdb_lrf_state              = 0x84
} t_vdb_lrf_registers;

/** Адреса модуля ТВ обектива (0x0400 -> 0x00) */
typedef enum {
    en_vdb_tv_lens_software_version_id           = 0x00,
    en_vdb_tv_lens_drive_coordinate              = 0x01,
    en_vdb_tv_lens_focal_length                  = 0x02, // [focal_length | iris_drive_coordinate / reserve]
    en_vdb_tv_lens_zoom_drive_speed              = 0x03, // Перемещение привода путем установки скорости (-255..-1 - «назад», 1..255 - «вперед», 0 - «стоп»)
    en_vdb_tv_lens_focus_drive_speed             = 0x04, //
    en_vdb_tv_lens_iris_drive_speed              = 0x05, // reserve
    en_vdb_tv_lens_target_zoom_drive_coordinate  = 0x06, // Позиционирование привода в координату
    en_vdb_tv_lens_target_focus_drive_coordinate = 0x07, // Позиционирование привода в координату
    en_vdb_tv_lens_target_iris_drive_coordinate  = 0x08, // reserve
    en_vdb_tv_lens_target_focal_length           = 0x09, // Позиционирование привода резкости в задаваемое фок. расст.
    en_vdb_tv_lens_current_temperature           = 0x0A,
    en_vdb_tv_lens_heater_state                  = 0x0B,
    en_vdb_tv_lens_auto_temp_control_state       = 0x0C,
    en_vdb_tv_lens_temp_thresh                   = 0x0D,
    en_vdb_tv_lens_collimator_intensity          = 0x0E,
    en_vdb_tv_lens_autofocus_state               = 0x0F,
    en_vdb_tv_lens_set_field                     = 0x010, // Установить поле (0 — широкое, 1 — среднее, 2 - узкое)
    en_vdb_tv_lens_telemetry_period              = 0x11,  // По умолчанию: 40 мс
    en_vdb_tv_lens_telemetry_recipient_address   = 0x12,  // По умолчанию: 0xFFFF
    en_vdb_tv_lens_tables_size                   = 0x13,
    en_vdb_tv_lens_limit_zoom_drive_coordinate   = 0x14,
    en_vdb_tv_lens_limit_focus_drive_coordinate  = 0x15,
    en_vdb_tv_lens_coarse_focus_drive_coordinate = 0x16,
    en_vbd_tv_lens_service_msg                   = 0xff
} t_vdb_tv_lens_registers;

/** Адреса модуля SW обектива */
typedef enum {
    en_vdb_sw_lens_software_version_id           = 0x00,
    en_vdb_sw_lens_drive_coordinate              = 0x01,
    en_vdb_sw_lens_focal_length                  = 0x02, // [focal_length | iris_drive_coordinate / reserve]
    en_vdb_sw_lens_zoom_drive_speed              = 0x03, // Перемещение привода путем установки скорости (-255..-1 - «назад», 1..255 - «вперед», 0 - «стоп»)
    en_vdb_sw_lens_focus_drive_speed             = 0x04, //
    en_vdb_sw_lens_iris_drive_speed              = 0x05, // reserve
    en_vdb_sw_lens_target_zoom_drive_coordinate  = 0x06, // Позиционирование привода в координату
    en_vdb_sw_lens_target_focus_drive_coordinate = 0x07, // Позиционирование привода в координату
    en_vdb_sw_lens_target_iris_drive_coordinate  = 0x08, // reserve
    en_vdb_sw_lens_target_focal_length           = 0x09, // Позиционирование привода резкости в задаваемое фок. расст.
    en_vdb_sw_lens_current_temperature           = 0x0A,
    en_vdb_sw_lens_heater_state                  = 0x0B,
    en_vdb_sw_lens_auto_temp_control_state       = 0x0C,
    en_vdb_sw_lens_temp_thresh                   = 0x0D,
    en_vdb_sw_lens_collimator_intensity          = 0x0E,
    en_vdb_sw_lens_autofocus_state               = 0x0F,
    en_vdb_sw_lens_set_field                     = 0x010, // Установить поле (0 — широкое, 1 — среднее, 2 - узкое)
    en_vdb_sw_lens_telemetry_period              = 0x11,  // По умолчанию: 40 мс
    en_vdb_sw_lens_telemetry_recipient_address   = 0x12,  // По умолчанию: 0xFFFF
    en_vdb_sw_lens_tables_size                   = 0x13,
    en_vdb_sw_lens_limit_zoom_drive_coordinate   = 0x14,
    en_vdb_sw_lens_limit_focus_drive_coordinate  = 0x15,
    en_vdb_sw_lens_coarse_focus_drive_coordinate = 0x16,
    en_vbd_sw_lens_service_msg                   = 0xff
} t_vdb_sw_lens_registers;

/** Адреса модуля SW сенсора */
typedef enum {
        en_vdb_sw_sensor_status        = 0x00,
        en_vdb_sw_sensor_gain_control  = 0x01,
} t_vdb_sw_sensor_registers;

/** Адреса модуля ТПВ-С обектива (0x0400 -> 0x00) */
typedef enum {
    en_vdb_mw_lens_software_version_id           = 0x00,
    en_vdb_mw_lens_drive_coordinate              = 0x01,
    en_vdb_mw_lens_focal_length                  = 0x02, // [focal_length | iris_drive_coordinate / reserve]
    en_vdb_mw_lens_zoom_drive_speed              = 0x03, // Перемещение привода путем установки скорости (-255..-1 - «назад», 1..255 - «вперед», 0 - «стоп»)
    en_vdb_mw_lens_focus_drive_speed             = 0x04, //
    en_vdb_mw_lens_iris_drive_speed              = 0x05, // reserve
    en_vdb_mw_lens_target_zoom_drive_coordinate  = 0x06, // Позиционирование привода в координату
    en_vdb_mw_lens_target_focus_drive_coordinate = 0x07, // Позиционирование привода в координату
    en_vdb_mw_lens_target_iris_drive_coordinate  = 0x08, // reserve
    en_vdb_mw_lens_target_focal_length           = 0x09, // Позиционирование привода резкости в задаваемое фок. расст.
    en_vdb_mw_lens_current_temperature           = 0x0A,
    en_vdb_mw_lens_heater_state                  = 0x0B,
    en_vdb_mw_lens_auto_temp_control_state       = 0x0C,
    en_vdb_mw_lens_temp_thresh                   = 0x0D,
    en_vdb_mw_lens_collimator_intensity          = 0x0E,
    en_vdb_mw_lens_autofocus_state               = 0x0F,
    en_vdb_mw_lens_set_field                     = 0x010, // Установить поле (0 — широкое, 1 — среднее, 2 - узкое)
    en_vdb_mw_lens_telemetry_period              = 0x11,  // По умолчанию: 40 мс
    en_vdb_mw_lens_telemetry_recipient_address   = 0x12,  // По умолчанию: 0xFFFF
    en_vdb_mw_lens_tables_size                   = 0x13,
    en_vdb_mw_lens_limit_zoom_drive_coordinate   = 0x14,
    en_vdb_mw_lens_limit_focus_drive_coordinate  = 0x15,
    en_vdb_mw_lens_coarse_focus_drive_coordinate = 0x16,
    en_vbd_mw_lens_service_msg                   = 0xff
} t_vdb_mw_lens_registers;

/* MW sensor */
typedef enum {
    // system settings
    en_vdb_mw_sensor_telemetry_enable             = 0x00,
    en_vdb_mw_sensor_firmware_version             = 0x01,
    en_vdb_mw_sensor_telemetry_address            = 0x02,
    en_vdb_mw_sensor_save_option_in_flash         = 0x03,
    en_vdb_mw_sensor_load_option_in_flash         = 0x04,
    en_vdb_mw_sensor_initialitation_defalt_value  = 0x05,
    // matrix settings
    en_vdb_mw_sensor_cce_input_ctrl       = 0x10,
    en_vdb_mw_sensor_pattern_ctrl         = 0x11,
    en_vdb_mw_sensor_direction_video      = 0x12,
    en_vdb_mw_sensor_black_while_video    = 0x13,
    en_vdb_mw_sensor_gpol                 = 0x14,
    en_vdb_mw_sensor_cce_min              = 0x15,
    en_vdb_mw_sensor_cce_max              = 0x16,
    en_vdb_mw_sensor_frame_period         = 0x17,
    en_vdb_mw_sensor_hl0                  = 0x18,
    en_vdb_mw_sensor_hl1                  = 0x19,
    en_vdb_mw_sensor_hl2                  = 0x1A,
    en_vdb_mw_sensor_hl3                  = 0x1B,
    en_vdb_mw_sensor_hl4                  = 0x1C,
    en_vdb_mw_sensor_sensor_serial_number = 0x1D,
    // DDE
    en_vdb_mw_sensor_dde_enable         = 0x30,
    en_vdb_mw_sensor_dde_base_mode      = 0x31,
    en_vdb_mw_sensor_dde_base_eps       = 0x32,
    en_vdb_mw_sensor_dde_gain_thrld     = 0x33,
    en_vdb_mw_sensor_dde_gain_max       = 0x34,
    en_vdb_mw_sensor_dde_alpha          = 0x35,
    en_vdb_mw_sensor_dde_hist_thrld     = 0x36,
    en_vdb_mw_sensor_dde_hist_thrld_aux = 0x37,
    // SNR
    en_vdb_mw_sensor_snr_enable         = 0x40,
    en_vdb_mw_sensor_snr_row_eps         = 0x41,
    en_vdb_mw_sensor_snr_col_eps         = 0x42,
    // NUC
    en_vdb_mw_sensor_nuc_calibration_enable       = 0x50,
    en_vdb_mw_sensor_nuc_bad_pixel_replace_enable = 0x51,
    en_vdb_mw_sensor_nuc_limit_level_out_data     = 0x52,
    en_vdb_mw_sensor_nuc_temperature_1            = 0x53,
    // telemetry
    en_vdb_mw_sensor_tel_status      = 0x70,
    en_vdb_mw_sensor_tel_temperature = 0x71,

    en_vbd_mw_sensor_service_msg        = 0xff
} t_vdb_mw_sensor_registers;


/* AF */
typedef enum {
    en_vdb_af_status = en_vdb_mb_autofocus, // 0x07
    en_vdb_af_enable,                    // 0x08 only w. if 1 - start, if 0 - stop
    en_vdb_af_telemetry_enable,          // 0x09
    en_vdb_af_telemetry_address          // 0x10
} t_vdb_autofocus_registers;

typedef enum {
    en_vdb_mbsm_status = en_vdb_mb_stream_mux, // 0x43
    en_vdb_mbsm_channel_select,           // 0x44 only w. if 1 - start, if 0 - stop
} t_vdb_md_stream_mux_registers;

typedef enum {
    en_vdb_mbfs_enable = en_vdb_mb_field_sync
} t_vdb_md_field_sync_registers;

typedef enum {
    en_vdb_mbmb_tv_fusion_roi = en_vdb_mb_stream_mem_bridge,
} t_vdb_md_memory_bridge_registers;

}

#endif // VDB_REGISTERS_H
