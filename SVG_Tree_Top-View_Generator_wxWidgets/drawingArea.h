#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "svg.h"    // custom generator

class DrawingArea : public wxPanel {
public:
    DrawingArea(wxFrame *parent, int id, wxPoint position, wxSize size);

    bool IsEmpty();
    bool OnSaveSvg(wxString path, SVG::Metadata metadata);
    bool OnSaveSvgDC(wxString path);
    bool OnSaveTxT(wxString path);
    bool Resize(wxSize size, bool reset = true);

    unsigned GetValue(unsigned number);

    void BreakPath();
    void OnMouseClicked(wxMouseEvent &event);
    void OnRedo();
    void OnReset();
    void OnUndo();
    void SetColor(unsigned number, wxColour colorPen, wxColour colorBrush);
    void SetRandomColor(wxColour color1 = wxColour(0, 0, 0, 255), wxColour color2 = wxColour(0, 0, 0, 255));
    void SetShape(unsigned number, bool all = false);
    void SetStyle(bool isSpline = false);
    void SetValue(unsigned number, unsigned value, bool all = false);

private:
    // Cursor
    wxColour colorBorderPen, colorBorderBrush;
    wxColour colorCursorPen, colorCursorBrush;
    wxColour colorLinePen, colorLineBrush;
    wxColour colorShapePen, colorShapeBrush;
    wxColour minColorShapeBrush, maxColorShapeBrush;
    wxPoint cursorPosition;

    unsigned cursorRadius;

    bool randomColorShapeBrush;

    // Status
    bool isDrawing;
    wxSize maxSize;
    wxSize currentSize;

    // Draw
    struct Shape {
        wxString name = "";
        wxColour pen = wxColour(0, 0, 0, 255);
        wxColour brush = wxColour(255, 255, 255, 255);
        unsigned lineWidth = 1;
        std::vector<wxPoint> points;

        Shape() {}
        Shape(wxString name, wxColour pen, wxColour brush, unsigned lineWidth,
              std::vector<wxPoint> points)
            : name(name), pen(pen), brush(brush), lineWidth(lineWidth), points(points) {}
    };

    struct Path {
        unsigned limitLength;
        unsigned shapeAngle;
        unsigned shapeLenght;
        unsigned shapeNumber;
        wxPoint begin, end;
        std::vector<wxPoint> points;

        Path(wxPoint point)
            : begin(point), end(point), points({point}), limitLength(0), shapeAngle(0), shapeLenght(0), shapeNumber(0) {}
        Path(wxPoint point, unsigned shapeNumber, unsigned shapeAngle = 0, unsigned shapeLenght = 0, unsigned limitLength = 0)
            : begin(point), end(point), points({point}), shapeAngle(shapeAngle), shapeLenght(shapeLenght), shapeNumber(shapeNumber), limitLength(limitLength) {}
    };

    std::vector<Path> bkp;
    std::vector<Path> path;
    std::vector<Shape> shapes;

    bool isSpline;
    bool breakPath;

    unsigned limitLength;
    unsigned lineWidth;
    unsigned panelBorder;
    unsigned shapeAngle;
    unsigned shapeLenght;
    unsigned shapeNumber;

    void OnDraw(wxDC &dc);
    void OnPaint(wxPaintEvent &event);
    void OnUpdate();

    std::vector<wxPoint> GetPoints(unsigned shape, wxPoint pos, unsigned lenght, unsigned angle);
};
