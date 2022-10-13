/*
#include "DataSourceBaseSync.h"
#include "MFDDataBurnTime.h"
#include "BaseSyncExports.hpp"

using namespace EnjoLib;

DataSourceBaseSync::DataSourceBaseSync()
: m_ibt(0)
, m_dv(0)
{}

bool DataSourceBaseSync::GetFromMM(MFDDataBurnTime * data)
{
    bool receivingBS = false;

    if (data->BS_burn != NULL) {
      receivingBS = data->BS_burn->dataValid;
    } else {
		receivingBS = data->m_mmextAdv.GetMMStruct(GetName(), "BaseSyncBurn", &data->BS_burn, BASESYNC_EXPORT_BURN_VER, sizeof(BaseSyncExportBurnStruct));
      if (receivingBS) receivingBS = data->BS_burn->dataValid;
    }

    if (receivingBS) {
      m_dv = data->BS_burn->dV;
      m_ibt = data->BS_burn->tToInstBurn;
      data->BSori = data->BS_burn->orientation;
      return true;
    } else {
        *this = DataSourceBaseSync(); // Reinitialize
      return false;
    }
}

std::string DataSourceBaseSync::GetDisplayStringDV(MFDDataBurnTime * data) const
{
    switch (data->BSori)
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
*/
