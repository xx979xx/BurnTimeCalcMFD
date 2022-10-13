#ifndef DATASOURCEBASESYNCSIMPLE_H
#define DATASOURCEBASESYNCSIMPLE_H

#include "DataSourceBase.h"


class DataSourceBaseSyncSimple : public DataSourceBase
{
    public:
        DataSourceBaseSyncSimple();
        virtual ~DataSourceBaseSyncSimple() {}

        const char * GetName() const { return "BaseSyncMFD"; }
        bool GetFromMM(MFDDataBurnTime * data);
        double GetIBT() const { return m_ibt; }
        double GetDV() const { return m_dv; }
        std::string GetDisplayStringDV(MFDDataBurnTime * data) const;

    protected:

    private:
        double m_ibt;
        double m_dv;
        int m_orientation;
};

#endif // DATASOURCEBASESYNCSIMPLE_H
