#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{
}

cApp::~cApp()
{
}

bool cApp::OnInit()
{
	frame1 = new cMain();
	frame1->Show();

	/*wxProgressDialog* dialog = new wxProgressDialog(wxT("Wait..."), wxT("Keep waiting..."), max, frame, wxPD_AUTO_HIDE | wxPD_APP_MODAL);

	for (int i = 0; i < max; i++) {
		wxMilliSleep(5); //here are computations
		if (i % 23) dialog->Update(i);
	}
	dialog->Update(max);
	delete dialog;
	*/
	return true;
}
