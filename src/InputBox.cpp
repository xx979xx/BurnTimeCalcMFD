#include "InputBox.h"
#include "BurnTimeMFD.h"
#include "MFDDataBurnTime.h"
#include "globals.h"
#include "Comms.h"
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

bool ObjectInput (void *id, char *str, void *usrdata)
{
    if ( str[0] == 0) return false;
	BurnTimeMFD * btcMFD = (BurnTimeMFD*)usrdata;
    MFDDataBurnTime * data = btcMFD->GetData();

	if (data->inputmode == INPUTMODE_MFDSEL )
    {
        if (Comms().ParseInput(str, data))
            btcMFD->HandlerAutoBurn();
        return true;
    }


	std::string SearchString(str);
	OBJHANDLE hFObj = oapiGetObjectByName(str);
	//Auslesen des letzten Zeichens und Bestimmen des Expondentens (read last char to check for exponent letter)
	std::string InputUnits =  "yzafpnum_kMGTPEZY;";
	std::string RightChar = std::string(SearchString.substr(SearchString.length()-1,SearchString.length()));
	std::string numbers = "0123456789;";
	//Überprüfe ob der String eine Zahl ist... (check if the string is a number)


	if (data->inputmode != INPUTMODE_TARGET )
	{
		for (unsigned int laufNumber=0; laufNumber<SearchString.length()-1;laufNumber++)
		{
			if
			(
				SearchString.substr(laufNumber,1) != "." &&
				SearchString.substr(laufNumber,1) != "0" &&
				SearchString.substr(laufNumber,1) != "1" &&
				SearchString.substr(laufNumber,1) != "2" &&
				SearchString.substr(laufNumber,1) != "3" &&
				SearchString.substr(laufNumber,1) != "4" &&
				SearchString.substr(laufNumber,1) != "5" &&
				SearchString.substr(laufNumber,1) != "6" &&
				SearchString.substr(laufNumber,1) != "7" &&
				SearchString.substr(laufNumber,1) != "8" &&
				SearchString.substr(laufNumber,1) != "9" &&
				(
					SearchString.substr(laufNumber, 1) != "-" || laufNumber != 0 || data->inputmode != INPUTMODE_DV &&
                    data->inputmode != INPUTMODE_DISTANCE && data->inputmode != INPUTMODE_PERIOD
				)
			) return false;
		}
		double MyExp=-24;
		int lauf = 0 ;
		while (InputUnits.substr(lauf,1)!=";")
		{
			if (InputUnits.substr(lauf,1) == RightChar.substr(0,1)) break;
			lauf++;
			MyExp +=3;
		}
		// Wenn kein Einheitenfaktor gefunden wurde...  (if an exponent factor is found)
		if (InputUnits.substr(lauf,1)==";")
		{
			if (RightChar != "0" && atoi(RightChar.c_str()) == 0) return false;
			if (data->inputmode==INPUTMODE_OFFSET )
			{
				data->sOffset = atof(SearchString.c_str());
				if (data->dspunit == 1) data->sOffset = data->sOffset  * 0.3048;
			}
			else if (data->inputmode==INPUTMODE_DV )
			{
				data->dv = atof(SearchString.c_str());
				if (data->dspunit == 1) data->dv = data->dv * 0.3048;
				if (data->dv < 0) {
					data->dv = fabs(data->dv);
					data->retroBurn = true;
				}
				else {
					data->retroBurn = false;
				}
			}
			else if (data->inputmode==INPUTMODE_EXTRA )
			{
				data->mextra = atof(SearchString.c_str());
				if (data->dspunit == 1) data->mextra = data->mextra * 0.45359237;
			}
			else if (data->inputmode==INPUTMODE_TIME )
			{
				data->EReference = oapiGetSimTime()+atof(SearchString.c_str());
				data->IManual = atof(SearchString.c_str());
			}
			else if (data->inputmode == INPUTMODE_DISTANCE)
			{
				data->dDist = atof(SearchString.c_str());
			}
			else if (data->inputmode == INPUTMODE_PERIOD)
			{
				data->dPeriod = atof(SearchString.c_str());
			}
		}
		else
		{
			if (data->inputmode==INPUTMODE_EXTRA )
			{
				data->mextra = atof(SearchString.c_str());
				if (data->dspunit == 1) data->mextra = data->mextra * 0.45359237;
			}
			if (data->inputmode==INPUTMODE_OFFSET )
			{
				data->sOffset = atof(SearchString.c_str()) * pow(10,MyExp);
				if (data->dspunit == 1) data->sOffset = data->sOffset * 0.3048;
			}
			else if (data->inputmode==INPUTMODE_DV )
			{
				data->dv = atof(SearchString.c_str()) * pow(10,MyExp);
				if (data->dspunit == 1) data->dv = data->dv * 0.3048;
				if (data->dv < 0) {
					data->dv = fabs(data->dv);
					data->retroBurn = true;
				}
				else {
					data->retroBurn = false;
				}
			}
			else if (data->inputmode == INPUTMODE_DISTANCE)
			{
				data->dDist = atof(SearchString.c_str()) * pow(10, MyExp);
			}
			else if (data->inputmode == INPUTMODE_PERIOD)
			{
				data->dPeriod = atof(SearchString.c_str()) * pow(10, MyExp);
			}
			else data->IManual = atof(SearchString.c_str()) * pow(10,MyExp);
			//sprintf(oapiDebugString(),"v1:%f  v2:%f ",atof(SearchString.c_str()),pow(10,MyExp));
		}
		if (data->inputmode == INPUTMODE_DISTANCE || data->inputmode == INPUTMODE_PERIOD)
		{
		}
		else
		{
			data->inputmode = INPUTMODE_NONE;
		}
		return true;
	}
	else
	{
		// Checking if string is equal
		std::transform(SearchString.begin(), SearchString.end(), SearchString.begin(), std::tolower);
		for (unsigned int i=0;i<oapiGetObjectCount();i++)
		{
			OBJHANDLE hSObj = oapiGetObjectByIndex(i);
			char name[255];
			oapiGetObjectName(hSObj,name,255);
			std::string strname(name);
			std::transform(strname.begin(), strname.end(), strname.begin(), std::tolower);
			if (strname == SearchString)
			{
			   data->IndexCenterObj = i;
			   data->inputmode=INPUTMODE_NONE;
			   return true;
			}
		}
		//checking if string is a part of the entered value
		for (unsigned int i = 0; i < oapiGetObjectCount();i++)
		{
			char ObjectNameCharStr[255];
			OBJHANDLE hobj = oapiGetObjectByIndex(i);
			oapiGetObjectName (hobj,ObjectNameCharStr,255);
			std::string StrObjectName(ObjectNameCharStr);
			std::transform(StrObjectName.begin(), StrObjectName.end(), StrObjectName.begin(), std::tolower);
			size_t pos;

			pos = StrObjectName.find(SearchString);
			if (pos != std::string::npos)
			{
			   data->IndexCenterObj = i;
			   data->inputmode=INPUTMODE_NONE;
			   return true;
			}
		}
		return false;
	}
}
