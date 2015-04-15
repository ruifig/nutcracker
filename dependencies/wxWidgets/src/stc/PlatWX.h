

// RVF +
#ifndef wxOVERRIDE
	#define wxOVERRIDE override
#endif
#ifndef wxFALLTHROUGH
	#define wxFALLTHROUGH
#endif
// RVF -

wxRect wxRectFromPRectangle(PRectangle prc);
PRectangle PRectangleFromwxRect(wxRect rc);
wxColour wxColourFromCD(const ColourDesired& ca);

