#include "DataSourceBaseSyncSimple.h"
#include "MFDDataBurnTime.h"

DataSourceBaseSyncSimple::DataSourceBaseSyncSimple()
: m_ibt(0)
, m_dv(0)
, m_orientation(0)
{}

bool DataSourceBaseSyncSimple::GetFromMM(MFDDataBurnTime * data)
{
    if (data->m_mmextBas.Get(GetName(), "dv", &m_dv) &&
        data->m_mmextBas.Get(GetName(), "InstantaneousBurnTime", &m_ibt) &&
        data->m_mmextBas.Get(GetName(), "Orientation", &m_orientation)
        ) {
        // Got everything
      return true;
    } else {
      *this = DataSourceBaseSyncSimple(); // Reinitialize
      return false;
    }
}

std::string DataSourceBaseSyncSimple::GetDisplayStringDV(MFDDataBurnTime * data) const
{
    switch (m_orientation)
    {
        case -1:
            return("BaseSync Nml- dV:      %7.3f");
        case 0:
            return("BaseSync Retro dV:     %7.3f");
        case 1:
            return("BaseSync Nml+ dV:      %7.3f");
    }
    return "BaseSync inv. orient.  %7.3f";
}
