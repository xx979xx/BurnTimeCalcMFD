#ifndef PLUGINBURNTIME_H
#define PLUGINBURNTIME_H

#include <multipleVesselsMFD/PluginMultipleVessels.hpp>


class PluginBurnTime : public EnjoLib::MFDGoodies::PluginMultipleVessels
{
    public:
        PluginBurnTime(HINSTANCE hDLL);
        virtual ~PluginBurnTime();
    protected:
        void UpdateClientPreStep( EnjoLib::MFDGoodies::MFDData * data, const EnjoLib::MFDGoodies::SimulationTimeData & timeData );
        void UpdateClientPostStep( EnjoLib::MFDGoodies::MFDData * data, const EnjoLib::MFDGoodies::SimulationTimeData & timeData );
        void InitClient();
        void CleanupClient();
        EnjoLib::MFDGoodies::MFDData * ConstructNewMFDData( VESSEL * vessel );

    private:
};

#endif // PLUGINBURNTIME_H
