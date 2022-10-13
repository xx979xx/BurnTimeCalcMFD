#include "PluginBurnTime.h"
#include "MFDDataBurnTime.h"
#include "BurnTimeMFD.h"

using namespace EnjoLib::MFDGoodies;

PluginBurnTime * plugin;

static struct {
  int mode;
} g_BurnTimeMFD;

DLLCLBK void InitModule (HINSTANCE hDLL) {
  static char *name = "BurnTimeMFD";
  MFDMODESPECEX spec;
  spec.name    = name;
  spec.key     = OAPI_KEY_T;
  spec.msgproc = BurnTimeMFD::MsgProc;
  spec.context = NULL;

  g_BurnTimeMFD.mode = oapiRegisterMFDMode (spec);
  plugin = new PluginBurnTime( hDLL );
  oapiRegisterModule (plugin);
}

DLLCLBK void ExitModule (HINSTANCE hDLL) {
  oapiUnregisterMFDMode (g_BurnTimeMFD.mode);
}

PluginBurnTime::PluginBurnTime(HINSTANCE hDLL)
: PluginMultipleVessels( hDLL )
{}

PluginBurnTime::~PluginBurnTime()
{}

void PluginBurnTime::InitClient()
{
}

void PluginBurnTime::CleanupClient()
{
}

MFDData * PluginBurnTime::ConstructNewMFDData( VESSEL * vessel )
{
    return new MFDDataBurnTime( vessel );
}

void PluginBurnTime::UpdateClientPreStep( MFDData * data, const SimulationTimeData & timeData )
{
    MFDDataBurnTime * dataBurnTime = static_cast<MFDDataBurnTime *>( data );
    dataBurnTime->Update();
    //dataBurnTime->autopilot.Update(timeData.GetSimDT());
}

void PluginBurnTime::UpdateClientPostStep( MFDData * data, const SimulationTimeData & timeData )
{
}
