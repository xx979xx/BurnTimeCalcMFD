#include "MFDDataBurnTime.h"
#include "globals.h"
#include <OrbiterSdk.h>
#include <Orbiter/BurnTime.hpp>

#include "DataSourceTransX.h"
#include "DataSourceBaseSync.h"
#include "DataSourceBaseSyncSimple.h"
#include "DataSourceLagrange.h"
#include "DataSourceDummy.h"

const int MFDDataBurnTime::numEngines = 6;
const THGROUP_TYPE MFDDataBurnTime::groups[numEngines]={THGROUP_MAIN,THGROUP_HOVER,THGROUP_RETRO, THGROUP_ATT_FORWARD, THGROUP_ATT_UP, THGROUP_ATT_BACK};
const char MFDDataBurnTime::group_names[numEngines][7]={"Main","Hover","Retro","RCS FW","RCS UP","RCS RE"};
const MMExt2::Advanced MFDDataBurnTime::m_mmextAdv("BTC");
const MMExt2::Basic    MFDDataBurnTime::m_mmextBas("BTC");

MFDDataBurnTime::MFDDataBurnTime(VESSEL * vessel)
: MFDData(vessel)
{
    mul=1.0;
    dv = 0.0;
    mextra = mrcs = 0.0;
    ECutoff=0;
    IsEngaged=false;
    IsArmed=false;
    includeRCS = false;
    mode=BURNMODE_PERI;
    IManual=0;
    IsCircular=false;
    Sel_eng = 0;
    dspunit=0;
    TDist=0;
    sOffset=500;
    inputmode = 0;
    IndexCenterObj = 0;
    BS_burn = NULL;
    otherMFDsel = -1;
    BSori = 0;
    velVector = _V(0,0,0);
    dDist = 0.0;
    Period = 0.0;
    dPeriod = 0.0;

    m_dataSources.push_back(new DataSourceTransX());
    m_dataSources.push_back(new DataSourceBaseSyncSimple());
    //m_dataSources.push_back(new DataSourceBaseSync());
    m_dataSources.push_back(new DataSourceLagrange());
    m_dataSources.push_back(new DataSourceDummy());
}

MFDDataBurnTime::~MFDDataBurnTime()
{
    for (unsigned i = 0; i < m_dataSources.size(); ++i)
        delete m_dataSources.at(i);
}

void MFDDataBurnTime::Update()
{
    //ReGetDataFromSource(); // confusing
    ArmAutopilot();
    ENow=oapiGetSimTime();
    if (!IsEngaged)
    {
        dvOld = 0;
    }
  VESSEL *vessel = GetVessel();
  CalcApses(vessel);
  CalcIBurn(vessel);
  if(IsArmed)
  {
    if (mode==BURNMODE_MAN)
    {
      IManual=EReference-ENow;
    }
	if (mode==BURNMODE_TGT)
	{
	  if (dv == 0)
	  {
		  dv = 0;
		  IReference=0;
		  EReference=0;
	  }
	  else
	  {
		 IReference=(TDist-sOffset)/(dv);
	     EReference=oapiGetSimTime()+IReference;
	  }
	}
    if(IsCircular)
    {
      CalcCircular();
      EReference=ENow+IReference;
    }
  }

  if (isp!=0) mdot=F/isp;
  else mdot=0;

  if(IsEngaged)
  {
    mfinal=mv-(ECutoff-ENow)*mdot;
    msfinal=ms-mv+mfinal;
	if (msfinal!=0) dv=isp*log(ms/msfinal);
	else dv=0;
  }
  else
  {
    mfinal=mv-IBurn*mdot;
  }
  msempty=ms-mv+me;

  if (msempty !=0 ) maxdv = isp * log( ms / (ms - mfuel) );
  else maxdv=0;

  if (mdot!=0) TTot=mfuel/mdot;
  else TTot=0;

  OBJHANDLE hSObj = oapiGetObjectByIndex(IndexCenterObj);
  double dvtmp=0;
  if (mode==BURNMODE_TGT)
  {
    VECTOR3 refval;
	vessel->GetRelativeVel(hSObj,refval);
	dvcurr = length(refval);

	//IReference= (TDist-sOffset)/(((dvcurr-dv)/2)+dv);
	if (dvcurr>dv) IReference= (TDist-sOffset)/(dv+((dvcurr-dv)/2));
	else           IReference= (TDist-sOffset)/(dvcurr +((dv-dvcurr)/2));

    EReference=oapiGetSimTime()+IReference;

//	sprintf(oapiDebugString(),"dVcurr: %f, dV: %f; MID: %f",dvcurr,dv,((dvcurr-dv)/2)+dv);
	vessel->GetRelativePos(hSObj,refval);
	TDist = length(refval);
  }
  else if (mode >= BURNMODE_TGT) mode = BURNMODE_PERI;


  if(IsEngaged && ENow>ECutoff && mode!=BURNMODE_TGT)
  {
	vessel->SetThrusterGroupLevel(groups[Sel_eng],0);
    IsEngaged=false;
    autopilot.Disable();
    dv=0;
	IManual = 0;
  }

  if (mode==BURNMODE_TGT && dvOld<dvcurr && IsEngaged)
  {
    vessel->SetThrusterGroupLevel(groups[Sel_eng],0);
    IsEngaged=false;
    autopilot.Disable();
    dv=0;
  }

  if(IsArmed && ENow>(EReference-IBurn2))
  {
	vessel->SetThrusterGroupLevel(groups[Sel_eng],1);
    IsEngaged=true;
    IsArmed=false;
    IsCircular=false;
    ECutoff=ENow+IBurn;
  }
  dvOld = dvcurr;
}

void getGroupThrustParm(VESSEL* vessel, THGROUP_TYPE group, double *F, double *isp) {
  *F = 0;
  *isp = 0;
  int nthr=vessel->GetGroupThrusterCount(group);

  for(int i=0; i<nthr; i++) {
    THRUSTER_HANDLE th=vessel->GetGroupThruster(group,i);
    *F += vessel->GetThrusterMax0(th);
	PROPELLANT_HANDLE ph=vessel->GetThrusterResource(th);
	double eff=1.0; //Some vessels play games with the propellant handles...
	if(ph!=NULL) {
	  //So only measure efficiency if it's attached to a prop tank, else assume 1.0
      eff=vessel->GetPropellantEfficiency(ph);
	}
    *isp += vessel->GetThrusterIsp(th) * vessel->GetThrusterMax0(th)*eff;
  }
  if (*F != 0)	*isp /= *F;
  else *isp=0;

}


double RocketEqnT(double dv, double m, double F, double isp) {

  return ( dv * m / (2.0 * F ) ) * ( 1 + exp( -1.0 * dv / isp ) );
}


void MFDDataBurnTime::CalcApses(VESSEL* vessel) {
  ELEMENTS el;
  ORBITPARAM op;
  OBJHANDLE Ref = vessel->GetGravityRef();
  vessel->GetElements(Ref, el, &op);

  e=el.e;
  a=el.a;
  mu=oapiGetMass(Ref)*GGRAV;
  double n=sqrt((e<1?1:-1)*mu/(a*a*a));
  double M=el.L-el.omegab;
  double MJD=oapiTime2MJD(oapiGetSimTime());

  if(e<1) {
    while(M<0)M+=2*PI;
    while(M>2*PI)M-=2*PI;
  }

  IPeri=-M/n;
  Period=(2*PI)/n;
  Rperi=a*(1-e);
  if(e<1) {
    Rapo=a*(1+e);
    IApo=IPeri+Period/2;
    while(IPeri<0)IPeri+=Period;
    while(IApo<0)IApo+=Period;
  } else {
    IApo=0;
    Rapo=0;
    if(mode==BURNMODE_APO) mode=BURNMODE_PERI;
  }
}

void MFDDataBurnTime::CalcCircular() {
  double Rapse;
  double Vcirc;
  double Vapse;
  switch(mode) {
    case BURNMODE_PERI:
      Rapse=Rperi;
      IReference=IPeri;
      break;
    case BURNMODE_APO:
      Rapse=Rapo;
      IReference=IApo;
      break;
    case BURNMODE_MAN:
      Rapse=Rapo;
      IReference=IManual;
      break;
  }
  Vcirc=sqrt(mu/Rapse);
  Vapse=sqrt(2*mu/Rapse-mu/a);
  dv=fabs(Vcirc-Vapse);
}

void MFDDataBurnTime::CalcDRadDPeri() {
    if (e > 1.0 - 1e-16)
    {
        if (dDist != 0.0) dDist = 0.0;
        return;
    }
    double Rapse;
    double Vtrgt;
    double Vapse;
    double Atrgt;
    double Ptrgt;
    double Rtrgt;
    switch (mode) {
    case BURNMODE_PERI:
        Rapse = Rperi;
        IReference = IPeri;
        break;
    case BURNMODE_APO:
        Rapse = Rapo;
        IReference = IApo;
        break;
    case BURNMODE_MAN:
        Rapse = Rapo;
        IReference = IManual;
        break;
    default:
        return;
    }
    Vapse = sqrt(mu * (2 / Rapse - 1 / a));
    if (mode == BURNMODE_PERI || BURNMODE_APO)
    {
        if (inputmode == INPUTMODE_DISTANCE || inputmode == INPUTMODE_PERIOD)
        {

            if (inputmode == INPUTMODE_PERIOD)
            {
                Ptrgt = Period + dPeriod;
                Atrgt = cbrt(mu * pow(Ptrgt, 2) / (4 * pow(PI, 2)));
                dDist = round(fabs(Atrgt - a) * 2 * 1e4) / 1e4;
            }
            else
            {
                Atrgt = a + (dDist == 0 ? 0 : dDist / 2);
                Ptrgt = 2 * PI * sqrt(pow(Atrgt, 3) / mu);
                dPeriod = round(fabs(Ptrgt - Period) * 1e4) / 1e4;
            }

            Vtrgt = sqrt(mu * (2 / Rapse - 1 / Atrgt));
            dv = round(fabs(Vtrgt - Vapse) * 1e7) / 1e7;
        }
        else
        {
            Atrgt = -Rapse * mu / (Rapse * pow(Vapse + dv, 2) - 2 * mu);
            Ptrgt = 2 * PI * sqrt(pow(Atrgt, 3) / mu);
            dDist = round(fabs(Atrgt - a) * 2 * 1e4) / 1e4;
            dPeriod = round(fabs(Ptrgt - Period) * 1e4) / 1e4;
        }
    }
    else
    {
        if (inputmode == INPUTMODE_DISTANCE || inputmode == INPUTMODE_PERIOD)
        {
            if (inputmode == INPUTMODE_PERIOD)
            {

            }
            else
            {

            }
        }
        else
        {
            //Atrgt = -Rapse * mu / (Rapse * pow(Vapse + dv, 2) - 2 * mu);
            //Ptrgt = 2 * PI * sqrt(pow(Atrgt, 3) / mu);
            //Rtrgt = 2 * Atrgt * mu / (Atrgt * pow(Vapse + dv, 2) + mu);
            //dDist = round(fabs(Rtrgt - Rapse) * 1e4) / 1e4;
            //dPeriod = round(fabs(Ptrgt - Period) * 1e4) / 1e4;
        }
    }


}

void MFDDataBurnTime::CalcIBurn(VESSEL* vessel)
{
    BurnTime bt;
  getGroupThrustParm(vessel,groups[Sel_eng],&F,&isp);
//vessel = oapiGetFocusInterface();
  mv = vessel->GetMass();
  ms = bt.GetStackMass(vessel);
  //ms = vessel->GetMass();

  // me = vessel->GetEmptyMass();
  THGROUP_TYPE thgt = groups[Sel_eng];
  THRUSTER_HANDLE th = vessel->GetGroupThruster(thgt,0);
  if (th == NULL)
  {
	  me = 0;
	  IBurn = 0;
	  IBurn2 = 0;
	  return;
  }
  ph = vessel->GetThrusterResource(th);
  //double mvvirt = ms - vessel->GetPropellantMass(ph) + vessel->GetPropellantMaxMass(ph);
  if (ph == NULL)
  {
	  me = 0;
	  IBurn = 0;
	  IBurn2 = 0;
	  return;
  }

  if (includeRCS)
  {
    mrcs = 0;
      if (THRUSTER_HANDLE thRCS = vessel->GetGroupThruster(THGROUP_ATT_PITCHUP,0)) // Any RCS group
        if (PROPELLANT_HANDLE phRCS = vessel->GetThrusterResource(thRCS))
            if (phRCS != ph) // If the selected engine type is not RCS, don't count it twice
                mrcs = vessel->GetPropellantMass(phRCS);
   }
   else
      mrcs = 0;

  double chosenEnginesFuel = vessel->GetPropellantMass(ph);

  mfuel = chosenEnginesFuel + mextra + mrcs;
//if (mdot!=0) TTot=(mv-me)/mdot;
  me = vessel->GetEmptyMass();

  CalcDRadDPeri();

  if (mode==BURNMODE_TGT)
  {
    double dvloc;
    if (dvcurr>dv)
	    dvloc = (dvcurr-dv)/2 + dv;
	else
	    dvloc = (dv-dvcurr)/2 + dvcurr;

    IBurn = RocketEqnT(dvloc,ms,F,isp);
    IBurn2 = RocketEqnT(dvloc/2,ms,F,isp);
  }
  else
  {
    IBurn = RocketEqnT(dv,ms,F,isp);
    IBurn2 = RocketEqnT(dv/2,ms,F,isp);
  }
}

void MFDDataBurnTime::ArmAutoBurn()
{
  switch(mode)
  {
    case BURNMODE_PERI:
      IReference=IPeri;
      break;
    case BURNMODE_APO:
      IReference=IApo;
      break;
    case BURNMODE_MAN:
      IReference=IManual;
      break;
	case BURNMODE_TGT:

	  if (dv == 0)
	  {
		  dv = 0;
		  IReference=0;
		  EReference=0;
	  }
	  else
	  {

	    IReference=(TDist-sOffset)/(dv);
	  }

	break;
		  //HIER

  }
  IsArmed=true;
  EReference=oapiGetSimTime()+IReference;
  ArmAutopilot();
}

DataSourceBase * MFDDataBurnTime::GetCurrentSource() // Can return NULL
{
    if (otherMFDsel < 0)
        return NULL;
    if (otherMFDsel >= int(m_dataSources.size()))
        return NULL;
    return m_dataSources.at(otherMFDsel);
}

void MFDDataBurnTime::ReGetDataFromSource()
{
    DataSourceBase * source = GetCurrentSource();
    if (! source)
        return;
    if (! source->GetFromMM(this))
        return;
    double dvFabs = fabs(source->GetDV());
    if (dvFabs > 1e-5)
    {
        velVector = source->GetVelVec();
        ArmAutopilot();
    }
}

double MFDDataBurnTime::GetTimeToIgnition() const
{
    return EReference-ENow-IBurn2;
}

void MFDDataBurnTime::ArmAutopilot()
{
    if (GetTimeToIgnition() < 350)
    {
        //autopilot.SetTargetVector(velVector);
    }
}

/*
double MFDDataBurnTime::GetStackMass(VESSEL* vessel) {
  //We don't have to worry about attachments. They either
  //are compensated for in the main vessel code, or arent,
  //in which case they have no mass. So, it's only docked
  //vessels which we care about

  //So, what we do is:
//Put the current vessel in the vessel-to-check list
  double totalMass=0;
  VESSEL* vesselsToCheck[100];
  int vesselsStored=0;
  vesselsToCheck[vesselsStored]=vessel;
  vesselsStored++;
//For each vessel in the vessel-to-check list
  for(int vesselsChecked=0;vesselsChecked<vesselsStored && vesselsChecked<100;vesselsChecked++) {
//  Accumulate this vessel's mass
    totalMass+=vesselsToCheck[vesselsChecked]->GetMass();
//  For each docking port
    UINT nDockingPorts=vesselsToCheck[vesselsChecked]->DockCount();
    for(UINT i_dock=0;i_dock<nDockingPorts;i_dock++) {
//    Get the docked vessel, if any,
      DOCKHANDLE hDock=vesselsToCheck[vesselsChecked]->GetDockHandle(i_dock);
      OBJHANDLE hVessel=vesselsToCheck[vesselsChecked]->GetDockStatus(hDock);
      VESSEL* pVessel=NULL;
      if(hVessel) pVessel=oapiGetVesselInterface(hVessel);
//    If it is not already in the vessel-to-check list
      bool hasVesselAlready=(pVessel==NULL);
      for(int i_vessel=0;i_vessel<vesselsStored;i_vessel++) if (vesselsToCheck[i_vessel]==pVessel) hasVesselAlready=true;
      if(!hasVesselAlready) {
//      Add it to the end of the list
        vesselsToCheck[vesselsStored]=pVessel;
        vesselsStored++;
//    end if
      }
//  end for
    }
//end for
  }
  return totalMass;
}
*/
