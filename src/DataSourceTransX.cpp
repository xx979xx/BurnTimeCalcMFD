#include "DataSourceTransX.h"
#include "MFDDataBurnTime.h"

DataSourceTransX::DataSourceTransX()
: m_ibt(0)
, m_dv(0)
, m_vel(_V(0, 0, 0))
{}

bool DataSourceTransX::GetFromMM(MFDDataBurnTime * data)
{
    if (data->m_mmextBas.Get(GetName(), "InstantaneousBurnTime", &m_ibt))
    {
        if (data->m_mmextBas.Get(GetName(), "dv", &m_dv))
        {
            data->m_mmextBas.Get(GetName(), "TargetVelocity", &m_vel); // Try getting the target vector as well
        // Received TransX
            return true;
        }
    }
    *this = DataSourceTransX(); // Reinitialize
    //m_ibt = m_dv = 0;
    return false;
}
