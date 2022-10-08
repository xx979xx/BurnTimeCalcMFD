#include "Comms.h"

#include "globals.h"
//#include <EnjoLib/ModuleMessaging.hpp>
//#include <EnjoLib/ModuleMessagingExt.hpp>

#include "MFDDataBurnTime.h"

#include <string>
#include <sstream>

using namespace std;
using namespace EnjoLib;

bool ObjectInput (void *id, char *str, void *usrdata);

Comms::Comms(){}
Comms::~Comms(){}

bool Comms::ParseInput(char * str, MFDDataBurnTime * data)
{
    if (strlen(str) != 1) return false;
    istringstream issId;
    issId.str(str);

    int sourceId = -1;
    if (!(issId >> sourceId))
        return false;
    --sourceId;
    if (sourceId < 0 || sourceId >= int(data->m_dataSources.size()))
        return false;

    return SetOtherMFDBurnVars(data->m_dataSources.at(sourceId), sourceId, data);
}


bool Comms::HandlerGetFromOtherMFD(BurnTimeMFD * btcMFD, MFDDataBurnTime * data)
{
    vector<int> foundIDs;
    for (unsigned i = 0; i < data->m_dataSources.size(); ++i)
    {
        if (data->m_dataSources.at(i)->GetFromMM(data))
            foundIDs.push_back(i);
    }
    if (foundIDs.empty())  // no data exposed
        return false;
    if (foundIDs.size() == 1)  // no disambiguation
        return SetOtherMFDBurnVars(data->m_dataSources.at(foundIDs.at(0)), foundIDs.at(0), data);
    // Disambiguation. Let the user choose the source
    ostringstream ossChoice;
    ossChoice << "Choose source:";
    for (unsigned i = 0; i < foundIDs.size(); ++i)
    {
        int sourceId = foundIDs.at(i);
        ossChoice << "  " << sourceId + 1 << " = " << data->m_dataSources.at(sourceId)->GetName();
    }
    data->inputmode = INPUTMODE_MFDSEL;
    oapiOpenInputBox((char*)ossChoice.str().c_str(),ObjectInput,0,20, (void*)btcMFD);
    return false;
}


bool Comms::SetOtherMFDBurnVars(const DataSourceBase * source, int sourceId, MFDDataBurnTime * data)
{
    data->IsArmed=data->IsEngaged=false;
    data->mode = BURNMODE_MAN;
    const double dv = fabs(source->GetDV());
	const double ibt = source->GetIBT();
    if (dv > 1e-5 && ibt > 0)
    { // We're not interested in negative times or zero dV's
        data->otherMFDsel = sourceId;
        data->velVector = source->GetVelVec();
        data->dv = dv;
        data->IManual = ibt;
        //HandlerAutoBurn();
        return true;
    }
    data->otherMFDsel = -1;
    return false;
}
