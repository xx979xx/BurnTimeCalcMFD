#ifndef GRAPH_H
#define GRAPH_H

#include <orbitersdk.h>

class Graph
{
    public:
        Graph(DWORD xstart, DWORD ystart, DWORD xend, DWORD yend); //Set a window within MFD
        virtual ~Graph();

        double vectorpointdisplay(oapi::Sketchpad *sketchpad, const VECTOR3 &target, VESSEL *vessel) const;

    protected:

    private:
        DWORD ixstart, iystart, ixend, iyend, windowsize;//window parameters
        oapi::Pen * penGrey, * penGreen;
};

#endif // GRAPH_H
