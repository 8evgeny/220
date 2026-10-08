#pragma once
#include "from_goen_struct.hpp"
#include <bitset>
#include <cstring>

class BoolUintTransformer
{
public:
    BoolUintTransformer(){};
    ~BoolUintTransformer() = default;

    void set_goen_status_field(FromGoenTelemetry& from_goen,
                               uint16_t &zoom_ratio,
                               StatusInformationFeedback1 &sif1,
                               StatusInformationFeedback2 &sif2,
                               StatusInformationFeedback3 &sif3,
                               SelfInspectionResult &sir
                               );
    void get_goen_status_field(FromGoenTelemetry &from_goen,
                               uint16_t &zoom_ratio,
                               StatusInformationFeedback1 &sif1,
                               StatusInformationFeedback2 &sif2,
                               StatusInformationFeedback3 &sif3,
                               SelfInspectionResult &sir
                               );

private:
    char convert_to_char(bool b);
};  // END class bool_uint_transformer
