#include "SoundMapBTC.h"
#include "SoundSampleIDEnum.hpp"

SoundMapBTC::SoundMapBTC()
{
    const std::string dir = "Sound/BurnTimeMFDEnjo/";
    AddSample( SOUND_ARM , 			dir + "beep-ui-confirm.wav");
    AddSample( SOUND_SWITCH , 			dir + "beep-short.wav");
}
