#include "Graph.h"
#include <Orbiter/SpaceMathOrbiter.hpp>


Graph::Graph(DWORD xstart, DWORD ystart, DWORD xend, DWORD yend)
: ixstart(xstart)
, iystart(ystart)
, ixend(xend)
, iyend(yend)
, windowsize(ixend-ixstart)
{
	DWORD temp=iyend-iystart;
	if (temp<windowsize) windowsize=temp;

	penGrey = oapiCreatePen(1, 1, RGB(0x50, 0x50, 0x50));
	penGreen = oapiCreatePen(1, 1, RGB(0x00, 0xEE, 0x00));

}

Graph::~Graph()
{
    oapiReleasePen(penGrey);
    oapiReleasePen(penGreen);
}

double Graph::vectorpointdisplay(oapi::Sketchpad *sketchpad, const VECTOR3 &target, VESSEL *vessel) const
//targetvector is a vector in the global reference plane
{
    if (length(target) == 0)
        return 0;
	//TransXFunction::SelectDefaultPen(sketchpad,TransXFunction::Grey);
	sketchpad->SetPen(penGrey);

	const int rings = 3;
	const int width = (ixend - ixstart),
			  height = (iyend - iystart);
	const double edgeBorderSize = 0.9;
	for(int i = 1; i <= rings; i++)
	{
		// Must move to the righthand side of the circle to draw
		double radius = i * width * edgeBorderSize / 2 / rings;
		sketchpad->Ellipse(int(width/2 - radius),
							int(height/2 - radius),
							int(width/2 + radius),
							int(height/2 + radius));
	}

    VECTOR3 trtarget = EnjoLib::SpaceMathOrbiter().GetRotationToTarget(vessel, target);
	// Draw the horizontal and vertical lines across the target circles
	sketchpad->MoveTo(int(width * (1 - edgeBorderSize) / 2), height / 2);
	sketchpad->LineTo(int(width * (1 + edgeBorderSize) / 2), height / 2);
	sketchpad->MoveTo(width / 2, int(height * (1 - edgeBorderSize) / 2));
	sketchpad->LineTo(width / 2, int(height * (1 + edgeBorderSize) / 2));

	// Draw the target cross
	double offsetsize = sqrt(trtarget.x * trtarget.x + trtarget.y * trtarget.y);
	double scalar = sqrt(0.5);
	double xang = trtarget.x / offsetsize;
	double yang =- trtarget.y / offsetsize;
	//sprintf(oapiDebugString(), "x = %.2lf, y = %.2lf", xang, yang);
	offsetsize = sqrt(offsetsize / trtarget.z);
	if (offsetsize > scalar || trtarget.z < 0)
		offsetsize = scalar;
	offsetsize = offsetsize / scalar * windowsize * edgeBorderSize / 2;
	int xpos = int(offsetsize * xang + width / 2 + ixstart);
	int ypos = int(offsetsize * yang + height / 2 + iystart);

	sketchpad->SetPen(penGreen);

	const int crossSize = 5;
	sketchpad->MoveTo(xpos - crossSize, ypos - crossSize);
	sketchpad->LineTo(xpos + crossSize, ypos + crossSize);
	sketchpad->MoveTo(xpos - crossSize, ypos + crossSize);
	sketchpad->LineTo(xpos + crossSize, ypos - crossSize);
	return length(trtarget);
}
