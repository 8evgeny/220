#pragma once

#include "vlx_media/vlx_av.h"
#include "../cmd/telemetry.h"

#include <QObject>

class VlxTelemetryChannel : public QObject
{
    Q_OBJECT

    struct Internal;

    VlxTelemetryChannel();

public:

    virtual ~VlxTelemetryChannel();

    static VlxTelemetryChannel& i();

    bool getNext(Telemetry &tm);

protected slots:

    void init();

    void onInputDemuxerStateChanged();

private:

    std::unique_ptr<VlxTelemetryChannel::Internal> m_i;
};
