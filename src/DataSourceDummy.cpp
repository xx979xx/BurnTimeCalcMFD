#include "DataSourceDummy.h"
#include "MFDDataBurnTime.h"

using namespace EnjoLib;

DataSourceDummy::DataSourceDummy()
: m_ibt(0)
, m_dv(0)
{}

bool DataSourceDummy::GetFromMM(MFDDataBurnTime * data)
{
	if (data->m_mmextBas.Get(GetName(), "dv", &m_dv) &&
        data->m_mmextBas.Get(GetName(), "InstantaneousBurnTime", &m_ibt)) {
        // Received Dummy (Launch MFD)
        data->velVector = GetVelVec(); // Testing
      return true;
    } else {
      *this = DataSourceDummy(); // Reinitialize
      return false;
    }
}
