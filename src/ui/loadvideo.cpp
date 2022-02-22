///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ui/loadvideo.h"
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////

MyFrame4::MyFrame4(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer(wxVERTICAL);

	m_panel30 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	bSizer30->Add(m_panel30, 1, wxEXPAND | wxALL, 5);

	m_dirPicker1 = new wxDirPickerCtrl(this, wxID_ANY, wxT("C:\\Users\\jhc18\\Desktop"), wxT("Select a folder"), wxDefaultPosition, wxSize(1000, -1), wxDIRP_DEFAULT_STYLE);
	bSizer30->Add(m_dirPicker1, 0, wxALL, 5);

	m_listBox1 = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(1000, 200), 0, NULL, 0);
	bSizer30->Add(m_listBox1, 0, wxALL, 5);

	m_button1 = new wxButton(this, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer30->Add(m_button1, 0, wxALL, 5);

	this->SetSizer(bSizer30);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	m_panel30->Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame4::originImgPaintEvent), NULL, this);
	m_dirPicker1->Connect(wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(MyFrame4::dirclickfuc), NULL, this);
	m_listBox1->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(MyFrame4::listclickfuc), NULL, this);
	m_button1->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame4::audioplayfunc), NULL, this);
}

MyFrame4::~MyFrame4()
{
	// Disconnect Events
	m_panel30->Disconnect(wxEVT_PAINT, wxPaintEventHandler(MyFrame4::originImgPaintEvent), NULL, this);
	m_dirPicker1->Disconnect(wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(MyFrame4::dirclickfuc), NULL, this);
	m_listBox1->Disconnect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(MyFrame4::listclickfuc), NULL, this);
	m_button1->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame4::audioplayfunc), NULL, this);
}

