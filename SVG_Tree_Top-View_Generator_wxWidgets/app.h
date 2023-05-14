#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if wxUSE_HYPERLINKCTRL
#include <wx/hyperlink.h>
#endif

#if wxUSE_FILE
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#endif

#include <wx/clrpicker.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "drawingArea.h"

class App : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;

};

class AppFrame : public wxFrame {
public:
    AppFrame(const wxString &title, const wxSize &size);
    ~AppFrame() {};

private:

    enum ID {
        ID_AboutDialog,
        ID_DrawingArea,
        ID_Menu_New,
        ID_Menu_Redo,
        ID_Menu_Reset,
        ID_Menu_Save,
        ID_Menu_SaveDCsvg,
        ID_Menu_SaveHsvg,
        ID_Menu_SaveTxt,
        ID_Menu_Undo,
        ID_chkBox_Random,
        ID_chkBox_Spline,
        ID_wxLabel,
        ID_wxStatuBar,
        ID_wxSlider = 100,
        ID_wxBitmapButton = 500,
        ID_wxColourPickerCtrl = 1000
    };

    DrawingArea    *drawingArea;
    unsigned       currentShape;

    wxBitmapButton     *bmpBtn[10];
    wxBoxSizer         *hBox[3];
    wxBoxSizer         *vBox[2];
    wxCheckBox         *checkBox[2];
    wxColourPickerCtrl *colorPCtrl[3];
    wxMenu             *menu[3];
    wxMenuBar          *menuBar;
    wxSlider           *slider[4];
    wxStaticText       *label[4];
    wxStatusBar        *statusBar;

    void OnKeyDown(wxKeyEvent &event);
    void OnNew(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void Reset();

    class AboutDialog : public wxDialog {
    public:
        AboutDialog();
        ~AboutDialog(){};

    private:
        const wxString ABOUT = "App to generate SVG tree images in top view.\n\n"
                               "References:\n";
        const wxString ABOUT_HLINK1 = "https://www.wxwidgets.org";
        const wxString ABOUT_HLINK2 = "https://www.w3.org/TR/SVG2";

        wxBoxSizer *vBox, *hBox[4];
        wxButton *okBtn;
        wxHyperlinkCtrl *hyperlink1, *hyperlink2;
        wxStaticText *label;
    };

    class NewDialog : public wxDialog {
    public:
        NewDialog(wxSize drawingAreaSize);
        ~NewDialog(){};

        wxSize GetSize();

    private:

        enum ID {
            ID_Text_width,
            ID_Text_height
        };

        wxBoxSizer *vBox, *hBox[4];
        wxButton *okBtn;
        wxStaticText *label[3];
        wxTextCtrl *width, *height;
    };
};

