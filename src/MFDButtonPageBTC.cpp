#include "MFDButtonPageBTC.h"

MFDButtonPageBTC::MFDButtonPageBTC()
{
    static const MFDBUTTONMENU mnu1[] = {
        {"Switch buttons page", 0, 'P'},
        {"Enter target", "or deltaV", 'V'},
        {"Enter time", "to Maneuver", 'T'},
        {"Reset MFD", 0, 'R' },
        {"Change mode", 0, 'M'},
        {"Burn Now", 0, 'B'},

        {"Arm Autoburn", 0, 'A'},
        {"Arm Autocirc", 0, 'C'},
        {"Select engine", 0, 'E'},
        {"Get from BS/TX", 0, 'G'},
        {"Enter extra fuel", 0, 'X'},
        {"Include RCS fuel", 0, 'I'}
    };
    RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

    RegisterFunction("PG",  OAPI_KEY_P, &BurnTimeMFD::HandlerSwitchButtonPage);
    RegisterFunction("DV",  OAPI_KEY_V, &BurnTimeMFD::HandlerTargetOrDV);
    RegisterFunction("DT",  OAPI_KEY_T, &BurnTimeMFD::HandlerTimeOfManoeuvre);
    RegisterFunction("RST", OAPI_KEY_R, &BurnTimeMFD::HandlerReset);
    RegisterFunction("MD",  OAPI_KEY_M, &BurnTimeMFD::HandlerChangeMode);
    RegisterFunction("BRN", OAPI_KEY_B, &BurnTimeMFD::HandlerBurnNow);

    RegisterFunction("ARM", OAPI_KEY_A, &BurnTimeMFD::HandlerAutoBurn);
    RegisterFunction("CIR", OAPI_KEY_C, &BurnTimeMFD::HandlerAimAutoCirc);
    RegisterFunction("ENG", OAPI_KEY_E, &BurnTimeMFD::HandlerSelectEngine);
    RegisterFunction("GET", OAPI_KEY_G, &BurnTimeMFD::HandlerGetFromOtherMFD);
    RegisterFunction("EXT", OAPI_KEY_X, &BurnTimeMFD::HandlerEnterExtraFuel);
    RegisterFunction("RCS", OAPI_KEY_I, &BurnTimeMFD::HandlerIncludeRCSFuel);

// {"DV","DT","OS","ST","RST", "MD", "ARM","BRN", "CIR","UNT","ENG","EXT"};
    static const MFDBUTTONMENU mnu2[] = {
        {"Switch buttons page", 0, 'B'},
        {"Set Target for", "distance calculation", 'S'},
        {"Enter offset distance", "to target", 'O'},
        {"Switch SI / US", "unit system", 'U'},
        {"Enable rot. AP", "", 'A'},
        {"Disable rot. AP", "", 'D'}
    };
    RegisterPage(mnu2, sizeof(mnu2) / sizeof(MFDBUTTONMENU));

    RegisterFunction("PG",  OAPI_KEY_P, &BurnTimeMFD::HandlerSwitchButtonPage);
    RegisterFunction("ST",  OAPI_KEY_S, &BurnTimeMFD::HandlerTargetForDistanceCalc);
    RegisterFunction("OS",  OAPI_KEY_O, &BurnTimeMFD::HandlerOffsetDistance);
    RegisterFunction("UNT", OAPI_KEY_U, &BurnTimeMFD::HandlerSwitchSI_US);
    //RegisterFunction("AP",  OAPI_KEY_A, &BurnTimeMFD::HandlerAutopilot);
    //RegisterFunction("DAP", OAPI_KEY_D, &BurnTimeMFD::HandlerAutopilotDisable);

}

MFDButtonPageBTC::~MFDButtonPageBTC()
{}

bool MFDButtonPageBTC::SearchForKeysInOtherPages() const
{
    return true;
}

