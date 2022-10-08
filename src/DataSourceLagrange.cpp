#include "DataSourceLagrange.h"
#include "MFDDataBurnTime.h"

DataSourceLagrange::DataSourceLagrange()
: m_ibt(0)
, m_vel(_V(0, 0, 0))
{
    //ctor
}

bool DataSourceLagrange::GetFromMM(MFDDataBurnTime * data)
{
    if (data->m_mmextBas.Get(GetName(), "TargetVelocity", &m_vel) &&
        data->m_mmextBas.Get(GetName(), "InstantaneousBurnTime", &m_ibt)) {
        // Received LagrangeMFD
      return true;
    } else {
      *this = DataSourceLagrange(); // Reinitialize
      return false;
    }
}
