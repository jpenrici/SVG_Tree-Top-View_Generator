#include "drawingArea.h"    // wxPanel

#include "wx/dcsvg.h"

inline wxPoint angularCoordinate(unsigned lenght, unsigned angle)
{
    // Origin: (0,0)
    return wxPoint(Cos(0, lenght, angle), Sin(0, lenght, angle));
}

inline wxPoint angularCoordinate(wxPoint p, unsigned lenght, unsigned angle)
{
    // Origin: Point
    return wxPoint(Cos(p.x, lenght, angle), Sin(p.y, lenght, angle));
}

inline wxPoint angularCoordinate(double x, double y, unsigned lenght, unsigned angle)
{
    // Origin: (x,y)
    return wxPoint(Cos(x, lenght, angle), Sin(y, lenght, angle));
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

    // State of the drawing
    currentSize = size;
    isDrawing = true;
    maxSize = size;
    path.clear();

    // Draw
    breakPath = true;
    isSpline = false;
    limitLength = 20;
    lineWidth = 10;
    panelBorder = 20;
    shapeAngle = 60;
    shapeLenght = 50;

    // Handlers
    Bind(wxEVT_LEFT_DOWN, &DrawingArea::OnMouseClicked, this, id);
    Bind(wxEVT_LEFT_UP, &DrawingArea::OnMouseClicked, this, id);
    Bind(wxEVT_MOTION, &DrawingArea::OnMouseClicked, this, id);
    Bind(wxEVT_PAINT, &DrawingArea::OnPaint, this, id);
    Bind(wxEVT_SIZE, [ = ](wxSizeEvent &) { Refresh(); }, id);
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
    // Cursor
    dc.SetPen(colorCursorPen);
    dc.SetBrush(colorCursorBrush);
    if (isDrawing || breakPath) {
        dc.DrawCircle(cursorPosition.x, cursorPosition.y, cursorRadius);
    }
    if (!path.empty()) {
        if (path.back().points.size() == 1) {
            dc.DrawCircle(path.back().points.front().x, path.back().points.front().y, cursorRadius);
        }
        if (!path.back().points.empty() && !breakPath) {
            dc.DrawLine(path.back().points.back().x, path.back().points.back().y,
                        cursorPosition.x, cursorPosition.y);
        }
    }
    // Grade
    if (isDrawing || path.empty()) {
        // Border
        dc.SetPen(colorBorderPen);
        dc.SetBrush(colorBorderBrush);
        dc.DrawRectangle(panelBorder, panelBorder, GetSize().x - 2 * panelBorder, GetSize().y - 2 * panelBorder);
        // Center
        dc.DrawLine(GetSize().x / 2, 0, GetSize().x / 2, GetSize().y);
        dc.DrawLine(0, GetSize().y / 2, GetSize().x, GetSize().y / 2);
    }
    // Shapes
    for (auto &shape : shapes) {
        dc.SetPen(wxPen(shape.pen, shape.lineWidth));
        dc.SetBrush(shape.brush);
        if (isSpline) {
            dc.DrawSpline(shape.points.size(), &shape.points[0]);
        }
        else {
            if (shape.name == "Line") {
                dc.DrawLines(shape.points.size(), &shape.points[0]);
            }
            else {
                dc.DrawPolygon(shape.points.size(), &shape.points[0]);
            }
        }
    }
};

void DrawingArea::OnUpdate()
{
    shapes.clear();
    std::vector<wxPoint> pointsLine;

    // Leafs
    wxPoint currentPoint;
    for (auto &line : path) { // check all branches
        if (line.shapeLenght > 0) {  // non-transparent leaf
            currentPoint = line.points.back();  // last branch point
            for (unsigned i = line.points.size() - 1; i > 0; i--) {  // check all branch points
                auto distance = Distance(currentPoint.x, currentPoint.y, line.points[i].x, line.points[i].y);
                if (distance > line.limitLength) { // distance greater than expected range
                    // Segment angle
                    auto lineAngle = LineAngle(currentPoint.x, currentPoint.y, line.points[i].x, line.points[i].y);
                    // Number of intermediate points in the segment
                    unsigned num = distance / line.limitLength;
                    for (unsigned j = 0; j < num; j++) {  // build intermediate points if necessary
                        // Fill color
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
                        // Current leafs
                        wxPoint point;
                        point = currentPoint - angularCoordinate(j * line.limitLength, lineAngle);
                        for (auto &signal : {-1, 1}) {
                            auto angle = lineAngle + signal * line.shapeAngle;
                            auto points = GetPoints(line.shapeNumber,
                                                    point + angularCoordinate(lineWidth, angle), line.shapeLenght, angle);
                            // Save structure
                            shapes.push_back(Shape(isSpline ? "Spline" : "Polygon", colorShapePen, colorShapeBrush, 1, points));
                        }
                    }
                    // Next segment
                    currentPoint = line.points[i];
                }
                // Branch points
                pointsLine.push_back(line.points[i]);
            }
        }
        // Current branch
        if (pointsLine.size() > 1 && lineWidth > 0) {
            // Save structure
            shapes.push_back(Shape("Line", colorLinePen, colorLineBrush, lineWidth, pointsLine));
        }
        // Prepare for the next branch
        pointsLine.clear();
    }
}

void DrawingArea::OnMouseClicked(wxMouseEvent &event)
{
    cursorPosition = ScreenToClient(::wxGetMousePosition());
    if (cursorPosition.x > panelBorder && cursorPosition.x < GetSize().x - panelBorder &&
        cursorPosition.y > panelBorder && cursorPosition.y < GetSize().y - panelBorder) {
        if (event.LeftDown()) {
            isDrawing = true;
            if (path.empty() || breakPath) {
                path.push_back(Path(cursorPosition, shapeNumber, shapeAngle, shapeLenght, limitLength));
                breakPath = false;
            }
        }
        if (isDrawing && event.LeftIsDown()) {
            if (!path.empty()) {
                path.back().points.push_back(cursorPosition);
                OnUpdate();
            }
        }
        if (event.LeftUp()) {
            isDrawing = false;
        }
        if (event.Moving() && !randomColorShapeBrush) {
            randomColorShapeBrush = false;
        }
    }
    Refresh();
}

void DrawingArea::BreakPath()
{
    breakPath = true;
}

bool DrawingArea::Resize(wxSize newSize, bool reset)
{
    if (newSize.x < 100 || newSize.y < 100 || newSize.x > maxSize.x || newSize.y > maxSize.y) {
        return false;
    }

    if (reset) {
        OnReset();
    }

    SetSize(newSize);
    currentSize = newSize;

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

void DrawingArea::SetShape(unsigned number, bool all)
{
    shapeNumber = number;
    if (all) {
        for (unsigned i = 0; i < path.size(); i++) {
            path[i].shapeNumber = number;
        }
    }
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

void DrawingArea::SetValue(unsigned number, unsigned value, bool all)
{
    switch (number) {
    case 0:
        shapeAngle = value < 0 ? 0 : value;
        shapeAngle = value > 180 ? 180 : value;
        if (all) {
            for (unsigned i = 0; i < path.size(); i++) {
                path[i].shapeAngle = shapeAngle;
            }
        }
        break;
    case 1:
        shapeLenght = value < 0 ? 0 : value;
        shapeLenght = value > 150 ? 150 : value;
        if (all) {
            for (unsigned i = 0; i < path.size(); i++) {
                path[i].shapeLenght = shapeLenght;
            }
        }
        break;
    case 2:
        limitLength = value < 0 ? 0 : value;
        limitLength = value > 50 ? 50 : value;
        if (all) {
            for (unsigned i = 0; i < path.size(); i++) {
                path[i].limitLength = limitLength;
            }
        }
        break;
    case 3:
        lineWidth = value < 0 ? 0 : value;
        lineWidth = value > 20 ? 20 : value;
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
    std::vector<unsigned> result{shapeAngle, shapeLenght, limitLength, lineWidth};
    return number < result.size() ? result[number] : 0;
}

bool DrawingArea::OnSaveSvgDC(wxString path)
{
    wxSVGFileDC svgDC(path, currentSize.x, currentSize.y);
    OnDraw(svgDC);

    return svgDC.IsOk();
}

bool DrawingArea::OnSaveSvg(wxString path, SVG::Metadata metadata)
{
    int count = 0;
    std::string image = "";
    std::string group = "";
    for (unsigned i = 0; i < shapes.size(); i++) {
        Shape s = shapes[i];
        SVG::Shape svgShape(std::string(s.name) + std::to_string(count++),
                            SVG::RGB2HEX(s.brush.Red(), s.brush.Green(), s.brush.Blue()),
                            SVG::RGB2HEX(s.pen.Red(), s.pen.Green(), s.pen.Blue()),
                            s.lineWidth);
        std::vector<SVG::Point> points;
        for (auto &point : s.points) {
            points.push_back(SVG::Point(point.x, point.y));
        }
        svgShape.points = points;
        if (s.name == "Polygon" || s.name == "Spline") {
            group += SVG::polygon(svgShape);
            if (i == shapes.size() - 1) {
                image = group;
            }
        }
        else {
            if (!group.empty()) {
                group = SVG::group("Leafs" + std::to_string(count++), group);
                image += SVG::group("Branch" + std::to_string(count++),
                                    group + SVG::polyline(svgShape));
                group = "";
            }
            else {
                image += SVG::polyline(svgShape);
            }
        }
    }

    std::string svg = SVG::svg(currentSize.x, currentSize.y, image, metadata);
    //wxMessageOutputDebug().Printf("%s", svg);

    return SVG::save(svg, std::string(path));
}

bool DrawingArea::OnSaveTxT(wxString path)
{
    std::string delim = "\t";
    std::string txt = "Drawing Area" + delim + std::to_string(currentSize.x) + " x " + std::to_string(currentSize.y) + "\n";
    txt += "Name" + delim + "Stroke" + delim + "Fill" + delim + "Stroke width" + delim + "Points\n";
    for (auto &shape : shapes) {
        txt += std::string(shape.name) + delim;
        txt += SVG::RGB2HEX(shape.pen.Red(), shape.pen.Green(), shape.pen.Blue()) + delim;
        txt += SVG::RGB2HEX(shape.brush.Red(), shape.brush.Green(), shape.brush.Blue()) + delim;
        txt += std::to_string(shape.lineWidth) + delim;
        for (auto &point : shape.points) {
            txt += std::to_string(point.x) + "," + std::to_string(point.y) + delim;
        }
        txt += "\n";
    }
    //wxMessageOutputDebug().Printf("%s", txt);

    return SVG::save(txt, std::string(path));
}

std::vector<wxPoint> DrawingArea::GetPoints(unsigned shape, wxPoint pos, unsigned lenght, unsigned angle)
{
    // Custom images similar to leaf buttons
    std::vector<wxPoint> points;
    if (shape == 2) {
        points = {pos,
            pos + angularCoordinate(lenght / 2, angle + 15),
            pos + angularCoordinate(lenght, angle),
            pos + angularCoordinate(lenght / 2, angle - 15),
            pos
        };
    }
    else if (shape == 3) {
        points = {pos,
            pos + angularCoordinate(lenght * 2 / 5, angle + 45),
            pos + angularCoordinate(lenght, angle),
            pos + angularCoordinate(lenght * 2 / 5, angle - 45),
            pos
        };
    }
    else if (shape == 4) {
        points = {pos,
            pos + angularCoordinate(lenght * 2 / 6, angle + 60),
            pos + angularCoordinate(lenght * 4 / 6, angle + 20),
            pos + angularCoordinate(lenght, angle),
            pos + angularCoordinate(lenght * 4 / 6, angle - 20),
            pos + angularCoordinate(lenght * 2 / 6, angle - 60),
            pos
        };
    }
    else if (shape == 5) {
        points = {pos,
            pos + angularCoordinate(lenght, angle + 15),
            pos + angularCoordinate(lenght * 3 / 5, angle),
            pos + angularCoordinate(lenght, angle - 15),
            pos
        };
    }
    else if (shape == 6) {
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
            pos
        };
    }
    else if (shape == 7) {
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
            pos
        };
    }
    else if (shape == 8) {
        points = {pos,
            pos + angularCoordinate(lenght / 2, angle + 70),
            pos + angularCoordinate(lenght / 2, angle + 50),
            pos + angularCoordinate(lenght / 2, angle + 10),
            pos + angularCoordinate(lenght / 2, angle - 10),
            pos + angularCoordinate(lenght / 2, angle - 50),
            pos + angularCoordinate(lenght / 2, angle - 70),
            pos
        };
    }
    else if (shape == 9) {
        points = {pos,
            pos + angularCoordinate(lenght, angle + 20),
            pos + angularCoordinate(lenght, angle + 5),
            pos + angularCoordinate(lenght, angle - 20),
            pos
        };
    }
    else if (shape == 10) {
        points = {pos,
            pos + angularCoordinate(lenght, angle + 60),
            pos + angularCoordinate(lenght * 2 / 5, angle - 45),
            pos,
            pos + angularCoordinate(lenght * 2 / 5, angle + 45),
            pos + angularCoordinate(lenght, angle - 60),
            pos
        };
    }
    else {
        // Simple line
        points = {pos, pos + angularCoordinate(lenght, angle)};
    }

    return points;
}
