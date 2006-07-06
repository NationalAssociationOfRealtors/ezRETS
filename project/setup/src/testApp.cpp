#include <wx/wx.h>
#include "DataSource.h"
#include "SetupDialog.h"
#include "SetupLog.h"
#include "str_stream.h"

#ifdef __WXMAC__
#include "ezretss.xpm"
#endif

using namespace odbcrets;

class TestApp : public wxApp
{
  public:
    virtual bool OnInit();
};

class TestFrame: public wxFrame
{
  public:
    TestFrame(const wxString & title);

    void OnQuit(wxCommandEvent & event);
    void OnEdit(wxCommandEvent & event);

  private:
    static const long ID_QUIT;
    static const long ID_EDIT;

    DECLARE_EVENT_TABLE();
};

IMPLEMENT_APP(TestApp);

bool TestApp::OnInit()
{
    TestFrame * frame = new TestFrame("Test Setup");
    frame->Show();

    return true;
}

const long TestFrame::ID_QUIT = wxID_EXIT;
const long TestFrame::ID_EDIT = wxNewId();

BEGIN_EVENT_TABLE(TestFrame, wxFrame)
    EVT_MENU(ID_QUIT, TestFrame::OnQuit)
    EVT_MENU(ID_EDIT, TestFrame::OnEdit)
END_EVENT_TABLE();

TestFrame::TestFrame(const wxString & title)
    : wxFrame(NULL, wxID_ANY, title)
{
    SetIcon(wxICON(ezretss));
    wxMenu * fileMenu = new wxMenu();
    fileMenu->Append(ID_QUIT, "E&xit\tAlt-X", "Quit this program");
    fileMenu->Append(ID_EDIT, "Edit &Data Source\tCtrl-D", "Edit a data source");
    wxMenuBar * menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");

    SetMenuBar(menuBar);
    CenterOnParent();
}

void TestFrame::OnQuit(wxCommandEvent & event)
{
    Close(true);
}

void TestFrame::OnEdit(wxCommandEvent & event)
{
    wxString name = wxGetTextFromUser("Data Source Name:", "Edit Data Source",
                                      "", this);
    DataSourcePtr dataSource(new DataSource(name.c_str()));
    dataSource->MergeFromIni();
    slog.debug(str_stream() << "Original DataSource: " << dataSource);
    DataSourcePtr newDataSource(new DataSource(*dataSource));
    SetupDialog setupDialog(dataSource,
                            this, _T("ezRETS ODBC Setup"));
    int rc = setupDialog.ShowModal();
    if (rc != wxID_OK)
    {
        return;
    }
        
    slog.debug(str_stream() << "New DataSource: " << dataSource);
}
