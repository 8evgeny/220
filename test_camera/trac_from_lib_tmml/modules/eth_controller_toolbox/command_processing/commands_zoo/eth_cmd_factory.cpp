#include "eth_cmd_factory.hpp"

#ifdef USE_ETH_CMD_UNCOMPRESS_FRAME_DEFAULT
#include "uncompress_frame/uncomp_frame_eth_cmd.hpp"
#endif /* USE_ETH_CMD_UNCOMPRESS_FRAME_DEFAULT */

#ifdef USE_ETH_CMD_COPTER_ALARM
#include "copter_alarm/copter_alarm_eth_cmd.hpp"
#endif /* USE_ETH_CMD_COPTER_ALARM*/

#ifdef USE_ETH_CMD_TELEMETRY_ATAS
#include "telemetry_atas/telemetry_atas_eth_cmd.hpp"
#endif /* USE_ETH_CMD_TELEMETRY_ATAS */

#ifdef USE_ETH_CMD_POWEROFF_REQUEST
#include "poweroff_request/poweroff_request_eth_cmd.hpp"
#endif /* USE_ETH_CMD_POWEROFF_REQUEST */

#ifdef USE_ETH_CMD_CONFIRM_CMD_RECEIVE
#include "confirmation_cmd_receive/confirmation_cmd_receive.hpp"
#endif /* USE_ETH_CMD_CONFIRM_CMD_RECEIVE */

#ifdef USE_ETH_CMD_AP_TELEMETRY
#include "ap_telemetry/ap_telemetry_eth_cmd.hpp"
#endif /* USE_ETH_CMD_AP_TELEMETRY */

#ifdef USE_ETH_CMD_TRACKER_METADATA
#include "track_meta/track_meta_eth_cmd.hpp"
#endif /* USE_ETH_CMD_TRACKER_METADATA */

#ifdef USE_ETH_CMD_TELEMETRY_ORIENTATION
#include "telemetry_orientation/telemetry_orientation_eth_cmd.hpp"
#endif /* USE_ETH_CMD_TELEMETRY_ORIENTATION */

#ifdef USE_ETH_CMD_AP_POS_CNT
#include "ap_pos_cnt/ap_pos_cnt_eth_cmd.hpp"
#endif /* USE_ETH_CMD_AP_POS_CNT */

#ifdef USE_ETH_CMD_FENIX2META
#include "fenix2meta/fenix2meta_eth_cmd.hpp"
#endif /*USE_ETH_CMD_FENIX2META*/

#ifdef USE_ETH_CMD_FENIX2CMD
#include "fenix2cmd/fenix2cmd_eth_cmd.hpp"
#endif /*USE_ETH_CMD_FENIX2CMD*/

#ifdef USE_ETH_CMD_400_INIT
#include "cmd400/eth_cmd_400_init.hpp"
#endif
#ifdef USE_ETH_CMD_400_DEINIT
#include "cmd400/eth_cmd_400_deinit.hpp"
#endif
#ifdef USE_ETH_CMD_RECORD_ON
#include "cmd400/eth_cmd_400_record_on.hpp"
#endif
#ifdef USE_ETH_CMD_VERTELKA_CLOSE
#include "cmd_vertelka/eth_cmd_vertelka_close.hpp"
#endif

#ifdef USE_ETH_CMD_VERTELKA_RUN10
#include "cmd_vertelka/eth_cmd_vertelka_run_10.hpp"
#endif

#ifdef USE_ETH_CMD_VERTELKA_RUN8
#include "cmd_vertelka/eth_cmd_vertelka_run_8.hpp"
#endif



#include <cassert>
#include <iostream>

namespace eth_cmd_processing
{
EthCmdFactory::EthCmdFactory()
{
#ifdef USE_ETH_CMD_UNCOMPRESS_FRAME_DEFAULT
    std::function< std::shared_ptr<EthCmdBase> () > uncompress_frame_callback = []()
    {
        return std::make_shared<EthCmdUncompFrame>();
    }; // uncompress_frame_callback
    factory_callbacks_map[ETH_CMD_UNCOMPRESS_FRAME_DEFAULT] = uncompress_frame_callback;
#endif /* USE_ETH_CMD_UNCOMPRESS_FRAME_DEFAULT */

#ifdef USE_ETH_CMD_COPTER_ALARM
    std::function< std::shared_ptr<EthCmdBase> () > copter_alarm_callback = []()
    {
        return std::make_shared<EthCmdCopterAlarm>();
    }; // copter_alarm_callback
    factory_callbacks_map[ETH_CMD_COPTER_ALARM] = copter_alarm_callback;
#endif /* USE_ETH_CMD_COPTER_ALARM*/

#ifdef USE_ETH_CMD_TELEMETRY_ATAS
    std::function< std::shared_ptr<EthCmdBase> () > telemetry_atas_callback = []()
    {
        return std::make_shared<EthCmdTelemetryAtas>();
    }; // poweroff request callback
    factory_callbacks_map[ETH_CMD_TELEMETRY_ATAS] = telemetry_atas_callback;
#endif /* USE_ETH_CMD_TELEMETRY_ATAS */

#ifdef USE_ETH_CMD_POWEROFF_REQUEST
    std::function< std::shared_ptr<EthCmdBase> () > poweroff_request_callback = []()
    {
        return std::make_shared<EthCmdRequestPoweroff>();
    }; // poweroff request callback
    factory_callbacks_map[ETH_CMD_POWEROFF_REQUEST] = poweroff_request_callback;
#endif /* USE_ETH_CMD_POWEROFF_REQUEST */

#ifdef USE_ETH_CMD_CONFIRM_CMD_RECEIVE
    std::function< std::shared_ptr<EthCmdConfirmCmdReceive> () > confirm_cmd_receive_callback = []()
    {
        return std::make_shared<EthCmdConfirmCmdReceive>();
    }; // confirm cmd receive callback
    factory_callbacks_map[ETH_CMD_CONFIRM_CMD_RECEIVE] = confirm_cmd_receive_callback;
#endif /* USE_ETH_CMD_CONFIRM_CMD_RECEIVE */

#ifdef USE_ETH_CMD_AP_TELEMETRY
    std::function< std::shared_ptr<EthCmdAPTelemetry> () > ap_telemetry_callback = []()
    {
        return std::make_shared<EthCmdAPTelemetry>();
    }; // ap telemetry callback
    factory_callbacks_map[ETH_CMD_AP_TELEMETRY] = ap_telemetry_callback;
#endif /* USE_ETH_CMD_AP_TELEMETRY */

#ifdef USE_ETH_CMD_TRACKER_METADATA
    std::function< std::shared_ptr<EthCmdTrackerMetaData> () > tracker_metadata_callback = []()
    {
        return std::make_shared<EthCmdTrackerMetaData>();
    }; // tracker metadata callback
    factory_callbacks_map[ETH_CMD_TRACKER_METADATA] = tracker_metadata_callback;
#endif /* USE_ETH_CMD_TRACKER_METADATA */

#ifdef USE_ETH_CMD_TELEMETRY_ORIENTATION
    std::function< std::shared_ptr<EthCmdOrientTelemetry> () > orient_telemetry_callback = []()
    {
        return std::make_shared<EthCmdOrientTelemetry>();
    };
    factory_callbacks_map[ETH_CMD_ORIENTATION_TELEMETRY] = orient_telemetry_callback;
#endif /* USE_ETH_CMD_TELEMETRY_ORIENTATION */

#ifdef USE_ETH_CMD_AP_POS_CNT
    std::function< std::shared_ptr<EthCmdApPosCnt> () > ap_pos_cnt_callback = []()
    {
        return std::make_shared<EthCmdApPosCnt>();
    };
    factory_callbacks_map[ETH_CMD_AP_POS_CNT] = ap_pos_cnt_callback;
#endif /* USE_ETH_CMD_AP_POS_CNT */

#ifdef USE_ETH_CMD_FENIX2META
    std::function< std::shared_ptr<EthCmdFenix2MetaData>() > fenix2_metadata_callback = []()
    {
        return std::make_shared<EthCmdFenix2MetaData>();
    }; // fenix2 metadata callback
    factory_callbacks_map[ETH_CMD_FENIX2_METADATA] = fenix2_metadata_callback;
#endif /*USE_ETH_CMD_FENIX2META*/

#ifdef USE_ETH_CMD_FENIX2CMD
    std::function< std::shared_ptr<EthCmdFenix2CmdData>() > fenix2_cmd_data_callback = []()
    {
        return std::make_shared<EthCmdFenix2CmdData>();
    }; // fenix2 cmddata callback
    factory_callbacks_map[ETH_CMD_FENIX2_CMD_DATA] = fenix2_cmd_data_callback;
#endif /*USE_ETH_CMD_FENIX2CMD*/

#ifdef USE_ETH_CMD_400_INIT
    std::function<std::shared_ptr<EthCmd400_init>()> eth_cmd_400_init_callback = []()
    {
        return std::make_shared<EthCmd400_init>();
    }; // cmd400
    factory_callbacks_map[ETH_CMD_400_INIT] = eth_cmd_400_init_callback;
#endif

#ifdef USE_ETH_CMD_400_DEINIT
    std::function<std::shared_ptr<EthCmd400_deinit>()> eth_cmd_400_deinit_callback = []()
    {
        return std::make_shared<EthCmd400_deinit>();
    }; // cmd400
    factory_callbacks_map[ETH_CMD_400_DEINIT] = eth_cmd_400_deinit_callback;
#endif

#ifdef USE_ETH_CMD_RECORD_ON
    std::function<std::shared_ptr<EthCmdBase>()> eth_cmd_400_record_on_callback = []()
    {
        return std::make_shared<EthCmd400_record_on>();
    }; // cmd400
    factory_callbacks_map[ETH_CMD_400_RECORD_ON] = eth_cmd_400_record_on_callback;
#endif // USE_ETH_CMD_RECOED_ON

#ifdef USE_ETH_CMD_VERTELKA_CLOSE
    std::function<std::shared_ptr<EthCmdVertelka_close>()> eth_cmd_vertelka_close_callback = []()
    {
        return std::make_shared<EthCmdVertelka_close>();
    }; // cmd_vertelka
    factory_callbacks_map[ETH_CMD_VERTELKA_CLOSE] = eth_cmd_vertelka_close_callback;
#endif

#ifdef USE_ETH_CMD_VERTELKA_RUN10
    std::function<std::shared_ptr<EthCmdVertelka_run_10>()> eth_cmd_vertelka_run10_callback = []()
    {
        return std::make_shared<EthCmdVertelka_run_10>();
    }; // cmd_vertelka
    factory_callbacks_map[ETH_CMD_VERTELKA_RUN10] = eth_cmd_vertelka_run10_callback;
#endif

#ifdef USE_ETH_CMD_VERTELKA_RUN8
    std::function<std::shared_ptr<EthCmdVertelka_run_8>()> eth_cmd_vertelka_run8_callback = []()
    {
        return std::make_shared<EthCmdVertelka_run_8>();
    }; // cmd_vertelka
    factory_callbacks_map[ETH_CMD_VERTELKA_RUN8] = eth_cmd_vertelka_run8_callback;
#endif


} // -- END EthCmdFactory

std::shared_ptr<EthCmdBase> EthCmdFactory::create(EthCommands cmd_type) const
{
    assert(cmd_type != ETH_CMD_NO_COMMAND);
    // assert(factory_callbacks_map.find(cmd_type) != factory_callbacks_map.end());
    if(factory_callbacks_map.find(cmd_type) == factory_callbacks_map.end())
    {
        std::cout << "Error factory_callbacks_map.find(cmd_type) != factory_callbacks_map.end(); cmd_type=" + std::to_string(cmd_type);
        return nullptr;
    }

    return factory_callbacks_map.at(cmd_type)(); // can be throw except std::out_of_range if cmd_type not in keys factory_callbacks_map
} // -- END create
} // eth_cmd_processing
