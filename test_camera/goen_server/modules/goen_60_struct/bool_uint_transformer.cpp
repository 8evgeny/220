#include "bool_uint_transformer.hpp"

using namespace std;

void BoolUintTransformer::get_goen_status_field(ToBortTelemetry &from_goen,
                                                uint16_t &zoom_ratio,
                                                StatusInformationFeedback1 &sif1,
                                                StatusInformationFeedback2 &sif2,
                                                StatusInformationFeedback3 &sif3,
                                                SelfInspectionResult &sir
                                                )
{
    // Функция для формирования булевых структур после получения пакета (на стороне emulator)  `!!! reverse
    uint8_t self_test_result = from_goen.self_test_result;
    uint8_t goen_state1 = from_goen.goen_state1;
    uint8_t goen_state2 = from_goen.goen_state2;
    uint8_t goen_state3 = from_goen.goen_state3;
    memcpy(&zoom_ratio, &from_goen.zoom_ratio, sizeof(uint16_t));

    bitset<8> self_test_result_bitset(self_test_result);
    bitset<8> goen_state1_bitset(goen_state1);
    bitset<8> goen_state2_bitset(goen_state2);
    bitset<8> goen_state3_bitset(goen_state3);
    bitset<16> zoom_ratio_bitset(zoom_ratio);

    for(int i = zoom_ratio_bitset.size() - 4; i < zoom_ratio_bitset.size(); i++)
    {
        zoom_ratio_bitset[i] = 0;
    } // END for(int i = zoom_ratio_bitset.size() - 4; i < zoom_ratio_bitset.size(); i++)
    zoom_ratio = (uint16_t)(zoom_ratio_bitset.to_ulong());

    int size_sf = sif1.vec.size();

    for(int i = 0; i < size_sf; ++i)
    {
        int k = size_sf - 1 - i;  // Обратный индекс для bitset
        *sir.vec[i] = (bool)self_test_result_bitset[k];
        *sif1.vec[i] = (bool)goen_state1_bitset[k];
        *sif2.vec[i] = (bool)goen_state2_bitset[k];
        *sif3.vec[i] = (bool)goen_state3_bitset[k];
    } // END for(int i = 0; i < gs1.vec.size(); ++i)
} // -- END void get_goen_status_field

void BoolUintTransformer::set_goen_status_field(ToBortTelemetry& from_goen,
                                                uint16_t &zoom_ratio,
                                                StatusInformationFeedback1 &sif1,
                                                StatusInformationFeedback2 &sif2,
                                                StatusInformationFeedback3 &sif3,
                                                SelfInspectionResult &sir
                                                )
{
    // Функция для формирования поля структуры для передачи пакета (на стороне goen)
    string sif1_str = "", sif2_str = "", sif3_str = "", sir_str = "";

    for(int i = 0; i < sif1.vec.size(); ++i)
    {
        sif1_str += convert_to_char(*sif1.vec[i]);
        sif2_str += convert_to_char(*sif2.vec[i]);
        sif3_str += convert_to_char(*sif3.vec[i]);
        sir_str += convert_to_char(*sir.vec[i]);
    } // END for(int i = 0; i < goen_state1.vec.size(); ++i)

    bitset<8> sif1_bs(sif1_str);
    bitset<8> sif2_bs(sif2_str);
    bitset<8> sir_bs(sir_str);

    memcpy(&from_goen.zoom_ratio, &zoom_ratio, sizeof(uint16_t)); // заполняем zoom_ratio и goen_state3 значением zoom_ratio
    bitset<8> goen_state3_bs(from_goen.goen_state3); // создаём bitset goen_state3 запоненный вторым байтом zoom_ratio
    bitset<8> sif3_bs(sif3_str); // создаём bitset goen_state3 с запоненными битовыми полями
    for(int i = 8; i > 4; i--) // переносим 4 байта, содержащие значение zoom_ratio
    {
        sif3_bs[i]=goen_state3_bs[i];
    } // END for(int i = 8; i > 4; i--)

    from_goen.self_test_result = sir_bs.to_ulong();
    from_goen.goen_state1 = sif1_bs.to_ulong();
    from_goen.goen_state2 = sif2_bs.to_ulong();
    from_goen.goen_state3 = sif3_bs.to_ulong();
}  // -- END set_goen_status_field


char BoolUintTransformer::convert_to_char(bool b)
{
    return b ? '1' : '0';
} // -- END char convert_to_char(bool b)
