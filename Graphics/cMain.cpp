#include "cMain.h"
wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_BUTTON(10001, OnButtonClicked)
wxEND_EVENT_TABLE()
cMain::cMain() : wxFrame(NULL, wxID_ANY, "Installation", wxPoint(30,30), wxSize(800, 500)) // creates the window
{	// creates the button
	button = new wxButton(this, 10001, "Click here to finish installation", wxPoint(10, 10), wxSize(150, 50));
}

cMain::~cMain()
{
}

void cMain::OnButtonClicked(wxCommandEvent& evt)
{
	this->Close();
	evt.Skip();
}