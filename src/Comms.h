#ifndef COMMS_H
#define COMMS_H

#include <OrbiterAPI.h>

class BurnTimeMFD;
class MFDDataBurnTime;
class DataSourceBase;
class Comms
{
    public:
        Comms();
        virtual ~Comms();

        bool ParseInput(char * str, MFDDataBurnTime * data);
        bool HandlerGetFromOtherMFD(BurnTimeMFD * btcMFD, MFDDataBurnTime * data);
        bool SetOtherMFDBurnVars(const DataSourceBase * source, int sourceId, MFDDataBurnTime * data);

    protected:

    private:


};

#endif // COMMS_H
