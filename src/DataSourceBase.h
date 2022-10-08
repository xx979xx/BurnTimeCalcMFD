#ifndef DATASOURCEBASE_H
#define DATASOURCEBASE_H

#include <OrbiterAPI.h>
#include <string>
class MFDDataBurnTime;
class DataSourceBase
{
    public:
        DataSourceBase();
        virtual ~DataSourceBase() {}

        virtual const char * GetName() const = 0;
        virtual bool GetFromMM(MFDDataBurnTime * data) = 0;
        virtual double GetIBT() const = 0;
        virtual double GetDV() const { return length(GetVelVec()); }
        virtual VECTOR3 GetVelVec() const { return _V(0, 0, 0); }
        virtual std::string GetDisplayStringDV(MFDDataBurnTime * data) const;
        virtual std::string GetDisplayStringBT(MFDDataBurnTime * data) const;
    protected:

    private:
};

#endif // DATASOURCEBASE_H
