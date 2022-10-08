#ifndef DATASOURCELAGRANGE_H
#define DATASOURCELAGRANGE_H

#include "DataSourceBase.h"

class DataSourceLagrange : public DataSourceBase
{
    public:
        DataSourceLagrange();
        virtual ~DataSourceLagrange() {}

        const char * GetName() const { return "LagrangeMFD"; }
        bool GetFromMM(MFDDataBurnTime * data);
        double GetIBT() const { return m_ibt; }
        VECTOR3 GetVelVec() const { return m_vel; }

    protected:

    private:
        double m_ibt;
        VECTOR3 m_vel;
};

#endif // DATASOURCETRANSX_H
