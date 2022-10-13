#ifndef MFDDATABURNTIME_H
#define MFDDATABURNTIME_H

#include <multipleVesselsMFD/MFDData.hpp>
#include <Orbitersdk.h>
#include <MMExt2_Advanced.hpp>
#include <MMExt2_Basic.hpp>
#include <Orbiter/AutopilotRotation.hpp>
#include <vector>
#include "DataSourceBase.h"

class MFDDataBurnTime : public EnjoLib::MFDGoodies::MFDData
{
public:
  MFDDataBurnTime(VESSEL * vessel);
  virtual ~MFDDataBurnTime();
  void Update();
  void ArmAutoBurn();
  void CalcCircular();
  void CalcApses(VESSEL* vessel);
  void CalcIBurn(VESSEL* vessel);
  //double GetStackMass(VESSEL* vessel);
  DataSourceBase * GetCurrentSource(); // Can return NULL
  void ReGetDataFromSource();
  double GetTimeToIgnition() const;
  void ArmAutopilot();
  void CalcDRadDPeri();
  //double GetTrueAnomaly(double ma);

  static const int numEngines;
  static const THGROUP_TYPE groups[6];
  static const char group_names[6][7];

  VECTOR3 velVector;

  double mu,a,e,IPeri,IApo,Rperi,Rapo,EArmed,IReference,mfuel;

  double mv,ms,me,isp,F,Eff;
  PROPELLANT_HANDLE ph;
  double IBurn,IBurn2,mul;
  int mode; //0 - IBurn-zero=IPeri
            //1 - IBurn-zero=IApo
            //2 - IBurn-zero=manual countdown
            //3 - By Target distance

  int MySoundId;
  int Sel_eng;
  double dv, mextra, mrcs;
  bool IsEngaged,IsArmed,IsCircular;
  double ECutoff,IManual,EReference;
  double TDist;
  double sOffset;

  bool includeRCS;

  int dspunit;  //0 - SI
                //1 - US

  double dvcurr, dvOld;
  double ENow;
  double mdot;
  double mfinal,msfinal,msempty;
  double maxdv;
  double TTot;

  int inputmode;
  int IndexCenterObj;

  const struct BaseSyncExportBurnStruct *BS_burn;
  int BSori;
  int otherMFDsel;
  AutopilotRotation autopilot;

  ELEMENTS el;
  double dDist, dPeriod;
  bool retroBurn;
  //double prev_ma, prev_ea = 0.0;


    std::vector<DataSourceBase*> m_dataSources;

	static const MMExt2::Advanced m_mmextAdv;
	static const MMExt2::Basic	 m_mmextBas;

protected:
private:


};

#endif // MFDDATABURNTIME_H
