#include "drawingArea.h"    // wxPanel
#include "svg.h"            // custom generator

#include "wx/dcsvg.h"

inline wxPoint angularCoordinate(double x, double y, unsigned lenght, unsigned angle)
{
    // Origin (x,y)
    return wxPoint(Cos(x, lenght, angle), Sin(y, lenght, angle));
}

inline wxPoint angularCoordinate(unsigned lenght, unsigned angle)
{
    // Origin (0,0)
    return wxPoint(Cos(0, lenght, angle), Sin(0, lenght, angle));
}

DrawingArea::DrawingArea(wxFrame *parent, int id, wxPoint position, wxSize size)
    : wxPanel(parent, id, position, size)
{
    // Cursor
    cursorRadius = 5;
    cursorPosition = wxPoint(0, 0);

    // Colors
    colorBorderBrush = wxColour(0, 0, 0, 0);
    colorBorderPen = wxColour(255, 0, 0, 255);
    colorCursorBrush = wxColour(0, 0, 0, 255);
    colorCursorPen = wxColour(0, 0, 0, 255);
    colorLineBrush = wxColour(0, 0, 0, 255);
    colorLinePen = wxColour(0, 0, 0, 255);
    colorShapeBrush = wxColour(0, 0, 0, 255);
    colorShapePen = wxColour(0, 0, 0, 255);
    minColorShapeBrush = colorShapeBrush;
    maxColorShapeBrush = colorShapeBrush;
    randomColorShapeBrush = false;

    // State of the drawing pen
    isDrawing = true;
    path.clear();

    // Draw
    isSpline = false;
    limitLength = 20;
    lineTickness = 10;
    panelBorder = 20;
    shapeAngle = 60;
    shapeLenght = 50;

    // Handlers
    Bind(wxEVT_LEFT_DOWN, &DrawingArea::OnMouseClicked, this, id);
    Bind(wxEVT_MOTION, &DrawingArea::OnMouseClicked, this, id);
    Bind(wxEVT_PAINT, &DrawingArea::OnPaint, this, id);
    Bind(wxEVT_SIZE, [ = ](wxSizeEvent &){ Refresh(); }, id);
}

void DrawingArea::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
    OnDraw(dc);
}

void DrawingArea::OnDraw(wxDC &dc)
{
    if (isDrawing) {
        // Cursor
        dc.SetPen(colorCursorPen);
        dc.SetBrush(colorCursorBrush);
        dc.DrawCircle(cursorPosition.x, cursorPosition.y, cursorRadius);
        // Border
        dc.SetPen(colorBorderPen);
        dc.SetBrush(colorBorderBrush);
        dc.DrawRectangle(panelBorder, panelBorder, GetSize().x - 2 * panelBorder, GetSize().y - 2 * panelBorder);
    }

    for(auto& shape : shapes) {
        dc.SetPen(wxPen(shape.pen, shape.tickness));
        dc.SetBrush(shape.brush);
        if (isSpline) {
            dc.DrawSpline(shape.points.size(), &shape.points[0]);
        } else {
            if (shape.name == "Line") {
                dc.DrawLines(shape.points.size(), &shape.points[0]);
            } else {
                dc.DrawPolygon(shape.points.size(), &shape.points[0]);
            }
        }
    }
};

void DrawingArea::OnUpdate()
{
    shapes.clear();
    std::vector<wxPoint> pointsLine;

    wxPoint pos;
    for (auto &line : path) {
        pos = line.end;
        for (unsigned i = line.points.size() - 1; i > 0; i--) {
            auto lenght = Distance(pos.x, pos.y, line.points[i].x, line.points[i].y);
            if (lenght > limitLength) {
                auto sectionAngle = LineAngle(pos.x, pos.y, line.points[i].x, line.points[i].y);
                if (randomColorShapeBrush) {
                    unsigned r = maxColorShapeBrush.Red() - minColorShapeBrush.Red();
                    unsigned g = maxColorShapeBrush.Green() - minColorShapeBrush.Green();
                    unsigned b = maxColorShapeBrush.Blue() - minColorShapeBrush.Blue();
                    r = r > 0 ? rand() % r : 0;
                    g = g > 0 ? rand() % g : 0;
                    b = b > 0 ? rand() % b : 0;
                    colorShapeBrush = wxColour((minColorShapeBrush.Red() + r) % 255,
                                               (minColorShapeBrush.Green() + g) % 255,
                                               (minColorShapeBrush.Blue() + b) % 255);
                }
                for (auto& signal : {-1, 1}) {
                    auto angle = sectionAngle + signal * shapeAngle;
                    auto points = GetPoints(shape, pos + angularCoordinate(lineTickness, angle), shapeLenght, angle);
                    shapes.push_back(Shape(isSpline ? "Spline" : "Polygon", colorShapePen, colorShapeBrush, 1, points));
                }
                pos = line.points[i];
                pointsLine.push_back(pos);
            }
        }
        pointsLine.push_back(line.begin);
        if (pointsLine.size() > 2 && lineTickness > 0) {
            shapes.push_back(Shape("Line", colorLinePen, colorLineBrush, lineTickness, pointsLine));
        }
        pointsLine.clear();
    }
}

void DrawingArea::OnMouseClicked(wxMouseEvent &event)
{
    cursorPosition = ScreenToClient(::wxGetMousePosition());
    if (cursorPosition.x > panelBorder && cursorPosition.x < GetSize().x - panelBorder &&
        cursorPosition.y > panelBorder && cursorPosition.y < GetSize().y - panelBorder) {
        if (event.LeftDown()) {
            // Close line
            if (!path.empty()) {
                path.back().end = path.back().points.back();
            }
            // Open new line
            isDrawing = true;
            path.push_back(Path(cursorPosition));
        }
        if (isDrawing && event.LeftIsDown()) {
            // Lines
            if (!path.empty()) {
                path.back().points.push_back(cursorPosition);
            } else {
                path.push_back(Path(cursorPosition));
            }
            OnUpdate();
        } else {
            isDrawing = false;
        }
        if (event.Moving() && !randomColorShapeBrush) {
            randomColorShapeBrush = false;
        }
    }
    Refresh();
}

bool DrawingArea::Resize(wxSize newSize)
{
    if (newSize.x < 100 || newSize.y < 100 ||
        newSize.x > GetSize().x || newSize.y > GetSize().y) {
        return false;
    }

    OnReset();
    SetSize(newSize);

    return true;
}

void DrawingArea::OnReset()
{
    bkp.clear();
    path.clear();
    shapes.clear();
    Refresh();
}

void DrawingArea::OnUndo()
{
    if (!path.empty()) {
        bkp.push_back(path.back());
        path.pop_back();
    }
    OnUpdate();
    Refresh();
}

void DrawingArea::OnRedo()
{
    if (!bkp.empty()) {
        path.push_back(bkp.back());
        bkp.pop_back();
    }
    OnUpdate();
    Refresh();
}

void DrawingArea::SetColor(unsigned int number, wxColour colorPen, wxColour colorBrush)
{
    switch (number) {
    case 0:
        colorShapePen = colorPen;
        break;
    case 1:
        colorShapeBrush = colorBrush;
        randomColorShapeBrush = false;
        break;
    case 2:
        colorLinePen = colorPen;
        colorLineBrush = colorBrush;
        break;
    default:
        break;
    }
    OnUpdate();
    Refresh();
}

void DrawingArea::SetShape(unsigned number)
{
    shape = number;
    OnUpdate();
    Refresh();
}

bool DrawingArea::IsEmpty()
{
    return path.empty();
}

void DrawingArea::SetStyle(bool isSpline)
{
    this->isSpline = isSpline;
    OnUpdate();
    Refresh();
}

void DrawingArea::SetValue(unsigned number, unsigned value)
{
    switch (number) {
    case 0:
        shapeAngle = value < 0 ? 0 : value;
        shapeAngle = value > 180 ? 180 : value;
        break;
    case 1:
        shapeLenght = value < 0 ? 0 : value;
        shapeLenght = value > 150 ? 150 : value;
        break;
    case 2:
        limitLength = value < 0 ? 0 : value;
        limitLength = value > 50 ? 50 : value;
        break;
    case 3:
        lineTickness = value < 0 ? 0 : value;
        lineTickness = value > 20 ? 20 : value;
        break;
    default:
        break;
    };
    OnUpdate();
    Refresh();
}

void DrawingArea::SetRandomColor(wxColour color1, wxColour color2)
{
    minColorShapeBrush = wxColour(std::min(color1.Red(), color2.Red()),
                                  std::min(color1.Green(), color2.Green()),
                                  std::min(color1.Blue(), color2.Blue()));
    maxColorShapeBrush = wxColour(std::max(color1.Red(), color2.Red()),
                                  std::max(color1.Green(), color2.Green()),
                                  std::max(color1.Blue(), color2.Blue()));
    randomColorShapeBrush = !(color1 == color2);
    OnUpdate();
    Refresh();
}

unsigned DrawingArea::GetValue(unsigned number)
{
    std::vector<unsigned> result{shapeAngle, shapeLenght, limitLength, lineTickness};
    return number < result.size() ? result[number] : 0;
}

bool DrawingArea::OnSaveSvgDC(wxString path, wxSize size)
{
    wxSVGFileDC svgDC (path, size.x, size.y);
    OnDraw (svgDC);

    return svgDC.IsOk();
}

bool DrawingArea::OnSaveSvg(wxString path, wxSize size)
{
    int count = 0;
    std::string polygons = "";
    for (auto& shape : shapes) {
        SVG::Shape svgShape(std::string(shape.name) + std::to_string(count++),
                            SVG::RGB2HEX(shape.pen.Red(), shape.pen.Green(), shape.pen.Blue()),
                            SVG::RGB2HEX(shape.brush.Red(), shape.brush.Green(), shape.brush.Blue()));
        std::vector<SVG::Point> points;
        for (auto& point : shape.points) {
            points.push_back(SVG::Point(point.x, point.y));
        }
        svgShape.points = points;
        polygons += SVG::polygon(svgShape);
    }
    std::string svg = SVG::svg(size.x, size.y, polygons);
    wxMessageOutputDebug().Printf("%s", svg);

    return SVG::save(svg, std::string(path));
}

bool DrawingArea::OnSaveTxT(wxString path, wxSize size)
{
    std::string delimiter = "\t";
    std::string txt = "Name" + delimiter + "Stroke" + delimiter +"Fill" + delimiter + "Points\n";
    for (auto& shape : shapes) {
        txt += std::string(shape.name) + delimiter;
        txt += SVG::RGB2HEX(shape.pen.Red(), shape.pen.Green(), shape.pen.Blue()) + delimiter;
        txt += SVG::RGB2HEX(shape.brush.Red(), shape.brush.Green(), shape.brush.Blue()) + delimiter;
        for (auto& point : shape.points) {
            txt += std::to_string(point.x) + "," + std::to_string(point.y) + delimiter;
        }
        txt += "\n";
    }
    wxMessageOutputDebug().Printf("%s", txt);
    //    return SVG::save(txt, std::string(path));
    return false;
}

std::vector<wxPoint> DrawingArea::GetPoints(unsigned shape, wxPoint pos, unsigned lenght, unsigned angle)
{
    std::vector<wxPoint> points;
    if (shape == 1) {
        points = {pos,
                  pos + angularCoordinate(lenght / 2, angle + 15),
                  pos + angularCoordinate(lenght, angle),
                  pos + angularCoordinate(lenght / 2, angle - 15),
                  pos};
    } else if (shape == 2) {
        points = {pos,
                  pos + angularCoordinate(lenght * 2 / 5, angle + 45),
                  pos + angularCoordinate(lenght, angle),
                  pos + angularCoordinate(lenght * 2 / 5, angle - 45),
                  pos};
    } else if (shape == 3) {
        points = {pos,
            pos + angularCoordinate(lenght * 2 / 6, angle + 60),
            pos + angularCoordinate(lenght * 4 / 6, angle + 20),
            pos + angularCoordinate(lenght, angle),
            pos + angularCoordinate(lenght * 4 / 6, angle - 20),
            pos + angularCoordinate(lenght * 2 / 6, angle - 60),
            pos
        };
    } else if (shape == 4) {
        points = {pos,
                  pos + angularCoordinate(lenght, angle + 15),
                  pos + angularCoordinate(lenght * 3 / 5, angle),
                  pos + angularCoordinate(lenght, angle - 15),
                  pos};
    } else if (shape == 5) {
        points = {pos,
                  pos + angularCoordinate(lenght, angle + 20),
                  pos + angularCoordinate(lenght * 1 / 5, angle),
                  pos + angularCoordinate(lenght, angle + 15),
                  pos + angularCoordinate(lenght * 2 / 5, angle),
                  pos + angularCoordinate(lenght, angle + 5),
                  pos + angularCoordinate(lenght * 3 / 5, angle),
                  pos + angularCoordinate(lenght, angle - 5),
                  pos + angularCoordinate(lenght * 2 / 5, angle),
                  pos + angularCoordinate(lenght, angle - 15),
                  pos + angularCoordinate(lenght * 1 / 5, angle),
                  pos + angularCoordinate(lenght, angle - 20),
                  pos};
    } else if (shape == 6) {
        points = {pos,
                  pos + angularCoordinate(lenght, angle + 45),
                  pos + angularCoordinate(lenght * 2 / 6, angle),
                  pos + angularCoordinate(lenght, angle + 30),
                  pos + angularCoordinate(lenght * 3 / 6, angle),
                  pos + angularCoordinate(lenght, angle + 10),
                  pos + angularCoordinate(lenght * 4 / 6, angle),
                  pos + angularCoordinate(lenght, angle - 10),
                  pos + angularCoordinate(lenght * 3 / 6, angle),
                  pos + angularCoordinate(lenght, angle - 30),
                  pos + angularCoordinate(lenght * 2 / 6, angle),
                  pos + angularCoordinate(lenght, angle - 45),
                  pos};
    } else if (shape == 7) {
        points = {pos,
                  pos + angularCoordinate(lenght / 2, angle + 70),
                  pos + angularCoordinate(lenght / 2, angle + 50),
                  pos + angularCoordinate(lenght / 2, angle + 10),
                  pos + angularCoordinate(lenght / 2, angle - 10),
                  pos + angularCoordinate(lenght / 2, angle - 50),
                  pos + angularCoordinate(lenght / 2, angle - 70),
                  pos};
    } else if (shape == 8) {
        points = {pos,
                  pos + angularCoordinate(lenght, angle + 20),
                  pos + angularCoordinate(lenght, angle + 5),
                  pos + angularCoordinate(lenght, angle - 20),
                  pos};
    } else if (shape == 9) {
        points = {pos,
                  pos + angularCoordinate(lenght, angle + 60),
                  pos + angularCoordinate(lenght * 2 / 5, angle - 45),
                  pos,
                  pos + angularCoordinate(lenght * 2 / 5, angle + 45),
                  pos + angularCoordinate(lenght, angle - 60),
                  pos};
    } else {
        points = {pos, pos + angularCoordinate(lenght, angle)};
    }

    return points;
}

