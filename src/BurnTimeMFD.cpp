// ==============================================================
// $Id: BurnTimeMFD.cpp,v 1.5 2004/05/25 19:08:46 dh219 Exp $
// BTC by D Henderson orbiter@aibs.org.uk
// Copyright 2004
// Extensively modified by C Jeppesen chrisj@kwansystems.org
// Copyright 2005-2006
// Extended with the ability to use different Engines (Hover,Main,Retro) by Topper
// Copyright 2007
// Version 1.5 by kwan3217 (Copyright (c) 2008):
//		Some code cleanup,
//		Bug case where engine group existed but was not connected to a propellant resource - FIXED
//		Works in SI or feet (for use with Space Shuttle)
// Version 2.0 by Topper Copyright (c)
//		Var-setting now working via inputbox
//		Entering of Targets (Vessels or Gbodys) instead of dv possible
//		New Autoburnmode "Burn by target distance" added
// Version 2.5 by Enjo (Copyright (c) 2014)
//    Interconnected with TransX via new ModuleMEssaging library (GET button)
//    Added a second button page via MFDButtonPage
// Version 2.6 by Enjo (Copyright (c) 2014)
//    Resolved CTDs when switching the MFD on and off
//    Simultaneous multiple vessels support (can be pre-programmed!) thanks to MultipleVesselsMFD
//    Added RCS Forward, Up, and Retro "engine types"
// Version 2.7 by Enjo (Copyright (c) 2014)
//    Resolved sporadic freeze on startup due to displaying of uninitialized variables
//    Resolved craft's "orbit freeze" bug when the MFD was closed and Time Acc. >= 10000 was applied
//    Added new button - RCS for including RCS fuel in total DV calculations
//    Internal: Changed "TBurn" ModuleMessaging variable to "InstantaneousBurnTime".
//    TransX update required for GET button to operate correctly
// Version 2.8 by ADSWNJ (Copyright (c) 2016)
//    Interconnected with BaseSync MFD for plane change and retro burns. GET button now auto-selects
//    TransX or BaseSync if either is running, or prompts user if both are running.
//    Miscellaneous display mods (removed space between number and symbol when the scale unit is x1,
//    put the Selected Engine Not Available in red, with advice to check doors and fuel, consistent
//    capitalization and decimal point alignment, etc.)
//    Replaced periapse and apoapse text with periapsis and apoapsis.
//    Put version number on MFD title, similar to BaseSyncMFD.
//    Updated the LGPL license text to version 3, below.
// Version 2.9 by ADSWNJ (Copyright (c) 2016)
//    Fixed text overwrite issue when in Target Mode and using invalid engine (i.e. doors closed or no propellant)
//    Fixed bug when using invalid engine and docked (burns instantly complete, instead of stating invalid engine)
// Version 3.0 by Enjo (Copyright (c) 2018)
//    Liberated from a direct linkage against ModuleMessagingExt. Now using dynamic linkage via LoadLibraryA
//    Created resource files, thus moving the MFD away from Misc to MFD modes in Orbiter Launchpad
//    Created extensible OOD communication and display classes
// Version 3.1 by Enjo (Copyright (c) 2018)
//    Liberated from BaseSynchExports.h. Now compiles as a self-contained project
//    Runtime library is now linked statically
//    Added a sound played upon arming the MFD

// Released under the terms of the LGPL: http://www.gnu.org/licenses/lgpl.txt
/*
    This software and sourcecode is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This Orbiter addon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this software and sourcecode; if not, see <http://www.gnu.org/licenses/>.

*/

// ==============================================================

#define STRICT
#define ORBITER_MODULE

#include "globals.h"
#include "MFDDataBurnTime.h"
#include "PluginBurnTime.h"

#define RED RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)
#define YELLOW RGB(255, 255, 0)
#define DARK_YELLOW RGB(128, 128, 0)
#define WHITE RGB(255, 255, 255)
#define BLUE RGB(50, 150, 255)
#define GRAY RGB(160, 160, 160)
#define BRIGHTERGRAY RGB(200, 200, 200)
#define ORANGE RGB(255, 130, 0)

#include "BurnTimeMFD.h"
#include "MFDButtonPageBTC.h"
#include "SoundSampleIDEnum.hpp"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string>
#include <cctype>
#include <algorithm>
#include "Comms.h"
#include "InputBox.h"

#define mToft 1.0/0.3048
#define gTolb 1.0/0.45359237
#define mpersToftpers 1.0/9.80665
#define NTolbf 0.224808943
#define mperssqToftperssq 1.0/0.3048

static MFDButtonPageBTC gButtons;
extern PluginBurnTime * plugin;

BurnTimeMFD::BurnTimeMFD (DWORD w, DWORD h, VESSEL *vessel, PluginBurnTime * plugin)
 : MFD2 (w, h, vessel)
 , m_data(dynamic_cast<MFDDataBurnTime *>(plugin->AssociateMFDData(vessel)))
 , m_graph(0,0,W,H)
 , m_sound(m_soundMap)
 {
  height = (int)h;
  if (m_data->mul==0) m_data->mul=1.0;
}

BurnTimeMFD::~BurnTimeMFD ()
{
}

bool BurnTimeMFD::Update(oapi::Sketchpad * skp)
{
	m_data->Update();
	//m_data->ReGetDataFromSource(); // confusing
  unsigned int thrustercount = 0;

  Title (skp, "BurnTimeMFD v3.1");


  //m_graph.vectorpointdisplay(skp, m_data->velVector, pV);

  int line1 = 1;
  int line8 = 2;
  int line2 = 3;
  int line9 = 4;
  int line3 = 5;
  int line4 = 6;
  int line5 = 7;
  int line6 = 8;
  int line7 = 9;

  int line10=10;
  int line11=11;
  int line12=12;
  int line13=13;
  int line14=14;
  int line15=15;
  int line16=16;
  int line17=17;
  int line18=18;
  int line19=19;

  skp->SetTextColor(GRAY);

  if (m_data->mode == BURNMODE_PERI) PrintEngUnit(skp,"Time to Periapsis:     %7.3f","s", m_data->IPeri, 5, line8 );
  if(m_data->IApo>0 && m_data->mode == BURNMODE_APO)
  {
    skp->SetTextColor(GRAY);
    PrintEngUnit(skp,"Time to Apoapsis:      %7.3f","s", m_data->IApo, 5, line8 );
  }

  const DataSourceBase * otherSrc = m_data->GetCurrentSource();
  if (m_data->mode == BURNMODE_MAN)
  {
      if (otherSrc == NULL)
      {
          skp->SetTextColor(((m_data->inputmode==INPUTMODE_TIME )?YELLOW:GRAY));
          PrintEngUnit(skp,"Time to Manual Start:  %7.3f","s", m_data->IManual, 5, line8 );
      }
      else
      {
          skp->SetTextColor(YELLOW);
          PrintEngUnit(skp, otherSrc->GetDisplayStringBT(m_data).c_str(),"s", m_data->IManual, 5, line8 );
      }
  }

  if (m_data->mode == BURNMODE_TGT && m_data->maxdv!=0 && m_data->ph != NULL)
  {

    skp->SetTextColor(GRAY);
    PrintEngUnit(skp,"Distance to Target:    %7.3f","m", "ft",1,mToft,m_data->TDist, 5, line8 );
    skp->SetTextColor(((m_data->inputmode==INPUTMODE_OFFSET )?YELLOW:GRAY));
    PrintEngUnit(skp,"Offset Distance:       %7.3f","m", "ft",1,mToft,m_data->sOffset, 5, line9 );
  }

  skp->SetTextColor( BRIGHTERGRAY );
  PrintString(skp,"Engine:                   %s",&m_data->group_names[m_data->Sel_eng][0],5,line10);

  if(m_data->maxdv==0 || m_data->ph == NULL)
  {
	  skp->SetTextColor( RED );
    PrintString(skp,"Selected engine not available%s","",5,line2);
    PrintString(skp,">>> Check engine doors and fuel%s","",5,line9);
  }
  else
  {
	if(m_data->IsEngaged)
	{
	  skp->SetTextColor( RED );
	  skp->Text(5,line(line3),"Autoburn Engaged",16);
	  PrintEngUnit(skp,"Time to Cutoff:        %7.3f","s", m_data->ECutoff-m_data->ENow,5,line4);
	}
	else if(m_data->IsArmed)
	{
	  skp->SetTextColor( YELLOW );

	  if(m_data->IsCircular)
	  {
	     PrintEngUnit(skp,"Autocirc Armed:        %7.3f","s", m_data->IBurn,5,line3);
	  }
	  else
	  {
	    PrintEngUnit(skp,"Autoburn Armed:        %7.3f","s", m_data->IBurn,5,line3);
	  }
	  PrintEngUnit(skp,"Time to Ignition:      %7.3f","s", m_data->GetTimeToIgnition(),5,line4);
	}
	else
	{
	  skp->SetTextColor( GREEN );
	  PrintEngUnit(skp,"Estimated Burn Time:   %7.3f","s", m_data->IBurn,5,line3);
	  PrintEngUnit(skp,"Half DeltaV Time       %7.3f","s", m_data->IBurn2,5,line4);
	  PrintEngUnit(skp,"Rel. DeltaS Distance   %7.3f","m", "ft",1,mToft,m_data->dv*m_data->IBurn2, 5, line5 );
	}

	skp->SetTextColor( (m_data->inputmode==INPUTMODE_DV)?YELLOW:GRAY );
    if (m_data->mode == BURNMODE_MAN)
    {

        if (otherSrc == NULL)
            PrintEngUnit(skp, "Target DeltaV:         %7.3f","m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
        else
        {
            skp->SetTextColor( YELLOW );
            PrintEngUnit(skp, otherSrc->GetDisplayStringDV(m_data).c_str(),"m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
        }
    }
    else
    {
        PrintEngUnit(skp,"Target DeltaV:         %7.3f","m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
    }

	skp->SetTextColor( (m_data->inputmode==INPUTMODE_EXTRA)?YELLOW:BLUE );
	PrintEngUnit(skp,"Extra Fuel Mass:       %7.3f","g","lbm", 1000,gTolb,m_data->mextra,5,line19);
	if (m_data->mextra !=0){
		skp->SetTextColor( ORANGE );
	    PrintEngUnit(skp,"Extra Fuel Mass:       %7.3f","g","lbm", 1000,gTolb,m_data->mextra,5,line19);
	}

    if (m_data->mode == BURNMODE_PERI || m_data->mode == BURNMODE_APO)// || m_data->mode == BURNMODE_MAN)
    {
        skp->SetTextColor((m_data->inputmode == INPUTMODE_DISTANCE) ? YELLOW : GRAY);
        PrintEngUnit(skp, "Max Altitude Change:   %7.3f", "m", "ft", 1, mToft, m_data->dDist, 5, line2);
        skp->SetTextColor((m_data->inputmode == INPUTMODE_PERIOD) ? YELLOW : GRAY);
        PrintEngUnit(skp, "Orbit Period Change:   %7.3f", "s", m_data->dPeriod, 5, line9);
    }
    


	std::string TargetString;

	char buffer[255];

	if (m_data->mode == BURNMODE_TGT)
	{
	  skp->SetTextColor(m_data->inputmode==INPUTMODE_TARGET?YELLOW:GRAY);
	  oapiGetObjectName(oapiGetObjectByIndex(m_data->IndexCenterObj),buffer,255);
	  TargetString = "=> Selected Object: " + std::string(buffer);
	  skp->Text(5,line(line2),TargetString.c_str(),TargetString.length());
	}

	skp->SetTextColor( ORANGE );
	std::string totalDV = std::string("Estimate Total dV") + (m_data->includeRCS ? "+RCS: " : ":     ") + "%7.3f";
	PrintEngUnit(skp,totalDV.c_str()                ,"m/s","ft/s",1,mToft, m_data->maxdv,5,line7);
	PrintEngUnit(skp,"Total Burn Time:       %7.3f","s", m_data->TTot,5,line6);
	skp->SetTextColor( BLUE );
	PrintEngUnit(skp,"Current Vehicle Mass:  %7.3f","g","lbm", 1000,gTolb,m_data->mv,5,line11);
	PrintEngUnit(skp,"Current Stack Mass:    %7.3f","g","lbm", 1000,gTolb, m_data->ms,5,line12);
	PrintEngUnit(skp,"Postburn Vehicle Mass: %7.3f","g","lbm", 1000,gTolb, m_data->mfinal,5,line13);
	PrintEngUnit(skp,"Empty Vehicle Mass:    %7.3f","g","lbm", 1000,gTolb, m_data->me,5,line14);
	PrintEngUnit(skp,"Mass Flow Rate:        %7.3f","g/s","lbm/s", 1000,gTolb, m_data->mdot,5,line15);
	PrintEngUnit(skp,"Eng Thrust:            %7.3f","N","lbf",1,NTolbf, m_data->F,5,line16);
	PrintEngUnit(skp,"Eng Isp:               %7.3f","m/s","ft/s",1,mpersToftpers, m_data->isp,5,line17);
	PrintEngUnit(skp,"Eng Acc:               %7.3f","m/s²","ft/s²",1,mperssqToftperssq,m_data->F/m_data->ms,5,line18);
  }
  return true;
}

int BurnTimeMFD::MsgProc (UINT msg, UINT mfinald, WPARAM wparam, LPARAM lparam)
{
  switch (msg)
  {
    case OAPI_MSG_MFD_OPENED:
       return (int)(new BurnTimeMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam, plugin));
  }

  return 0;
}

MFDDataBurnTime * BurnTimeMFD::GetData()
{
    return m_data;
}

bool BurnTimeMFD::ConsumeKeyBuffered (DWORD key)
{
    return gButtons.ConsumeKeyBuffered(this, key);
}

void BurnTimeMFD::HandlerSwitchButtonPage()
{
    gButtons.SwitchPage(this);
}

void BurnTimeMFD::HandlerTargetOrDV()
{
    m_data->inputmode=INPUTMODE_DV;
    m_data->mode = BURNMODE_MAN;
    m_data->otherMFDsel = -1;
    bool ObjectInput (void *id, char *str, void *usrdata);
    oapiOpenInputBox("Enter dV + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerTimeOfManoeuvre()
{
   m_data->inputmode=INPUTMODE_TIME;
   m_data->mode = BURNMODE_MAN;
   m_data->otherMFDsel = -1;
   bool ObjectInput (void *id, char *str, void *usrdata);
   oapiOpenInputBox("Enter dT + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerOffsetDistance()
{
    m_data->inputmode=INPUTMODE_OFFSET;
    m_data->mode = BURNMODE_TGT;
    oapiOpenInputBox("Enter offset distance + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerTargetForDistanceCalc()
{
    m_data->inputmode=INPUTMODE_TARGET;
    m_data->mode = BURNMODE_TGT;
    oapiOpenInputBox("Enter unique part of the target's name.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerReset()
{
	  m_data->mul=1.0;
	  m_data->dv = 0.0;
	  m_data->mextra = 0.0;
	  m_data->ECutoff=0;
	  m_data->IsEngaged=false;
	  m_data->IsArmed=false;
	  m_data->mode=BURNMODE_PERI;
      m_data->otherMFDsel = -1;
	  m_data->IManual=0;
	  m_data->IsCircular=false;
      m_data->dDist = 0.0;
      m_data->dPeriod = 0.0;
}

void BurnTimeMFD::HandlerChangeMode()
{
	  if(m_data->IsArmed && m_data->IsEngaged) return;
	  m_data->mode++;
	  if(m_data->mode>=BURNMODE_TGT) m_data->mode=0;
}

void BurnTimeMFD::HandlerAutoBurn()
{
      if(m_data->IsArmed)
      {
        m_data->IsArmed=false;
        m_data->otherMFDsel = -1;
        return;
      }
      if(m_data->IsEngaged)
      {
        m_data->IsEngaged=false;
      }
      oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(m_data->groups[m_data->Sel_eng],0);
      m_data->ArmAutoBurn();
      m_sound.PlayWave(SOUND_ARM);
}

void BurnTimeMFD::HandlerBurnNow()
{
      m_data->IsArmed=false;
      m_data->IsEngaged=true;
      m_data->ECutoff=oapiGetSimTime()+m_data->IBurn;
      oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(m_data->groups[m_data->Sel_eng],1.0);
}

void BurnTimeMFD::HandlerAimAutoCirc()
{
      if (m_data->mode > BURNMODE_APO) m_data->mode = BURNMODE_PERI;
      if(m_data->IsArmed)
      {
        m_data->IsArmed=false;
        m_data->IsCircular=false;
        return;
      }
      if(m_data->IsEngaged)
      {
        m_data->IsEngaged=false;
        oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(m_data->groups[m_data->Sel_eng],0);
      }
      m_data->IsCircular=true;
      m_data->CalcCircular();
      m_data->ArmAutoBurn();
      m_sound.PlayWave(SOUND_ARM);
}

void BurnTimeMFD::HandlerSwitchSI_US()
{
    m_data->dspunit=1-m_data->dspunit;
}

void BurnTimeMFD::HandlerSelectEngine()
{
      m_data->Sel_eng++;
      if (m_data->Sel_eng==MFDDataBurnTime::numEngines) m_data->Sel_eng=0;
}

void BurnTimeMFD::HandlerEnterExtraFuel()
{
	  m_data->inputmode=INPUTMODE_EXTRA;
	  bool ObjectInput (void *id, char *str, void *usrdata);
	  oapiOpenInputBox("Enter extra fuel mass in kilograms",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerIncludeRCSFuel()
{
    m_data->includeRCS = ! m_data->includeRCS;
}

/*
void BurnTimeMFD::HandlerAutopilot()
{
    m_data->autopilot.SetTargetVector(m_data->velVector);
}

void BurnTimeMFD::HandlerAutopilotDisable()
{
    m_data->autopilot.Disable();
}
*/

void BurnTimeMFD::HandlerGetFromOtherMFD()
{
    if (Comms().HandlerGetFromOtherMFD(this, m_data))
        HandlerAutoBurn();
}

void BurnTimeMFD::HandlerDRadialDistance()
{
    m_data->inputmode = INPUTMODE_DISTANCE;
    m_data->mode = BURNMODE_PERI;
    m_data->otherMFDsel = -1;
    bool ObjectInput(void* id, char* str, void* usrdata);
    oapiOpenInputBox("Enter Radial DISTANCE change(yzafpnum kMGTPEZY):", ObjectInput, 0, 20, (void*)this);
}
void BurnTimeMFD::HandlerDOrbitPeriod()
{
    m_data->inputmode = INPUTMODE_PERIOD;
    m_data->mode = BURNMODE_PERI;
    m_data->otherMFDsel = -1;
    bool ObjectInput(void* id, char* str, void* usrdata);
    oapiOpenInputBox("Enter Orbit PERIOD change(yzafpnum kMGTPEZY):", ObjectInput, 0, 20, (void*)this);
}

int BurnTimeMFD::line( int i ) {
  return (int)((float)i*((float)height/20.0));
}

int BurnTimeMFD::ButtonMenu (const MFDBUTTONMENU **menu) const {
  return gButtons.ButtonMenu(menu);
}

char *BurnTimeMFD::ButtonLabel (int bt) {
  //char *label[12] = {"DV","DT","OS","ST","RST", "MD", "ARM","BRN", "CIR","UNT","ENG","EXT", "DRD", "DOP"};
  //return (bt < 12 ? label[bt] : 0);
  return gButtons.ButtonLabel(bt);
}

bool BurnTimeMFD::ConsumeButton(int bt, int event) {
  return gButtons.ConsumeButton(this, bt, event);
}

int DisplayEngUnit(char* buffer, char* pattern, char* noscalepattern, double x) {
  char Big[]=  " kMGTPEZY";
  char Small[]=" munpfazy";
  int ptr=0;
  if(fabs(x)>1e-24) {
    if(fabs(x)>1) {
      while(fabs(x)>1000) {
        ptr++;
        x=x/1000.0;
      }
      if (ptr==0) {
        return sprintf(buffer,noscalepattern,x);
      } else {
        return sprintf(buffer,pattern,x,Big[ptr]);
      }
    } else {
      while(fabs(x)<1) {
        ptr++;
        x=x*1000.0;
      }
      if (ptr==0) {
        return sprintf(buffer,noscalepattern,x);
      } else {
        return sprintf(buffer,pattern,x,Small[ptr]);
      }
    }
  } else {
    return sprintf(buffer,noscalepattern,x);
  }
}

void BurnTimeMFD::PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* unitSI, const char* unitUS, double multSI, double multUS, double value, int x, int l) {
  char buf[256];
  char label[256];
  char noscalelabel[256];
  sprintf(label,"%s%%c%s",format,m_data->dspunit==0?unitSI:unitUS);
  sprintf(noscalelabel,"%s%s",format,m_data->dspunit==0?unitSI:unitUS);
  int len=DisplayEngUnit(buf,label,noscalelabel,value*(m_data->dspunit==0?multSI:multUS));
  skp->Text(x,line(l),buf,len);
}

void BurnTimeMFD::PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* units, double mult, double value, int x, int l) {
  char buf[256];
  char label[256];
  char noscalelabel[256];
  sprintf(label,"%s%%c%s",format,units);
  sprintf(noscalelabel,"%s%s",format,units);
  int len=DisplayEngUnit(buf,label,noscalelabel,value*mult);
  skp->Text(x,line(l),buf,len);
}

void BurnTimeMFD::PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* units, double value, int x, int l) {
  char buf[256];
  char label[256];
  char noscalelabel[256];
  sprintf(label,"%s%%c%s",format,units);
  sprintf(noscalelabel,"%s%s",format,units);
  int len=DisplayEngUnit(buf,label,noscalelabel,value);
  skp->Text(x,line(l),buf,len);
}

void BurnTimeMFD::PrintString(oapi::Sketchpad * skp, const char* format, const char* value, int x, int l) {
  char buf[256];
  sprintf(buf,format,value);
  skp->Text(x,line(l),buf,strlen(buf));
}

void BurnTimeMFD::PrintString(oapi::Sketchpad * skp, const char* format, int x, int l) {
  char buf[256];
  skp->Text(x,line(l),format,strlen(buf));
}