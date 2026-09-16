#include <QScopedValueRollback>
#include <qdebug.h>
#include <unistd.h>
#include "ev9500m/camera_ev9500m.h"

#include "cmd_parser.h"
#include "vdb_registers.h"

typedef struct
{
    uint8_t ch_id;
    uint8_t ch_en;
} CH_ENABLE_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t ch_en;
} CONTR_ENHANCEMENT_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t corr;
} __attribute__((packed)) PROCESSING_BRIGHTNESS_PL;

typedef struct
{
    uint8_t ch_id;
    uint16_t corr;
} __attribute__((packed)) PROCESSING_CONTRAST_PL;

typedef struct
{
    uint8_t inv;
} NEGATIVE_MW_PL;

typedef struct
{
    uint8_t ch_id;
} AUTOFOCUS_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t motor_speed;
} MAN_ZOOM_IN_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t motor_speed;
} MAN_ZOOM_OUT_PL;

typedef struct
{
    uint8_t ch_id;
} MAN_ZOOM_STOP_PL;

typedef struct
{
    uint8_t ch_id;
    uint16_t motor_coord;
} __attribute__((packed)) MAN_ZOOM_POSITION_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t motor_speed;
} MAN_FOCUS_FAR_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t motor_speed;
} MAN_FOCUS_NEAR_PL;

typedef struct
{
    uint8_t ch_id;
} MAN_FOCUS_STOP_PL;

typedef struct
{
    uint8_t ch_id;
    uint16_t motor_coord;
}  __attribute__((packed)) MAN_FOCUS_POSITION_PL ;

typedef struct
{
    uint8_t ch_id;
    uint8_t motor_speed;
} MAN_IRIS_OPEN_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t motor_speed;
} MAN_IRIS_CLOSE_PL;


typedef struct
{
    uint8_t ch_id;
} MAN_IRIS_STOP_PL;

typedef struct
{
    uint8_t ch_id;
} FIELDS_SYNC_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t sync_dis;
} REGULAR_FSYNC_PL;

//SYNC_NARROW_FEILDS_PL

enum class  SWIR_GAIN : uint8_t{
    SWIR_GAIN_LOW = 0x00,
    SWIR_GAIN_HIGHT =0x01,
    SWIR_GAIN_AUTO =0x02
};

typedef struct
{
    uint8_t ch_id;
    SWIR_GAIN sync_dis;
}  __attribute__((packed)) SWIR_GAIN_CONTROL_PL;

enum class  LFR_RADIATION_WM : uint16_t{
    LFR_RAD_LFR_POWER_ON_RESET = 0x0000,
    LFR_RAD_ONE_SHOT =0xAA55,
    LFR_RAD_CONTINOUS =0xB5AD,
    LFR_RAD_SHOT_ONLY =0x9EED
};

typedef struct
{
    LFR_RADIATION_WM work_mode;
}  __attribute__((packed)) LFR_RADIATION_PL;


enum class  LFR_POWER_ON_WM : uint8_t{
    LFR_POWER_OFF = 0x00,
    LFR_EN =0x01,
};

typedef struct
{
    LFR_POWER_ON_WM work_mode;
}   __attribute__((packed)) LFR_POWER_ON_PL;

typedef struct
{
    uint8_t shot_freq;
} LFR_SET_FREQ_PL;


enum class  ARR_SET_EN : uint8_t{
    ARR_SET_OFF = 0x00,
    ARR_SET_ON = 0x01,
};

typedef struct
{
    ARR_SET_EN arr_mod;
} ARR_SET_PL;

enum class  STAB_ENUM : uint8_t{
    STAB_OFF = 0x00,
    STAB_ON  = 0x01,
};


typedef struct
{
    STAB_ENUM en;
} STAB_EN_PL;

//typedef struct PARK_PL

enum class  SET_DEF_POS_EN : uint8_t{
    DEF_POS_ZERO    = 0xA1,
    DEF_POS_POHOD   = 0xA2,
    DEF_POS_PILOT   = 0xA3,
    DEF_POS_PARK    = 0xA4,
    DEF_POS_ARR_OFF = 0xA5,
};

typedef struct
{
    SET_DEF_POS_EN pos;
} SET_DEF_POS_PL;



typedef struct
{
    int32_t Wy:24;
    int32_t Wz:24;
}
__attribute__((packed))  SET_SPEED_PL;


enum class  CHANGE_MODE_ENUM : uint8_t{
    CM_BY_SPEED    = 0x00,
    CM_BY_ANGLE    = 0x01,
};

typedef struct
{
    CHANGE_MODE_ENUM cm;
} CHANGE_MODE_PL;

typedef struct
{
    int16_t dz;
    int16_t dy;
} SET_RELATIVE_ANGLE_PL;

//typedef struct ARR_REMOVE_PL

typedef struct
{
    int16_t dz;
    int16_t dy;
} SET_ABSOLUTE_ANGLE_PL;

typedef struct
{
    uint16_t ch_id;
    uint16_t x_center;
    uint16_t y_center;
    uint16_t width;
    uint16_t height;
}    TRACKING_EN_SINGLE_PL;

//typedef struct TRACKING_DIS_SINGLE_PL

typedef struct
{
    uint8_t chs_id;
} TRACKING_CHS_PL;

typedef struct
{
    uint8_t ch_id;
    uint8_t chs_id;
    uint8_t simple_alg_en;
    uint16_t x_center;
    uint16_t y_center;
    uint16_t width;
    uint16_t height;
}   __attribute__((packed))  TRACKING_EN_4_PL;


typedef struct
{
    uint8_t chs_id;
} TRACKING_DIS_PL;

typedef struct
{
    uint16_t code;
} ALIGNMENT_PL;

typedef struct
{
    uint16_t code;
    uint8_t en_flag;
} __attribute__((packed))  AUTO_INSPECTION_PL;


typedef struct
{
    float   z;
    float   x;
}
__attribute__((packed)) set_speed_deg;


typedef struct
{
    float   z;
    float   x;
}
__attribute__((packed)) set_absolute_angle_deg;


enum CMD_CODE{
    CH_ENABLE = 0x00,             //Разрешить передачу выходного видеопотока канала.
    CONTR_ENHANCEMENT = 0x01,     //Разрешить обработку видеопотока канала для повышения контраста изображения
    PROCESSING_BRIGHTNESS = 0x4A, // Коррекция яркости видеопотока. Диапазон коррекции: (-128...127)
    PROCESSING_CONTRAST = 0x4B,   //Коррекция контрастности изображения видеопотока. Диапазон коррекции: (0…2), значение 32768 соответствует 1.
    NEGATIVE_MW = 0x4C,           //Инверсия видеопотока ТПВ-С канала. 0х00 – неактивно (горячее - яркое), 0х01 – активно (горячее - тёмное).
    AUTOFOCUS = 0x02,             //Разрешить однократный цикл автофокусировки по видео в канале
    MAN_ZOOM_IN = 0x03,           //Начать уменьшение углового поля с заданной скоростью.   Диапазон: (0x00…0xff) ед.
    MAN_ZOOM_OUT = 0x04,          //Начать увеличение углового поля с заданной скоростью.   Диапазон: (0x00…0xff) ед
    MAN_ZOOM_STOP = 0x05,         //Остановить изменение углового поля
    MAN_ZOOM_POSITION = 0x1A,     //Позиционирование углового поля по значению координаты привода.
    MAN_FOCUS_FAR = 0x06,         //Начать движение привода резкости от фокальной плоскости с заданной скоростью.  Диапазон: (0x00…0xff) ед.
    MAN_FOCUS_NEAR = 0x07,        //Начать движение привода резкости к фокальной плоскости с заданной скоростью.   Диапазон: (0x00…0xff) ед.
    MAN_FOCUS_STOP = 0x08,        //Остановить движение привода резкости.
    MAN_FOCUS_POSITION = 0x1B,    //Позиционирование фокуса по значению координаты привода.
    MAN_IRIS_OPEN = 0x09,         //Начать открывание диафрагмы с заданной скоростью.  Диапазон: (0x00…0xff) ед.
    MAN_IRIS_CLOSE = 0x0A,        //Начать закрывание диафрагмы с заданной скоростью.  Диапазон: (0x00…0xff) ед.
    MAN_IRIS_STOP = 0x0B,         //Остановить движение привода диафрагмы.
    FIELDS_SYNC = 0x0C,           //Произвести цикл синхронизации с выбранным опорным каналом угловых полей всех других каналов.
    REGULAR_FSYNC = 0x0D,         //Разрешить регулярную синхронизацию угловых полей каналов по наблюдаемому (опорному) каналу.   0x00 – прекратить ≠0x00 – разрешить
    SYNC_NARROW_FEILDS = 0x0E,    //Произвести сведение каналов ТВ и ТПВ-С в узкое угловое поле
    SWIR_GAIN_CONTROL = 0x40,     //Изменить режим работы усиления канала ТПВ-Б  0х00 – режим низкого усиления 0х01 – режим высокого усиления 0х02 – автоматическое переключение режимов усиления
    LFR_RADIATION = 0x11,         //0х0000 – прерывание текущей задачи и блокировка излучения (сброс LFR_POWER_ON)    0хAA55 – однократный замер ЛД     0хB5AD – режим непрерывного         ЛЦУ с заданной частотой («циклическая» работа ЛД)     0х9EED – «вспышка» ЛЦУ без ЛД
    LFR_POWER_ON = 0x12,          //=0x01 – Разрешить использование лазерного излучения    =0x00 – Отключить силовые цепи ЛЦУ (равнозначно команде LFR_RADIATION со значением                                         0х0000)
    LFR_SET_FREQ = 0x13,          //Код частоты ЛЦУ.   Диапазон частот: (1…30), шаг 1 Гц
    ARR_SET = 0x20,               //Управление механическим арретированием.    0x00 – снять механическое арретирование. После снятия арретирование, устройство переходит в положение «0». (Команда используется в начале работы с устройством)     0x01 – установить механическое арретирование. Установка арретирование производится только, если устройство находится в положение «Парковка»
    STAB_EN  = 0x21,              //Управление режимом работы системы стабилизации гироплатформы.    0x00 – отключить стабилизацию и управление положением (включить электрическое арретирование); 0x01 – включить стабилизацию и управление положением.
    PARK = 0x22,                  //Парковка – выполнить ориентацию платформы в положение защиты входных окон.
    SET_DEF_POS = 0x23,           //Произвести ориентацию платформы в заданные угловые положения: 0xA1(0x00) – положение «0», 0xA2(0x01) – положение «Поход», 0xA4 – положение «Парковка» 0xA5 – аналогично 	команде    ARR_SET со значением =0.
    SET_SPEED = 0x24,             //Начать движение платформы с заданной угловой скоростью.    Скорость – знаковое целое число в диапазоне (-90000…+ 90000) ед. (до ±72,4 º/с)
    CHANGE_MODE = 0x26,           //Изменить режим управления гироплатформой.    0х00 – управление «по скорости»     0х01 – управление «по углу»
    SET_RELATIVE_ANGLE = 0x27,    //Произвести разворот гироплатформы на углы z, Y относительно текущего положения.    Углы – знаковое целое число в диапазоне (-179…+ 179) º
    ARR_REMOVE = 0x28,            //Снять механическое арретирование
    SET_ABSOLUTE_ANGLE = 0x29,    //Произвести разворот гироплатформы на абсолютные углы z, Y. Углы – знаковое целое число в диапазоне (-179…+ 179) º
    TRACKING_EN_SINGLE = 0x30,    //Режим «по одиночной цели». Начать захват и сопровождение цели, выделенной заданной рамкой с центром, координаты которого отсчитываются от верхнего левого угла кадра.    Размеры и координаты – целые числа в диапазоне (0...1920), пиксель.
    TRACKING_DIS_SINGLE = 0x31,   //Режим «по одиночной цели».    Отключить режим захвата и сопровождения
    TRACKING_CHS = 0x32,          //Режим «по групповой цели».    Выбор номера цели для сопровождения. Если цель с выбранным номером была раньше захвачена, то изделие переходит в режим сопровождение цели с номером id.
    TRACKING_EN_4 = 0x33,         //Режим «по групповой цели».    Начать захват цели с номером id, выделенной заданной рамкой с центром в (X, Y), координаты которого отсчитываются от верхнего левого угла кадра активного спектрального канала.      Размеры и координаты – целые числа в диапазоне от 0 до width по горизонтали и от 0 до height по вертикали. Где width и height ширина и высота кадра активного спектрального канала в пикселях.     Если в момент захвата сопровождение цели не ведётся, то сопровождаться начинает захваченная цель.  (Модуль захвата и сопровождения одновременно может работать только с 1 каналом).
    TRACKING_DIS = 0x34,          //Режим «по групповой цели».    Сброс захвата цели с номером id. Если цели с данным номером сопровождается, то режим сопровождения отключается. Статус других, ранее захваченных целей, не изменяется.
    ALIGNMENT = 0xA0,             //Команда юстировки - привязка положения «0/0» ЛВ    (текущие угловые координаты ЛВ запоминаются как новый ноль отсчета).     Код 	подтверждения 	команды 	– 0xD8A4
    AUTO_INSPECTION = 0xA1,       //Произвести самоконтроль (включая привода платформы и объективов) Код подтверждения команды – 0xBBE3    Если флаг активации:          =0x00 – остановка самоконтроля =0x01 – запуск самоконтроля.

    SET_ABSOLUTE_ANGLE_DEG = 0xA2,//    Произвести разворот гироплатформы на абсолютные углы Z, Y. Углы – знаковое целое число в диапазоне (-179 ... + 179)º;
    SET_SPEED_DEG = 0xA7          //    Начать движение платформы с заданной угловой скоростью.    Скорость – знаковое целое число в диапазоне до (-60 ... +60) град./с.
};


CMD_Parser::CMD_Parser() {
    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
        qDebug() << Q_FUNC_INFO << "Network UDP CMD parser thread id: " << QThread::currentThreadId();
    });
    QObject::connect(&thread, &QThread::finished, this, &CMD_Parser::proc_Thread_Finished);

    thread.start();

}
CMD_Parser::~CMD_Parser()
{
    qDebug() << Q_FUNC_INFO;

    thread.exit();
    thread.wait();
}
void CMD_Parser::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;

}


qint32 CMD_Parser::proc_checkCmd(QByteArray ba)
{
    // qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId() << ba.size() << ba.toHex( ' ');
    cm = NULL;
    msg = NULL;
    if(ba.size() == 516 )
    {
        cm = (ucp*)ba.data();

    }
    else
        return -1;

    if ((cm->pkt_preambule == 0) && (cm->service_pkt_flag  == 0x0100) )
    {
        msg = &cm->service_msg;
    }
    else
        return -2;

    const uint8_t dst_lo = (uint8_t)msg->DST;
    const uint8_t dst_hi = (uint8_t)(msg->DST >> 8);
    if( (msg->CMD == 0) && (msg->SRC == 0x0000) &&
        (dst_hi == vdb_registers::en_vbd_GSP && dst_lo == vdb_registers::en_vdb_gsp_motor))
    {
        //  TODO:   По непонятным причинам команда выключающая двигатели
        //          имеет другой DST:
        return proc_TURN_OFF_MOTORS();
    }


    if ((msg->CMD == 0) && (msg->SRC == 0x0000) && (msg->DST == 0x0103)) // && (msg->DST != 0x0000)
    {
        return processCmd();
    }
    else
    {
        if ((msg->CMD == 0) && (msg->SRC == 0x0000) && (msg->DST == 0x090C)) // && (msg->DST != 0x0000)
        {
            return processCmdGowinMini640();
        }
        else
        {
            msg = NULL;
            return -3;
        }
    }

}

qint32 CMD_Parser::processCmdGowinMini640()
{
    qint32 ret = -1;
    qDebug() <<Q_FUNC_INFO<< "Cooler state:"<< ((msg->CODE)?"ON":"OFF") ;
    switch(msg->CODE)
    {
     case 0:
         emit sig_MINI640_CMD(MINI640_COOLER_OFF,0);
         break;

     case 1:
         emit sig_MINI640_CMD(MINI640_COOLER_ON,0);
         break;

    }
     return 1;
}

qint32 CMD_Parser::processCmd()
{
    qint32 ret = -1;
    switch(msg->CODE)
    {
    case CH_ENABLE:
        ret = proc_CH_ENABLE();
        break;
    case CONTR_ENHANCEMENT:
        ret = proc_CONTR_ENHANCEMENT();
        break;
    case PROCESSING_BRIGHTNESS:
        ret = proc_PROCESSING_BRIGHTNESS();
        break;
    case PROCESSING_CONTRAST:
        ret = proc_PROCESSING_CONTRAST();
        break;
    case NEGATIVE_MW:
        ret = proc_NEGATIVE_MW();
        break;
    case AUTOFOCUS:
        ret = proc_AUTOFOCUS();
        break;
    case MAN_ZOOM_IN:
        ret = proc_MAN_ZOOM_IN();
        break;
    case MAN_ZOOM_OUT:
        ret = proc_MAN_ZOOM_OUT();
        break;
    case MAN_ZOOM_STOP:
        ret = proc_MAN_ZOOM_STOP();
        break;
    case MAN_ZOOM_POSITION:
        ret = proc_MAN_ZOOM_POSITION();
        break;
    case MAN_FOCUS_FAR:
        ret = proc_MAN_FOCUS_FAR();
        break;
    case MAN_FOCUS_NEAR:
        ret = proc_MAN_FOCUS_NEAR();
        break;
    case MAN_FOCUS_STOP:
        ret = proc_MAN_FOCUS_STOP();
        break;
    case MAN_FOCUS_POSITION:
        ret = proc_MAN_FOCUS_POSITION();
        break;
    case MAN_IRIS_OPEN:
        ret = proc_MAN_IRIS_OPEN();
        break;
    case MAN_IRIS_CLOSE:
        ret = proc_MAN_IRIS_CLOSE();
        break;
    case MAN_IRIS_STOP:
        ret = proc_MAN_IRIS_STOP();
        break;
    case FIELDS_SYNC:
        ret = proc_FIELDS_SYNC();
        break;
    case REGULAR_FSYNC:
        ret = proc_REGULAR_FSYNC();
        break;
    case SYNC_NARROW_FEILDS:
        ret = proc_SYNC_NARROW_FEILDS();
        break;
    case SWIR_GAIN_CONTROL:
        ret = proc_SWIR_GAIN_CONTROL();
        break;
    case LFR_RADIATION:
        ret = proc_LFR_RADIATION();
        break;
    case LFR_POWER_ON:
        ret = proc_LFR_POWER_ON();
        break;
    case LFR_SET_FREQ:
        ret = proc_LFR_SET_FREQ();
        break;
    case ARR_SET:
        ret = proc_ARR_SET();
        break;
    case STAB_EN:
        ret = proc_STAB_EN();
        break;
    case PARK:
        ret = proc_PARK();
        break;
    case SET_DEF_POS:
        ret = proc_SET_DEF_POS();
        break;
    case SET_SPEED:
        ret = proc_SET_SPEED();
        // ret = proc_SET_SPEED_DEG();
        break;
    case CHANGE_MODE:
        ret = proc_CHANGE_MODE();
        break;
    case SET_RELATIVE_ANGLE:
        ret = proc_SET_RELATIVE_ANGLE();
        break;
    case ARR_REMOVE:
        ret = proc_ARR_REMOVE();
        break;
    case SET_ABSOLUTE_ANGLE:
        ret = proc_SET_ABSOLUTE_ANGLE();
        break;
    case TRACKING_EN_SINGLE:
        ret = proc_TRACKING_EN_SINGLE();
        break;
    case TRACKING_DIS_SINGLE:
        ret = proc_TRACKING_DIS_SINGLE();
        break;
    case TRACKING_CHS:
        ret = proc_TRACKING_CHS();
        break;
    case TRACKING_EN_4:
        ret = proc_TRACKING_EN_4();
        break;
    case TRACKING_DIS:
        ret = proc_TRACKING_DIS();
        break;
    case ALIGNMENT:
        ret = proc_ALIGNMENT();
        break;
    case AUTO_INSPECTION:
        ret = proc_AUTO_INSPECTION();
        break;

    case SET_SPEED_DEG:
        ret = proc_SET_SPEED_DEG();
        break;

    case SET_ABSOLUTE_ANGLE_DEG:
        ret = proc_SET_ABSOLUTE_ANGLE_DEG();
        break;

    default:
        ret = -10; //Processing func not found
        qDebug() <<Q_FUNC_INFO<< "ERROR CMD CODE DECODE"<< msg->CODE;
        break;
    }


    return ret;
}
qint32 CMD_Parser::proc_CH_ENABLE()
{
    qDebug() <<Q_FUNC_INFO;
    CH_ENABLE_PL* ch_en;
    ch_en =(CH_ENABLE_PL*) &msg->PAYLOAD;
    qDebug() <<"Channel enable. Ch: "<< ch_en->ch_id<<". En:" << ch_en->ch_en;
    emit sig_Switch_Channel( ch_en->ch_id ,  ch_en->ch_en);
    return 0;

}

qint32 CMD_Parser::proc_CONTR_ENHANCEMENT()
{
    qDebug() <<Q_FUNC_INFO;

    CONTR_ENHANCEMENT_PL* ctrl_enh;
    ctrl_enh =(CONTR_ENHANCEMENT_PL*) &msg->PAYLOAD;
    qDebug() <<"CONTR_ENHANCEMENT. Ch: "<< ctrl_enh->ch_id<<". En:" << ctrl_enh->ch_en;
    return 0;

}

qint32 CMD_Parser::proc_PROCESSING_BRIGHTNESS()
{
    qDebug() <<Q_FUNC_INFO;

    PROCESSING_BRIGHTNESS_PL* br_proc;
    br_proc =(PROCESSING_BRIGHTNESS_PL*) &msg->PAYLOAD;
    qDebug() <<"PROCESSING_BRIGHTNESS. Ch: "<< br_proc->ch_id<<". Corr:" << br_proc->corr;
    //ics.brightness = br_proc->corr;
   // ics.contrast = br_proc->corr>0?br_proc->corr:0;

  //  emit  sig_ISP_PAR(ics);
    return 0;

}

qint32 CMD_Parser::proc_PROCESSING_CONTRAST()
{
    qDebug() <<Q_FUNC_INFO;

    PROCESSING_CONTRAST_PL* contr_proc;
    contr_proc =(PROCESSING_CONTRAST_PL*) &msg->PAYLOAD;
    uint32_t contrast_val = contr_proc->corr/ 0xFF;
    if(contrast_val > 0xFF)  contrast_val = 0xFF;
    qDebug() <<"PROCESSING CONTRAST. Ch: "<< contr_proc->ch_id<<". Corr:" << contrast_val;
   // ics.contrast = contr_proc->corr;
   // emit  sig_ISP_PAR(ics);
    emit sig_VISCA_CMD(TVCamera::ContrastValue, contrast_val );

    return 0;

}

qint32 CMD_Parser::proc_NEGATIVE_MW()
{
    qDebug() <<Q_FUNC_INFO;
    NEGATIVE_MW_PL* neg_mw;
    neg_mw =(NEGATIVE_MW_PL*) &msg->PAYLOAD;
    qDebug() <<"NEGATIVE MW. Inv: "<< neg_mw->inv;

    return 0;

}


qint32 CMD_Parser::proc_AUTOFOCUS ()
{
    qDebug() <<Q_FUNC_INFO;
    AUTOFOCUS_PL* af;
    af =(AUTOFOCUS_PL*) &msg->PAYLOAD;
    //cam.setFocus(Camera::FocusMode,0);
    emit sig_VISCA_CMD(TVCamera::FocusMode,0);

    qDebug() <<"AUTOFOCUS. Ch.ID: "<< af->ch_id;
    return 0;

}

qint32 CMD_Parser::proc_MAN_ZOOM_IN ()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_ZOOM_IN_PL* mz;
    mz =(MAN_ZOOM_IN_PL*) &msg->PAYLOAD;
   // cam.setZoom(Camera::ZoomWide,7);
    if((mz->ch_id == 0) || (mz->ch_id == 0x80))
        emit sig_VISCA_CMD(TVCamera::ZoomWide,7);
    else
        if ((mz->ch_id == 3)|| (mz->ch_id == 0x81))
    {
             emit sig_J200_CMD(J200_SET_SPEED, 127,127);
            emit sig_J200_CMD(J200_ZOOM_OUT_CONT);
    }
    qDebug() <<"MAN ZOOM IN. Ch.ID: "<< mz->ch_id<<"Motor speed:"<< mz->motor_speed;
    return 0;

}

qint32 CMD_Parser::proc_MAN_ZOOM_OUT()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_ZOOM_OUT_PL* mz;
    mz =(MAN_ZOOM_OUT_PL*) &msg->PAYLOAD;

    //cam.setZoom(Camera::ZoomTele,7);
     if((mz->ch_id == 0) || (mz->ch_id == 0x80))
        emit sig_VISCA_CMD(TVCamera::ZoomTele,7);
    else
         if ((mz->ch_id == 3) || (mz->ch_id == 0x81))
    {
        emit sig_J200_CMD(J200_SET_SPEED, 127,127);
        emit sig_J200_CMD(J200_ZOOM_IN_CONT);
    }
    qDebug() <<"MAN ZOOM OUT. Ch.ID: "<< mz->ch_id<<"Motor speed:"<< mz->motor_speed;
    return 0;

}

qint32 CMD_Parser::proc_MAN_ZOOM_STOP ()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_ZOOM_STOP_PL* mz;
    mz =(MAN_ZOOM_STOP_PL*) &msg->PAYLOAD;
    //cam.setZoom(Camera::ZoomStop,0);

    if((mz->ch_id == 0) || (mz->ch_id == 0x80))
        emit sig_VISCA_CMD(TVCamera::ZoomStop,7);
    else
        if ((mz->ch_id == 3)|| (mz->ch_id == 0x81))
                emit sig_J200_CMD(J200_ZOOM_STOP);


    qDebug() <<"MAN ZOOM STOP. Ch.ID: "<< mz->ch_id;
    return 0;

}

qint32 CMD_Parser::proc_MAN_ZOOM_POSITION ()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_ZOOM_POSITION_PL* pz;
    pz =(MAN_ZOOM_POSITION_PL*) &msg->PAYLOAD;
    if((pz->ch_id == 0) || (pz->ch_id == 0x80))
        emit sig_VISCA_CMD(TVCamera::ZoomPos,pz->motor_coord);
    else
     if ((pz->ch_id == 3)|| (pz->ch_id == 0x81))
             emit sig_J200_CMD(J200_GOTO_POS,pz->motor_coord);

    qDebug() <<"MAN ZOOM POSITION. Ch.ID: "<< pz->ch_id<<"Motor coord:"<< pz->motor_coord;
    return 0;

}

qint32 CMD_Parser::proc_MAN_FOCUS_FAR ()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_FOCUS_FAR_PL* mf;
    mf =(MAN_FOCUS_FAR_PL*) &msg->PAYLOAD;
    //cam.setFocus(Camera::FocusFar,7);

    if((mf->ch_id == 0) || (mf->ch_id == 0x80))
        emit sig_VISCA_CMD(TVCamera::FocusFar,1);
    else
        if ((mf->ch_id == 3)|| (mf->ch_id == 0x81))
    {
        emit sig_J200_CMD(J200_SET_SPEED, 2,2);
        emit sig_J200_CMD(J200_FOCUS_FAR_CONT);
    }

    qDebug() <<"MAN FOCUS FAR. Ch.ID: "<< mf->ch_id<<"Motor speed:"<< mf->motor_speed;
    return 0;

}

qint32 CMD_Parser::proc_MAN_FOCUS_NEAR()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_FOCUS_NEAR_PL* mf;
    mf =(MAN_FOCUS_NEAR_PL*) &msg->PAYLOAD;
    //cam.setFocus(Camera::FocusNear,7);

    if((mf->ch_id == 0) || (mf->ch_id == 0x80))
        emit sig_VISCA_CMD(TVCamera::FocusNear,1);
    else
        if ((mf->ch_id == 3)|| (mf->ch_id == 0x81))
    {
        emit sig_J200_CMD(J200_SET_SPEED, 2,2);
        emit sig_J200_CMD(J200_FOCUS_NEAR_CONT);
    }
    qDebug() <<"MAN FOCUS NEAR. Ch.ID: "<< mf->ch_id<<"Motor speed:"<< mf->motor_speed;
    return 0;

}

qint32 CMD_Parser::proc_MAN_FOCUS_STOP()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_FOCUS_STOP_PL* mf;
    mf =(MAN_FOCUS_STOP_PL*) &msg->PAYLOAD;


    if((mf->ch_id == 0) || (mf->ch_id == 0x80))
         emit sig_VISCA_CMD(TVCamera::FocusStop,0);
    else
        if ((mf->ch_id == 3)|| (mf->ch_id == 0x81))
            emit sig_J200_CMD(J200_FOCUS_STOP);

    qDebug() <<"MAN FOCUS STOP. Ch.ID: "<< mf->ch_id;
    return 0;

}

qint32 CMD_Parser::proc_MAN_FOCUS_POSITION ()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_FOCUS_POSITION_PL* mf;
    mf =(MAN_FOCUS_POSITION_PL*) &msg->PAYLOAD;

    if((mf->ch_id == 0) || (mf->ch_id == 0x80))
        emit sig_VISCA_CMD(TVCamera::FocusPos,mf->motor_coord);
    else
        if ((mf->ch_id == 3)|| (mf->ch_id == 0x81))
    {
        double gpol = mf->motor_coord/10;
            //emit sig_J200_CMD(J200_FOCUS_STOP);
        if(!((gpol<500) || (gpol>1500)))
             emit sig_MINI640_CMD(MINI640_SET_GPOL_BIAS,0,gpol);
    }

    qDebug() <<"MAN FOCUS POSITION. Ch.ID: "<< mf->ch_id<<"Motor coord:"<< mf->motor_coord;
    return 0;

}
int defog=0;

qint32 CMD_Parser::proc_MAN_IRIS_OPEN()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_IRIS_OPEN_PL* mio;
    mio =(MAN_IRIS_OPEN_PL*) &msg->PAYLOAD;
    emit sig_VISCA_CMD(TVCamera::IrisOpen,1);

    if(defog<5) defog++;
    emit  sig_VISCA_CMD(TVCamera::SetNR,defog);


    qDebug() <<"MAN IRIS OPEN. Ch.ID: "<< mio->ch_id<<"Motor speed:"<< mio->motor_speed;
    return 0;

}

qint32 CMD_Parser::proc_MAN_IRIS_CLOSE()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_IRIS_CLOSE_PL* mic;
    mic =(MAN_IRIS_CLOSE_PL*) &msg->PAYLOAD;
    emit sig_VISCA_CMD(TVCamera::IrisClose,1);
    if(defog>0) defog--;
    if(defog == 0)
        emit  sig_VISCA_CMD(TVCamera::SetNR,0);
    else
        emit  sig_VISCA_CMD(TVCamera::SetNR,defog);

    qDebug() <<"MAN IRIS CLOSE. Ch.ID: "<< mic->ch_id<<"Motor speed:"<< mic->motor_speed;
    return 0;

}

qint32 CMD_Parser::proc_MAN_IRIS_STOP ()
{
    qDebug() <<Q_FUNC_INFO;
    MAN_IRIS_STOP_PL* mis;
    mis =(MAN_IRIS_STOP_PL*) &msg->PAYLOAD;
    qDebug() <<"MAN IRIS STOP. Ch.ID: "<< mis->ch_id;
    return 0;

}

qint32 CMD_Parser::proc_FIELDS_SYNC ()
{
    qDebug() <<Q_FUNC_INFO;
    FIELDS_SYNC_PL* f_sync;
    f_sync =(FIELDS_SYNC_PL*) &msg->PAYLOAD;
    qDebug() <<"FIELDS SYNC. Ch.ID: "<< f_sync->ch_id;
    return 0;

}

qint32 CMD_Parser::proc_REGULAR_FSYNC ()
{
    qDebug() <<Q_FUNC_INFO;
    FIELDS_SYNC_PL* f_sync;
    f_sync =(FIELDS_SYNC_PL*) &msg->PAYLOAD;
    qDebug() <<"FIELDS SYNC. Ch.ID: "<< f_sync->ch_id;
    return 0;

}

qint32 CMD_Parser::proc_SYNC_NARROW_FEILDS()
{
    qDebug() <<Q_FUNC_INFO;
    qDebug() <<"SYNC_NARROW_FEILDS.";
    return 0;

}

qint32 CMD_Parser::proc_SWIR_GAIN_CONTROL ()
{
    qDebug() <<Q_FUNC_INFO;
    SWIR_GAIN_CONTROL_PL* sgc;
    sgc =(SWIR_GAIN_CONTROL_PL*) &msg->PAYLOAD;
    qDebug() <<"FIELDS SYNC. Ch.ID: "<< sgc->ch_id<<"Gain control:" << (int)sgc->sync_dis;

    return 0;

}

qint32 CMD_Parser::proc_LFR_RADIATION()
{
    qDebug() <<Q_FUNC_INFO;
    LFR_RADIATION_PL* lfr;
    lfr =(LFR_RADIATION_PL*) &msg->PAYLOAD;
    QString s_wm="";

    switch(lfr->work_mode)
    {
    case   LFR_RADIATION_WM::LFR_RAD_LFR_POWER_ON_RESET:
        s_wm="LFR_RAD_LFR_POWER_ON_RESET";
        emit sig_LDC_CMD(LDC_SET_EMIT_STATE, 0);
        break;
    case   LFR_RADIATION_WM::LFR_RAD_ONE_SHOT:
        s_wm="LFR_RAD_ONE_SHOT";
        emit sig_LDC_CMD(LDC_SET_EMIT_MODE, 0); //singleShot
        emit sig_LDC_CMD(LDC_SET_EMIT_STATE, 1);
        emit sig_LDC_CMD(LDC_SET_EMIT_STATE, 0);

        break;
    case   LFR_RADIATION_WM::LFR_RAD_CONTINOUS:
        s_wm="LFR_RAD_CONTINOUS";
        emit sig_LDC_CMD(LDC_SET_EMIT_MODE, 1); //continousShot
        emit sig_LDC_CMD(LDC_SET_EMIT_STATE, 1);
        break;
    case  LFR_RADIATION_WM::LFR_RAD_SHOT_ONLY:
        s_wm="LFR_RAD_SHOT_ONLY";
        emit sig_LDC_CMD(LDC_SET_EMIT_MODE, 1); //ShotOnly
        emit sig_LDC_CMD(LDC_SET_EMIT_STATE, 1);
        break;
    others:
        s_wm="Uncrecognized Work Mod";
        break;
    }

    qDebug() <<"LFR RADIATION. Work mode code: "<< (uint)lfr->work_mode<<s_wm;
    return 0;

}

qint32 CMD_Parser::proc_LFR_POWER_ON ()
{
    qDebug() <<Q_FUNC_INFO;
    LFR_POWER_ON_PL* lfr;
    lfr =(LFR_POWER_ON_PL*) &msg->PAYLOAD;
    QString s_wm="";

    switch(lfr->work_mode)
    {
    case   LFR_POWER_ON_WM::LFR_EN:
        s_wm="LFR_EN";
        emit sig_LDC_CMD(LDC_SET_TEMP_REG_STATE, 1);

        break;
    case   LFR_POWER_ON_WM::LFR_POWER_OFF:
        s_wm="LFR_POWER_OFF";
        emit sig_LDC_CMD(LDC_SET_TEMP_REG_STATE, 0);

        break;
    others:
        s_wm="Uncrecognized Work Mod";
        break;
    }

    qDebug() <<"LFR POWER ON. Сode: "<< (uint)lfr->work_mode<<s_wm;
    return 0;

}

qint32 CMD_Parser::proc_LFR_SET_FREQ()
{
    qDebug() <<Q_FUNC_INFO;

    LFR_SET_FREQ_PL* lfr_freq;
    lfr_freq =(LFR_SET_FREQ_PL*) &msg->PAYLOAD;
    qDebug() <<"LFR SET SHOT FREQ: "<< lfr_freq->shot_freq;
    emit sig_LDC_CMD(LDC_SET_EMIT_MFREQ, (uint32_t)lfr_freq->shot_freq);

    return 0;

}

qint32 CMD_Parser::proc_ARR_SET ()
{
    qDebug() <<Q_FUNC_INFO;
    ARR_SET_PL* arr;
    arr =(ARR_SET_PL*) &msg->PAYLOAD;
    QString s_arr="";

    switch(arr->arr_mod)
    {
    case   ARR_SET_EN::ARR_SET_ON:
        s_arr="ARR_SET_ON";
        break;
    case   ARR_SET_EN::ARR_SET_OFF:
        s_arr="LFR_POWER_OFF";
        break;
    others:
        s_arr="Uncrecognized";
        break;
    }

    emit sig_setDrivePositionMode( true, false, false, false);

    qDebug() <<"ARR_SET : "<< s_arr;
    return 0;

}

qint32 CMD_Parser::proc_STAB_EN ()
{
    qDebug() <<Q_FUNC_INFO;
    STAB_EN_PL* stab_en;
    stab_en =(STAB_EN_PL*) &msg->PAYLOAD;
    QString s_arr="";

    switch(stab_en->en)
    {
    case STAB_ENUM::STAB_ON:
        s_arr="STAB_ON";
        emit sig_setStabilization( true);
        break;

    case STAB_ENUM::STAB_OFF:
        s_arr="STAB_OFF";
        emit sig_setStabilization( false);
        break;

    others:
        s_arr="Uncrecognized";
        break;
    }

    qDebug() <<"STAB EN: "<< s_arr;
    return 0;

}

qint32 CMD_Parser::proc_PARK ()
{
    qDebug() <<Q_FUNC_INFO;

    qDebug() <<"PARK CMD";

    return 0;

}

qint32 CMD_Parser::proc_SET_DEF_POS ()
{
    qDebug() <<Q_FUNC_INFO;
    SET_DEF_POS_PL* def;
    def =(SET_DEF_POS_PL*) &msg->PAYLOAD;
    QString s_pos="";

    switch(def->pos)
    {
    case SET_DEF_POS_EN::DEF_POS_ARR_OFF:
        s_pos="DEF_POS_ARR_OFF";
        break;

    case  SET_DEF_POS_EN::DEF_POS_PARK:
        s_pos="DEF_POS_PARK";
        // emit sig_setControlPosition( 0.0f, 90.0f);
        emit sig_setDrivePositionMode( false, false, false, true);
        break;

    case  SET_DEF_POS_EN::DEF_POS_PILOT:
        s_pos="DEF_POS_PILOT";
        // emit sig_setControlPosition( 0.0f, -20.0f);
        emit sig_setDrivePositionMode( false, false, true, false);
        break;

    case  SET_DEF_POS_EN::DEF_POS_POHOD:
        s_pos="DEF_POS_POHOD";
        // emit sig_setControlPosition( -180.0f, 0.0f);
        emit sig_setDrivePositionMode( false, true, false, false);
        break;

    case  SET_DEF_POS_EN::DEF_POS_ZERO:
        s_pos="DEF_POS_ZERO";
        // emit sig_setControlPosition( 0.0f, 0.0f);
        emit sig_setDrivePositionMode( true, false, false, false);
        break;

    others:
        s_pos="Uncrecognized";
        break;
    }
    qDebug() <<"SET DEF POS: "<< s_pos;

    return 0;

}

qint32 CMD_Parser::proc_SET_SPEED()
{
    qDebug() <<Q_FUNC_INFO;
    SET_SPEED_PL* sp;
    sp = (SET_SPEED_PL*) &msg->PAYLOAD;


    qDebug() <<"SET SPEED: Wy:"<< sp->Wy<<"Wz:"<< sp->Wz;

    const float coff    = (2000.0f * (58.1f / 2.0f));
    const float z       = ((float)sp->Wz) / coff;
    const float x       = ((float)sp->Wy) / coff;

    emit sig_setControlSpeed( z, x);

    return 0;

}

qint32 CMD_Parser::proc_CHANGE_MODE()
{
    qDebug() <<Q_FUNC_INFO;
    CHANGE_MODE_PL* cm;
    cm =(CHANGE_MODE_PL*) &msg->PAYLOAD;
    QString s_cm="";

    switch(cm->cm)
    {
    case CHANGE_MODE_ENUM::CM_BY_ANGLE:
        s_cm="CM_BY_ANGLE";
        emit sig_setStabilization( false);
        break;
    case CHANGE_MODE_ENUM::CM_BY_SPEED:
        s_cm="CM_BY_SPEED";
        break;
    others:
        s_cm="Uncrecognized";
        break;
    }
    qDebug() <<"CHANGE_MODE: "<< s_cm;
    return 0;

}

qint32 CMD_Parser::proc_SET_RELATIVE_ANGLE ()
{
    // qDebug() <<Q_FUNC_INFO;

    const SET_RELATIVE_ANGLE_PL *const relativeAngle = (SET_RELATIVE_ANGLE_PL *)&msg->PAYLOAD;
    qDebug() << Q_FUNC_INFO << relativeAngle->dy << relativeAngle->dz;

    emit sig_setControlPositionRel( relativeAngle->dy, relativeAngle->dz);

    // CHANGE_MODE_PL* cm;
    // cm =(CHANGE_MODE_PL*) &msg->PAYLOAD;
    // QString s_cm="";

    // switch(cm->cm)
    // {
    // case CHANGE_MODE_ENUM::CM_BY_ANGLE:
    //     s_cm="CM_BY_ANGLE";
    //     break;
    // case CHANGE_MODE_ENUM::CM_BY_SPEED:
    //     s_cm="CM_BY_SPEED";
    //     break;
    // others:
    //     s_cm="Uncrecognized";
    //     break;
    // }
    // qDebug() <<"CHANGE_MODE: "<< s_cm;
    return 0;

}

qint32 CMD_Parser::proc_ARR_REMOVE ()
{
    qDebug() <<Q_FUNC_INFO;
    qDebug() <<"ARR REMOVE";
    return 0;

}

qint32 CMD_Parser::proc_SET_ABSOLUTE_ANGLE ()
{
    qDebug() <<Q_FUNC_INFO;
    SET_ABSOLUTE_ANGLE_PL* saa;
    saa = (SET_ABSOLUTE_ANGLE_PL*) &msg->PAYLOAD;

    qDebug() <<"SET ABSOLUTE ANGLE: dY:"<< saa->dy<<"dZ:"<< saa->dz;
    return 0;

}

qint32 CMD_Parser::proc_TRACKING_EN_SINGLE ()
{
    qDebug() <<Q_FUNC_INFO;
    TRACKING_EN_SINGLE_PL* track;
    track = (TRACKING_EN_SINGLE_PL*) &msg->PAYLOAD;

    qDebug() <<"TRACKING EN SINGLE: Ch.ID:"<< track->ch_id<<"X:"<< track->x_center \
             <<"Y:"<< track->y_center<<"Width:"<< track->width<<"Height:"<< track->height;
    return 0;

}

qint32 CMD_Parser::proc_TRACKING_DIS_SINGLE ()
{
    qDebug() <<Q_FUNC_INFO;
    qDebug() <<"TRACKING DISABLE SINGLE";
    return 0;

}

qint32 CMD_Parser::proc_TRACKING_CHS ()
{
    qDebug() <<Q_FUNC_INFO;
    TRACKING_CHS_PL* track;
    track = (TRACKING_CHS_PL*) &msg->PAYLOAD;

    qDebug() <<"TRACKING CHS.ID:"<< track->chs_id;
    return 0;

}

qint32 CMD_Parser::proc_TRACKING_EN_4 ()
{
    qDebug() <<Q_FUNC_INFO;
    TRACKING_EN_4_PL* track;
    track = (TRACKING_EN_4_PL*) &msg->PAYLOAD;
    emit sig_TrackingSetTargetObject(track->x_center, track->y_center,
                                     track->width,track->height);


    qDebug() <<"TRACKING EN 4: Ch.ID:"<< track->ch_id<<"CHS.ID:"<<track->chs_id
             <<"Simle alg en:" << track->simple_alg_en
             <<"X:"<< track->x_center
             <<"Y:"<< track->y_center
             <<"Width:"<< track->width
             <<"Height:"<< track->height;
    return 0;

}

qint32 CMD_Parser::proc_TRACKING_DIS ()
{
    qDebug() <<Q_FUNC_INFO;
    TRACKING_DIS_PL* track;
    track = (TRACKING_DIS_PL*) &msg->PAYLOAD;

    emit sig_TrackingSetStopTracking();
    qDebug() <<"TRACKING DIS CHS.ID:"<< track->chs_id;
    return 0;

}

qint32 CMD_Parser::proc_ALIGNMENT ()
{
    qDebug() <<Q_FUNC_INFO;
    ALIGNMENT_PL* align;
    align = (ALIGNMENT_PL*) &msg->PAYLOAD;
    QString s_align="";
    if( align->code == 0xD8A4)
        s_align = "Code 0xD8A4 success";
    else
        s_align = "Code erproc_TURN_OFF_MOTORSror";

    qDebug() <<"ALIGNMENT:"<< s_align;

    return 0;

}

qint32 CMD_Parser::proc_AUTO_INSPECTION ()
{
    qDebug() <<Q_FUNC_INFO;
    AUTO_INSPECTION_PL* inspect;
    inspect = (AUTO_INSPECTION_PL*) &msg->PAYLOAD;
    QString s_inspect="";
    if( inspect->code == 0xBBE3)
        s_inspect = "Code 0xBBE3 success";
    else
        s_inspect = "Code error";

    qDebug() <<"AUTO_INSPECTION:"<< s_inspect << "En:"<<inspect->en_flag;

    return 0;

}


qint32 CMD_Parser::proc_SET_SPEED_DEG()
{
    set_speed_deg *const sp = (set_speed_deg *)&msg->PAYLOAD;

    qDebug() << "SET SPEED DEG: Z:"<< sp->z << "X:" << sp->x;
    emit sig_setControlSpeed( sp->z, sp->x);

    return 0;
}


qint32 CMD_Parser::proc_SET_ABSOLUTE_ANGLE_DEG()
{
    set_absolute_angle_deg* saa;
    saa = (set_absolute_angle_deg*) &msg->PAYLOAD;

    qDebug() << "SET ABSOLUTE ANGLE DEG: X:" << saa->x << "Z:" << saa->z;

    emit sig_setControlPositionAbs( saa->z, saa->x);
    return 0;
}


qint32 CMD_Parser::proc_TURN_OFF_MOTORS()
{
    qDebug() << "TURN OFF MOTORS";

    emit sig_turnOffMotors();

    return 0;
}


