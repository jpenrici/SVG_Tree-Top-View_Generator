#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class DrawingArea : public wxPanel {
public:
    DrawingArea(wxFrame *parent, int id, wxPoint position, wxSize size);

    bool IsEmpty();
    bool OnSaveSvg(wxString path);
    bool OnSaveSvgDC(wxString path);
    bool OnSaveTxT(wxString path);
    bool Resize(wxSize size);

    unsigned GetValue(unsigned number);

    void OnMouseClicked(wxMouseEvent &event);
    void OnRedo();
    void OnReset();
    void OnUndo();
    void SetColor(unsigned number, wxColour colorPen, wxColour colorBrush);
    void SetRandomColor(wxColour color1 = wxColour(0, 0, 0, 255), wxColour color2 = wxColour(0, 0, 0, 255));
    void SetShape(unsigned number);
    void SetStyle(bool isSpline = false);
    void SetValue(unsigned number, unsigned value);

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
        wxString name;
        wxColour pen, brush;
        unsigned lineWidth;
        std::vector<wxPoint> points;

        Shape(wxString name, wxColour pen, wxColour brush, unsigned lineWidth,
              std::vector<wxPoint> points)
            : name(name), pen(pen), brush(brush), lineWidth(lineWidth),
            points(points) {}
    };

    struct Path {
        wxPoint begin, end;
        std::vector<wxPoint> points;

        Path(wxPoint point) : begin(point), end(point), points({point}) {}
    };

    std::vector<Path> bkp;
    std::vector<Path> path;
    std::vector<Shape> shapes;

    bool isSpline;

    unsigned limitLength;
    unsigned lineWidth;
    unsigned panelBorder;
    unsigned shape;
    unsigned shapeAngle;
    unsigned shapeLenght;

    void OnDraw(wxDC &dc);
    void OnPaint(wxPaintEvent &event);
    void OnUpdate();

    std::vector<wxPoint> GetPoints(unsigned shape, wxPoint pos, unsigned lenght, unsigned angle);
};
