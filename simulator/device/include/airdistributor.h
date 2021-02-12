#ifndef     AIR_DISTRIBUTOR_H
#define     AIR_DISTRIBUTOR_H

#include    "brake-device.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class DEVICE_EXPORT AirDistributor : public BrakeDevice
{
public:

    AirDistributor(QObject *parent = Q_NULLPTR);

    virtual ~AirDistributor();

    void setBrakepipePressure(double pTM);

    void setBrakeCylinderPressure(double pBC);

    void setAirSupplyPressure(double pAS);

    virtual double getBrakeCylinderAirFlow() const;

    virtual double getAirSupplyFlow() const;

    double getAuxRate() const;

protected:

    double  pTM;

    double  pBC;

    double  pAS;

    double  Qbc;

    double  Qas;

    double  auxRate;
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
typedef AirDistributor* (*GetAirDistributor)();

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
#define GET_AIR_DISTRIBUTOR(ClassName) \
    extern "C" Q_DECL_EXPORT AirDistributor *getAirDistributor() \
    { \
        return new (ClassName) (); \
    }

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
extern "C" Q_DECL_EXPORT AirDistributor *loadAirDistributor(QString lib_path);

#endif // AIR_DISTRIBUTOR_H
