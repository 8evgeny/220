#ifndef DBUS_WRAPPER_H
#define DBUS_WRAPPER_H

#include "settingsmanager.h"
#include <QObject>
#include <qdbusinterface.h>
#include <qthread.h>

class DBUS_Wrapper :public QObject
{
    Q_OBJECT
    QThread thread;

public:
    explicit DBUS_Wrapper(SettingsManager *sm = nullptr, QObject *parent = nullptr);
    void dbus_CommInit();


public slots:
    /**
     * @brief Calls the SetBrightness method on the D-Bus interface.
     * @param brightness The brightness value to set (type: double).
     */
    void setBrightness(double brightness);

    /**
     * @brief Calls the GetOutObject method on the D-Bus interface.
     * Retrieves the output object parameters (cx, cy, w, h).
     */
    void getOutObject(uint &cx_out, uint &cy_out, uint &w_out, uint &h_out);

    /**
     * @brief Calls the GetOutRoi method on the D-Bus interface.
     * Retrieves the output ROI parameters (cx, cy, w, h).
     */
    void getOutRoi(uint &cx_out, uint &cy_out, uint &w_out, uint &h_out);

    /**
     * @brief Calls the SetTargetObject method on the D-Bus interface.
     * @param cx The x-coordinate of the target object's center.
     * @param cy The y-coordinate of the target object's center.
     * @param w The width of the target object.
     * @param h The height of the target object.
     */
    void setTargetObject(uint cx, uint cy, uint w, uint h);

    /**
     * @brief Calls the SetStopTr method on the D-Bus interface.
     */
    void setStopTracking();

    /**
     * @brief Calls the GetATg method on the D-Bus interface.
     * Retrieves the active and locked states of the target.
     */
    void getActiveLockTracking(bool &active_out, bool &tracking_out);

    /**
     * @brief Calls the SetRExt method on the D-Bus interface.
     * @param ext The external parameter to set (type: double).
     */
    void setRoiExt(double ext);


private:
    SettingsManager *sm;
    QDBusInterface *  dbusInterface;
    bool isInit = false;



};


#endif // DBUS_WRAPPER_H
