ГОЭН-220 — пакет поставки (финальный)
=====================================

Состав:
  cli_client/   — UDP-тест-клиент: исходники (с --multicast), собранный x86-64 бинарник,
                  config.ini, build.sh, README
  server_bin/   — бинарник демона qgst-goen220 (aarch64) + config.ini.example
  server_src/   — исходники сервера изменённых модулей (для накатки patched-версии):
                  app/{settingsmanager,network,gst_wrapper,cmd_parser,telemetry,main_app,ts_ring_buffer}
                  app/ev9500m/{camera_ev9500m,...} + visca/libvisca*
                  (полный проект — в Software/src/app репозитория)

УДОСТОВЕРИТЕЛЬНЫЕ ТЕСТЫ (проведено на плате 192.168.1.93):
  Режим Network.UDP_Telemetry_Enable:
    false     -> UDP-пакетов НЕТ                       (проверено, 0 пакетов)
    true      -> unicast на IP_Telemetry, ~25 Гц        (проверено, +108/4с)
    multicast -> группа 239.1.1.1, ~25 Гц               (проверено, +107/4с)
  Приём UDP-команд на 52516: [CMD] ... size: 516        (проверено)
  SRT-видео: caller -> 192.168.1.74:5016, latency=200,
             payload=1316, fec=0                         (проверено)

НАСТРОЙКА СЕРВЕРА (config.ini):
  Network.UDP_Telemetry_Enable = false | true | multicast
  Network.IP_Telemetry         = <unicast IP или multicast-группа 224.0.0.0/4>
  Network.UDP_Port_Edge2       = 52515   ; порт ОТПРАВКИ телеметрии
  Network.UDP_Port_HostPC      = 52516   ; порт ПРИЁМА команд (UDP+TCP)

  ВАЖНО: порты исторически именно так (Edge2=52515, HostPC=52516) — не менять местами.
  В мультикаст-режиме сервер ставит IP_MULTICAST_TTL=2 и loopback=1.

ТЕРМОСТАБИЛИЗАЦИЯ ИК-КАМЕРЫ (команда кулера Говин '+'):
  IRCam.IR_Cam_Thermo_Enable = true (по умолчанию, разрешено) | false (запрещено)
  При false команда "включить кулер" от терминала игнорируется:
  лог "[GOWIN] thermostabilization disabled by config ...".
  Проверено на плате: true -> "... enabled", false -> "... disabled".

КЛИЕНТ (x86-64):
  unicast:    ./cli_client/bin/goen-client-udp-cli -p 52515 --json
  multicast:  ./cli_client/bin/goen-client-udp-cli -p 52515 --json --multicast 239.1.1.1
  Лог: HH:MM:SS.mmm {...} — интервал ~40 мс (25 Гц).
  Сборка: cmake -B build && cmake --build build

РАЗВЁРНУТО СЕЙЧАС:
  сервер 192.168.1.93: режим multicast, IP 239.1.1.1
  клиент 192.168.1.80 (omni): --multicast 239.1.1.1, лог /tmp/udp80_mc2.log
  терминал 192.168.1.74: SRT 5016 + UDP 52515 (multicast не получает — не вступил в группу)
