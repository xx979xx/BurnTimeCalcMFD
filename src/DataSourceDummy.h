#ifndef DATASOURCEDUMMY_H
#define DATASOURCEDUMMY_H

#include "DataSourceBase.h"

class DataSourceDummy : public DataSourceBase
{
    public:
        DataSourceDummy();
        virtual ~DataSourceDummy() {}

        const char * GetName() const { return "LaunchMFD"; }
        bool GetFromMM(MFDDataBurnTime * data);
        double GetIBT() const { return m_ibt; }
        double GetDV() const { return m_dv; }
        VECTOR3 GetVelVec() const { return _V(0, 250, 250); }

    protected:

    private:
        double m_ibt;
        double m_dv;
};

#endif // DATASOURCETRANSX_H
