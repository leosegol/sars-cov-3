#pragma once

#include "wx/wx.h"
#include "cMain.h"

class cApp: public wxApp
{

private:
	cMain* frame1 = NULL;
public:
	cApp();
	~cApp();
	virtual bool OnInit();
};

