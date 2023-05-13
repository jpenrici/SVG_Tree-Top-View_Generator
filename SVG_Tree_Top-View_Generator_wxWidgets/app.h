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
        ID_wxCheckBox,
        ID_wxLabel,
        ID_wxStatuBar,
        ID_wxSlider = 100,
        ID_wxBitmapButton = 500,
        ID_wxColourPickerCtrl = 1000
    };

    DrawingArea    *drawingArea;

    wxBitmapButton     *bmpBtn[10];
    wxBoxSizer         *hBox[3];
    wxBoxSizer         *vBox[2];
    wxCheckBox         *checkBox;
    wxColourPickerCtrl *colorPCtrl[3];
    wxMenu             *menu[3];
    wxMenuBar          *menuBar;
    wxSlider           *slider[4];
    wxStaticText       *label[4];
    wxStatusBar        *statusBar;

    void OnAbout(wxCommandEvent &event);
    void OnBitmapButtonClicked(wxCommandEvent &event);
    void OnChangeColor(wxColourPickerEvent &event);
    void OnChangeSlider(wxCommandEvent &event);
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

        wxBoxSizer *vBox, *hBox[4];
        wxButton *okBtn;
        wxHyperlinkCtrl *hyperlink1, *hyperlink2;
        wxStaticText *label;
    };
};

class DrawingArea : public wxPanel {
public:
    DrawingArea(wxFrame *parent);

    bool IsEmpty();
    bool OnSaveSvg(wxString path, wxSize size);
    bool OnSaveSvg2(wxString path, wxSize size);
    void IsSpline(bool isSpline = false);
    void OnRedo();
    void OnReset();
    void OnUndo();
    void SetColor(unsigned number, wxColour colorPen, wxColour colorBrush);
    void SetShape(unsigned number);
    void SetValue(unsigned number, unsigned value);

private:
    // Cursor
    wxColour colorCursorPen, colorCursorBrush;
    wxColour colorLinePen, colorLineBrush;
    wxColour colorShapePen, colorShapeBrush;
    wxPoint cursorPosition;
    unsigned cursorRadius;

    // Status
    bool isDrawing;

    // Draw
    struct Shape {
        wxColour pen, brush;
        std::vector<wxPoint> points;

        Shape(wxColour pen, wxColour brush, std::vector<wxPoint> points)
            : pen(pen), brush(brush), points(points) {}
    };

    struct Path {
        wxPoint begin, end;
        std::vector<wxPoint> points;

        Path(wxPoint point) : begin(point), end(point), points({point}) {}
    };

    std::vector<Shape> shapes;
    std::vector<Path> path;
    std::vector<Path> bkp;

    unsigned lineTickness;
    unsigned limitLength;
    unsigned shapeAngle;
    unsigned shapeLenght;
    unsigned shape;
    bool isSpline;

    void OnDraw(wxDC& dc);
    void OnMouseClicked(wxMouseEvent &event);
    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);
    std::vector<wxPoint> GetPoints(unsigned shape, wxPoint pos, unsigned lenght, unsigned angle);
};
