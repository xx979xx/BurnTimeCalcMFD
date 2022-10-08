#ifndef BURNTIMEMFD_H_INCLUDED
#define BURNTIMEMFD_H_INCLUDED

#include <orbitersdk.h>
#include "Graph.h"
#include <MFDSound++/Sound.hpp>
#include "SoundMapBTC.h"

class MFDDataBurnTime;
class PluginBurnTime;
class BurnTimeMFD: public MFD2 {
public:
  BurnTimeMFD (DWORD w, DWORD h, VESSEL *vessel, PluginBurnTime * plugin);
  ~BurnTimeMFD ();
  bool ConsumeKeyBuffered (DWORD key);
  bool Update (oapi::Sketchpad * skp);
  static int MsgProc (UINT msg, UINT mfinald, WPARAM wparam, LPARAM lparam);
  int ButtonMenu (const MFDBUTTONMENU **menu) const;
  char *ButtonLabel (int bt);
  bool ConsumeButton(int bt, int event);

  void PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* units, double mult, double value, int x, int l);
  void PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* units, double value, int x, int l);
  void PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* unitSI, const char* unitUS, double multSI, double multUS, double value, int x, int l);
  void PrintString(oapi::Sketchpad * skp, const char* format, const char* value, int x, int l);
  void PrintString(oapi::Sketchpad * skp, const char* format, int x, int l);

  MFDDataBurnTime * GetData();
  MFDDataBurnTime * m_data;

  //Button page handlers
  void HandlerSwitchButtonPage();
  void HandlerTargetOrDV();
  void HandlerTimeOfManoeuvre();
  void HandlerOffsetDistance();
  void HandlerTargetForDistanceCalc();
  void HandlerReset();
  void HandlerChangeMode();
  void HandlerAutoBurn();
  void HandlerBurnNow();
  void HandlerAimAutoCirc();
  void HandlerSwitchSI_US();
  void HandlerSelectEngine();
  void HandlerEnterExtraFuel();
  void HandlerIncludeRCSFuel();
  void HandlerGetFromOtherMFD();
  //void HandlerAutopilot();
  //void HandlerAutopilotDisable();
  void HandlerDRadialDistance();
  void HandlerDOrbitPeriod();

protected:
  VESSEL *messel;
  int height;

private:
  //static int Sel_eng;
	void writemenu(oapi::Sketchpad * skp);
	int line( int );
  Graph m_graph;

  SoundMapBTC m_soundMap;
  EnjoLib::MFDGoodies::Sound m_sound;
};

#endif // BURNTIMEMFD_H_INCLUDED
