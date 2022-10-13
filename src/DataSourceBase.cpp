#include "DataSourceBase.h"

DataSourceBase::DataSourceBase()
{}

std::string DataSourceBase::GetDisplayStringDV(MFDDataBurnTime * data) const
{
    return std::string(GetName()) + " DeltaV:      %7.3f";
}

std::string DataSourceBase::GetDisplayStringBT(MFDDataBurnTime * data) const
{
    return std::string(GetName()) + " T to Mnvre:  %7.3f";
}
