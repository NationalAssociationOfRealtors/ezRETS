#include "TextValueSizer.h"

using namespace odbcrets;

TextValueSizer::TextValueSizer(wxWindow * parent)
    : wxFlexGridSizer(2)
{
    mParent = parent;
    AddGrowableCol(1);
    mNumRows = 0;
}

wxSizerFlags TextValueSizer::DefaultValueFlags() const
{
    wxSizerFlags valueFlags(1);
    valueFlags.Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT).Expand();
    return valueFlags;
}

void TextValueSizer::AddRow(const wxString & text, wxWindow * value)
{
    AddRow(text, value, DefaultValueFlags());
}

void TextValueSizer::AddRow(const wxString & text, wxWindow * value,
                            wxSizerFlags valueFlags)
{
    wxSizerFlags textFlags(0);
    textFlags.Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);

    if (mNumRows == 0)
    {
        textFlags.Border(wxRIGHT, 10);
    }
    else
    {
        textFlags.Border(wxTOP | wxRIGHT, 10);
        valueFlags.Border(wxTOP, 10);
    }

    Add(new wxStaticText(mParent, wxID_ANY, text), textFlags);
    Add(value, valueFlags);
    mNumRows++;
}
