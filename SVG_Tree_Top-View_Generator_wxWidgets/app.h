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

#include "svg.h" // custom generator

inline wxPoint angularCoordinate(double x0, double y0, unsigned lenght, unsigned angle)
{
    return wxPoint(Cos(0, lenght, angle), Sin(0, lenght, angle));
}

class App : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;

};

class DrawingArea;  // wxPanel with wxPaintDC

class AppFrame : public wxFrame {
public:
    AppFrame(const wxString &title, const wxSize &size);
    ~AppFrame() {};

private:

    enum ID {
        ID_AboutDialog,
        ID_Menu_Redo,
        ID_Menu_Reset,
        ID_Menu_SaveDCsvg,
        ID_Menu_SaveHsvg,
        ID_Menu_Undo,
        ID_wxLabel,
        ID_wxStatuBar,
        ID_wxSlider = 100,
        ID_wxBitmapButton = 500,
        ID_wxColourPickerCtrl = 1000
    };

    wxBitmapButton     *bmpBtn[10];
    wxBoxSizer         *hBox[2];
    wxBoxSizer         *vBox[2];
    wxMenu             *menu[3];
    wxMenuBar          *menuBar;
    wxSlider           *slider[4];
    wxStaticText       *label[4];
    wxStatusBar        *statusBar;
    wxColourPickerCtrl *colourPickerCtrl[2];

    DrawingArea    *drawingArea;

    void OnAbout(wxCommandEvent &event);
    void OnChangeSlider(wxCommandEvent &event);
    void OnChangeColor(wxColourPickerEvent &event);
    void OnBitmapButtonClicked(wxCommandEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnReset(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void OnUndo(wxCommandEvent &event);

    class AboutDialog : public wxDialog {
    public:
        AboutDialog();
        ~AboutDialog() {};

    private:
        const wxString ABOUT = "App to generate SVG tree images in top view.\n\n"
                               "References:\n";
        const wxString ABOUT_HLINK1 = "https://www.wxwidgets.org";
        const wxString ABOUT_HLINK2 = "https://www.w3.org/TR/SVG2";

        wxButton *okBtn;
        wxStaticText *label;
        wxHyperlinkCtrl *hyperlink1, *hyperlink2;
        wxBoxSizer *vBox, *hBox[4];
    };
};

class DrawingArea : public wxPanel {
public:
    DrawingArea(wxFrame *parent);

    bool IsEmpty();
    bool OnSaveSvg(wxString path, wxSize size);
    void DrawShape(wxDC& dc, unsigned shape, wxPoint pos, unsigned lenght, unsigned angle);
    void OnDraw(wxDC& dc);
    void OnMouseClicked(wxMouseEvent &event);
    void OnPaint(wxPaintEvent &event);
    void OnRedo();
    void OnReset();
    void OnSize(wxSizeEvent &event);
    void OnUndo();
    void SetColor(unsigned number, wxColor colorPen, wxColor colorBrush);
    void SetShape(unsigned number);
    void SetValue(unsigned number, unsigned value);

private:
    // Cursor
    wxColor colorCursorPen, colorCursorBrush;
    wxColor colorLinePen, colorLineBrush;
    wxColor colorShapePen, colorShapeBrush;
    wxPoint cursorPosition;
    unsigned cursorRadius;

    // Status
    bool isDrawing;

    // Draw
    struct Path {
        wxPoint begin, end;
        std::vector<wxPoint> points;

        Path(wxPoint point) : begin(point), end(wxPoint(0, 0)), points({point}) {}
    };

    std::vector<Path> path;
    std::vector<Path> bkp;

    unsigned lineTickness;
    unsigned limitLength;
    unsigned shapeAngle;
    unsigned shapeLenght;
    unsigned shape;
};
