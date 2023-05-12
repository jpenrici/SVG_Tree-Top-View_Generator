#include "app.h"

#include "wx/dcsvg.h"

bool App::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    AppFrame *frame = new AppFrame("wxWidgtes App to Draw Trees", wxSize(960, 650));
    frame->Show();

    SetTopWindow(frame);

    return true;
}

AppFrame::AppFrame(const wxString &title, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, wxPoint(1, 1), size)
{
    // Menu
    menu[0] = new wxMenu;
    menu[0]->Append(ID_Menu_SaveDCsvg, "&Save\tCtrl-S",
                    "Save SVG file using wxWidgets library.");
    menu[0]->Append(ID_Menu_SaveHsvg, "&Save [Custom]\tCtrl-Shift-S",
                    "Save SVG file using custom library.");
    menu[0]->AppendSeparator();
    menu[0]->Append(wxID_EXIT);

    menu[1] = new wxMenu;
    menu[1]->Append(ID_Menu_Undo, "&Undo\tCtrl-Z", "Remove the last branch.");
    menu[1]->Append(ID_Menu_Redo, "&Redo\tCtrl-Y", "Reconstruct removed branch.");
    menu[1]->AppendSeparator();
    menu[1]->Append(ID_Menu_Reset, "&Reset\tDelete", "Clear drawing area.");

    menu[2] = new wxMenu;
    menu[2]->Append(wxID_ABOUT, "&About\tF1", "Show about dialog.");

    menuBar = new wxMenuBar;
    menuBar->Append(menu[0], "&File");
    menuBar->Append(menu[1], "&Edit");
    menuBar->Append(menu[2], "&Help");
    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &AppFrame::OnSave, this, ID_Menu_SaveDCsvg);
    Bind(wxEVT_MENU, &AppFrame::OnSave, this, ID_Menu_SaveHsvg);
    Bind(wxEVT_MENU, &AppFrame::OnUndo, this, ID_Menu_Undo);
    Bind(wxEVT_MENU, &AppFrame::OnUndo, this, ID_Menu_Redo);
    Bind(wxEVT_MENU, &AppFrame::OnReset, this, ID_Menu_Reset);
    Bind(wxEVT_MENU, &AppFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, [ = ](wxCommandEvent &) { Close(true); }, wxID_EXIT);

    // Font
    wxFont font(14, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    // Text
    label[0] = new wxStaticText(this, ID_wxLabel, "Angle");
    label[1] = new wxStaticText(this, ID_wxLabel, "Lenght");
    label[2] = new wxStaticText(this, ID_wxLabel, "Distance");
    label[3] = new wxStaticText(this, ID_wxLabel, "Tickness");

    label[0]->SetFont(font);
    label[1]->SetFont(font);
    label[2]->SetFont(font);
    label[3]->SetFont(font);

    label[0]->SetToolTip("Angle of the leaf on the branch");
    label[1]->SetToolTip("Leaf lenght");
    label[2]->SetToolTip("Minimum distance between sheets");
    label[3]->SetToolTip("Branch thickness");

    // Panel
    drawingArea = new DrawingArea(this);
    drawingArea->SetBackgroundColour(*wxWHITE);
    drawingArea->SetShape(0); // Only lines

    // Slider
    slider[0] = new wxSlider(this, ID_wxSlider + 0, 50, 0, 100);   // shapeAngle
    slider[1] = new wxSlider(this, ID_wxSlider + 1, 50, 0, 100);   // shapeLenght
    slider[2] = new wxSlider(this, ID_wxSlider + 2, 50, 0, 100);   // limitLength
    slider[3] = new wxSlider(this, ID_wxSlider + 3, 20, 0, 100);   // lineTickness

    for (unsigned i = 0; i < 4; ++i) {
        slider[i]->Bind(wxEVT_SLIDER, &AppFrame::OnChangeSlider, this, ID_wxSlider + i);
        drawingArea->SetValue(i, slider[i]->GetValue());
    }

    // ColourPickerCtrl
    colourPickerCtrl[0] = new wxColourPickerCtrl(this, ID_wxColourPickerCtrl + 0,
                                                 wxColor(0, 102, 0, 255));
    colourPickerCtrl[1] = new wxColourPickerCtrl(this, ID_wxColourPickerCtrl + 1,
                                                 wxColor(50, 200, 50, 255));
    colourPickerCtrl[2] = new wxColourPickerCtrl(this, ID_wxColourPickerCtrl + 2,
                                                 wxColor(130, 60, 0, 255));

    colourPickerCtrl[0]->SetToolTip("Leaf color");
    colourPickerCtrl[1]->SetToolTip("Leaf fill color");
    colourPickerCtrl[2]->SetToolTip("Branch color");

    colourPickerCtrl[0]->Bind(wxEVT_COLOURPICKER_CHANGED, &AppFrame::OnChangeColor, this);
    colourPickerCtrl[1]->Bind(wxEVT_COLOURPICKER_CHANGED, &AppFrame::OnChangeColor, this);
    colourPickerCtrl[2]->Bind(wxEVT_COLOURPICKER_CHANGED, &AppFrame::OnChangeColor, this);
    drawingArea->SetColor(0, colourPickerCtrl[0]->GetColour(), colourPickerCtrl[0]->GetColour());
    drawingArea->SetColor(1, colourPickerCtrl[1]->GetColour(), colourPickerCtrl[1]->GetColour());
    drawingArea->SetColor(2, colourPickerCtrl[2]->GetColour(), colourPickerCtrl[2]->GetColour());

    // Box and Controllers
    vBox[0] = new wxBoxSizer(wxVERTICAL);
    vBox[1] = new wxBoxSizer(wxVERTICAL);
    hBox[0] = new wxBoxSizer(wxHORIZONTAL);
    hBox[1] = new wxBoxSizer(wxHORIZONTAL);
    hBox[2] = new wxBoxSizer(wxHORIZONTAL);

    std::vector<std::string> icons {
        "icon_line0.png", "icon_line1.png", "icon_line2.png", "icon_line3.png",
        "icon_line4.png", "icon_line5.png", "icon_line6.png", "icon_line7.png",
        "icon_line8.png", "icon_line9.png"
    };
    for (unsigned i = 0; i < 10; ++i) {
        bmpBtn[i] = new wxBitmapButton(this, ID_wxBitmapButton + i,
                                       wxBitmap("Resources/" + icons[i],
                                                wxBITMAP_TYPE_ANY));
        bmpBtn[i]->SetSize(bmpBtn[i]->GetBestSize());
        bmpBtn[i]->SetName(std::to_string(i));
        bmpBtn[i]->SetToolTip(icons[i]);
        bmpBtn[i]->Bind(wxEVT_BUTTON, &AppFrame::OnBitmapButtonClicked, this);
        vBox[1]->Add(bmpBtn[i], 0, 0, 0, 0);
    }

    hBox[0]->Add(vBox[1]);
    hBox[0]->AddSpacer(5);
    hBox[0]->Add(drawingArea, 1, wxEXPAND);
    hBox[0]->AddSpacer(5);

    hBox[1]->Add(colourPickerCtrl[0]);
    hBox[1]->Add(colourPickerCtrl[1]);

    hBox[2]->AddSpacer(10);
    hBox[2]->Add(hBox[1]);
    hBox[2]->AddSpacer(5);
    hBox[2]->Add(label[0]);
    hBox[2]->Add(slider[0]);
    hBox[2]->Add(label[1]);
    hBox[2]->Add(slider[1]);
    hBox[2]->Add(label[2]);
    hBox[2]->Add(slider[2]);
    hBox[2]->AddSpacer(5);
    hBox[2]->Add(colourPickerCtrl[2]);
    hBox[2]->AddSpacer(5);
    hBox[2]->Add(label[3]);
    hBox[2]->Add(slider[3]);

    vBox[0]->AddSpacer(10);
    vBox[0]->Add(hBox[0], 1, wxEXPAND);
    vBox[0]->AddSpacer(10);
    vBox[0]->Add(hBox[2]);
    vBox[0]->AddSpacer(10);

    // Status Bar
    statusBar = new wxStatusBar(this, ID_wxStatuBar);
    statusBar->SetFont(font);
    statusBar->SetMinHeight(30);

    // Frame
    SetSizer(vBox[0]);
    SetMinSize(size);
    SetStatusBar(statusBar);
    SetStatusText("");

    Bind(wxEVT_CHAR_HOOK, &AppFrame::OnKeyDown, this);
}

void AppFrame::OnSave(wxCommandEvent &event)
{
    if (drawingArea->IsEmpty()) {
        SetStatusText("Nothing to do!");
        return;
    }

    wxFileDialog dialog(this, "Save Picture as", wxEmptyString,
                        "ops", "SVG vector picture (*.svg)|*.svg",
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() == wxID_OK) {
        auto path = dialog.GetPath();
        auto size = drawingArea->GetBestSize();
        wxMessageOutputDebug().Printf("%d, %d", size.x, size.y);
        if (event.GetId() == ID_Menu_SaveDCsvg) {
            if (drawingArea->OnSaveSvg(path, size)) {
                SetStatusText("Save: " + path);
            } else {
                SetStatusText("There was something wrong!");
            }
        } else if (event.GetId() == ID_Menu_SaveHsvg) {
            // To do
        }
    }
}

void AppFrame::OnUndo(wxCommandEvent &event)
{
    if (event.GetId() == ID_Menu_Undo) {
        drawingArea->OnUndo();
    } else if (event.GetId() == ID_Menu_Redo) {
        drawingArea->OnRedo();
    }
}

void AppFrame::OnBitmapButtonClicked(wxCommandEvent &event)
{
    unsigned number = event.GetId() % ID_wxBitmapButton;
    drawingArea->SetShape(number);
}

void AppFrame::OnChangeSlider(wxCommandEvent &event)
{
    unsigned number = event.GetId() % ID_wxSlider;
    drawingArea->SetValue(number, slider[number]->GetValue());
}

void AppFrame::OnChangeColor(wxColourPickerEvent &event)
{
    unsigned number = event.GetId() % ID_wxColourPickerCtrl;
    drawingArea->SetColor(number,
                          colourPickerCtrl[number]->GetColour(),
                          colourPickerCtrl[number]->GetColour());
}

void AppFrame::OnReset(wxCommandEvent &event)
{
    slider[3]->SetValue(10);
    drawingArea->OnReset();
    drawingArea->SetShape(0);
    drawingArea->SetValue(3, slider[3]->GetValue());
    SetStatusText(wxString(wxEmptyString));
}

void AppFrame::OnAbout(wxCommandEvent &event)
{
    AboutDialog *aboutDialog = new AboutDialog();
    aboutDialog->Show(true);
}

void AppFrame::OnKeyDown(wxKeyEvent &event)
{
    auto keyCode = event.GetKeyCode();
    //SetStatusText(std::to_string(keyCode));

    if (keyCode == 127) { // Delete
        drawingArea->OnReset();
    }
    if (keyCode == 13) { // Enter
        // Pass
    }

    event.Skip();
}

AppFrame::AboutDialog::AboutDialog()
    : wxDialog(NULL, wxID_ANY, "About", wxDefaultPosition, wxSize(450, 300))
{
    // Box
    vBox = new wxBoxSizer(wxVERTICAL);
    hBox[0] = new wxBoxSizer(wxHORIZONTAL);
    hBox[1] = new wxBoxSizer(wxHORIZONTAL);
    hBox[2] = new wxBoxSizer(wxHORIZONTAL);
    hBox[3] = new wxBoxSizer(wxHORIZONTAL);

    // Label
    label = new wxStaticText(this, wxID_ANY, ABOUT, wxDefaultPosition,
                             wxDefaultSize, wxTE_MULTILINE | wxTE_RICH);
    label->SetFont(wxFont(14, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_NORMAL));

    // Hyperlink
    hyperlink1 = new wxHyperlinkCtrl(this, wxID_ANY,
                                     ABOUT_HLINK1, ABOUT_HLINK1,
                                     wxDefaultPosition, wxDefaultSize,
                                     wxHL_DEFAULT_STYLE);
    hyperlink1->SetFont(wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                               wxFONTWEIGHT_NORMAL));

    hyperlink2 = new wxHyperlinkCtrl(this, wxID_ANY,
                                     ABOUT_HLINK2, ABOUT_HLINK2,
                                     wxDefaultPosition, wxDefaultSize,
                                     wxHL_DEFAULT_STYLE);
    hyperlink2->SetFont(wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                               wxFONTWEIGHT_NORMAL));

    // Button
    okBtn = new wxButton(this, wxID_ANY, "Ok", wxDefaultPosition, wxSize(70, 30));
    okBtn->Bind(wxEVT_BUTTON, [ = ](wxCommandEvent &) { Close(true); });
    okBtn->SetFocus();

    // Dialog
    hBox[0]->Add(label, 1);
    hBox[1]->Add(hyperlink1, 1);
    hBox[2]->Add(hyperlink2, 1);
    hBox[3]->Add(okBtn, 1);

    vBox->AddSpacer(10);
    vBox->Add(hBox[0], 1, wxALIGN_CENTRE_HORIZONTAL);
    vBox->AddSpacer(10);
    vBox->Add(hBox[1], 1, wxALIGN_CENTRE_HORIZONTAL);
    vBox->Add(hBox[2], 1, wxALIGN_CENTRE_HORIZONTAL);
    vBox->AddSpacer(10);
    vBox->Add(hBox[3], 1, wxALIGN_CENTRE_HORIZONTAL);

    SetSizer(vBox);

#ifdef WIN32
    SetBackgroundColour(wxColour(*wxWHITE));
#endif

    Centre();
    ShowModal();

    // Exit
    Destroy();
}

DrawingArea::DrawingArea(wxFrame *parent)
    : wxPanel(parent)
{
    // Cursor
    cursorRadius = 5;
    cursorPosition = wxPoint(0, 0);

    // Colors
    colorCursorPen = wxColor(0, 0, 0, 255);
    colorCursorBrush = wxColor(0, 0, 0, 255);
    colorLinePen = wxColor(0, 0, 0, 255);
    colorLineBrush = wxColor(0, 0, 0, 255);
    colorShapePen = wxColor(0, 0, 0, 255);
    colorShapeBrush = wxColor(0, 0, 0, 255);

    // State of the drawing pen
    isDrawing = true;
    path.clear();

    // Draw
    lineTickness = 10;
    limitLength = 20;
    shapeAngle = 60;
    shapeLenght = 50;

    // Handlers
    Bind(wxEVT_PAINT, &DrawingArea::OnPaint, this, wxID_ANY);
    Bind(wxEVT_SIZE, &DrawingArea::OnSize, this, wxID_ANY);
    Bind(wxEVT_LEFT_DOWN, &DrawingArea::OnMouseClicked, this, wxID_ANY);
    Bind(wxEVT_RIGHT_DOWN, &DrawingArea::OnMouseClicked, this, wxID_ANY);
    Bind(wxEVT_MOTION, &DrawingArea::OnMouseClicked, this, wxID_ANY);
}

void DrawingArea::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
    OnDraw(dc);
}

void DrawingArea::OnUndo()
{
    if (!path.empty()) {
        bkp.push_back(path.back());
        path.pop_back();
    }
    Refresh();
}

void DrawingArea::OnRedo()
{
    if (!bkp.empty()) {
        path.push_back(bkp.back());
        bkp.pop_back();
    }
    Refresh();
}

void DrawingArea::OnDraw(wxDC &dc)
{
    dc.SetPen(colorCursorPen);
    dc.SetBrush(colorCursorBrush);

    if (isDrawing) {
        dc.DrawCircle(cursorPosition.x, cursorPosition.y, cursorRadius);
    }

    wxPoint pos;
    std::vector<wxPoint> points;
    for (auto &line : path) {
        pos = line.end;
        for (unsigned i = line.points.size() - 1; i > 0; i--) {
            auto lenght = Distance(pos.x, pos.y, line.points[i].x, line.points[i].y);
            if (lenght > limitLength ) {
                auto sectionAngle = LineAngle(pos.x, pos.y, line.points[i].x, line.points[i].y);
                dc.SetPen(colorShapePen);
                dc.SetBrush(colorShapeBrush);
                DrawShape(dc, shape,
                          pos + angularCoordinate(0, 0, lineTickness, sectionAngle + shapeAngle),
                          shapeLenght, sectionAngle + shapeAngle);
                DrawShape(dc, shape,
                          pos + angularCoordinate(0, 0, lineTickness, sectionAngle - shapeAngle),
                          shapeLenght, sectionAngle - shapeAngle);
                pos = line.points[i];
                points.push_back(pos);
            }
        }
        points.push_back(line.begin);
        dc.SetPen(wxPen(colorLinePen, lineTickness));
        dc.SetBrush(colorLineBrush);
        if (points.size() > 2 && lineTickness > 0) {
            dc.DrawSpline(points.size(), &points[0]);
        }
        points.clear();
    }
}

void DrawingArea::OnSize(wxSizeEvent &event)
{
    Refresh();
}

void DrawingArea::OnMouseClicked(wxMouseEvent &event)
{
    cursorPosition = ScreenToClient(::wxGetMousePosition());
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
    } else {
        isDrawing = false;
    }
    Refresh();
}

void DrawingArea::OnReset()
{
    cursorPosition = wxPoint(0, 0);
    path.clear();
    Refresh();
}

bool DrawingArea::OnSaveSvg(wxString path, wxSize size)
{
    wxSVGFileDC svgDC (path, size.x, size.y);
    OnDraw (svgDC);

    return svgDC.IsOk();
}

void DrawingArea::SetColor(unsigned int number, wxColor colorPen, wxColor colorBrush)
{
    switch (number) {
    case 0:
        colorShapePen = colorPen;
        break;
    case 1:
        colorShapeBrush = colorBrush;
        break;
    case 2:
        colorLinePen = colorPen;
        colorLineBrush = colorBrush;
        break;
    default:
        break;
    }
    Refresh();

}

void DrawingArea::SetShape(unsigned number)
{
    shape = number;
    Refresh();
}

bool DrawingArea::IsEmpty()
{
    return path.empty();
}

void DrawingArea::SetValue(unsigned number, unsigned value)
{
    switch (number) {
    case 0:
        shapeAngle = value * 180 / 100;
        break;
    case 1:
        shapeLenght = value * 150 / 100;
        break;
    case 2:
        limitLength = value * 50 / 100;
        break;
    case 3:
        lineTickness = value * 20 / 100;
        break;
    default:
        break;
    };
    Refresh();
}

void DrawingArea::DrawShape(wxDC& dc, unsigned int shape,
                            wxPoint pos, unsigned int lenght, unsigned int angle)
{
    std::vector<wxPoint> points;
    if (shape == 1) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght / 2, angle + 15),
                  pos + angularCoordinate(0, 0, lenght, angle),
                  pos + angularCoordinate(0, 0, lenght / 2, angle - 15)};
    } else if (shape == 2) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght * 2 / 5, angle + 45),
                  pos + angularCoordinate(0, 0, lenght, angle),
                  pos + angularCoordinate(0, 0, lenght * 2 / 5, angle - 45)};
    } else if (shape == 3) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght * 2 / 6, angle + 60),
                  pos + angularCoordinate(0, 0, lenght * 4 / 6, angle + 20),
                  pos + angularCoordinate(0, 0, lenght, angle),
                  pos + angularCoordinate(0, 0, lenght * 4 / 6, angle - 20),
                  pos + angularCoordinate(0, 0, lenght * 2 / 6, angle - 60),
                  };
    } else if (shape == 4) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght, angle + 15),
                  pos + angularCoordinate(0, 0, lenght * 3 / 5, angle),
                  pos + angularCoordinate(0, 0, lenght, angle - 15)};
    } else if (shape == 5) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght, angle + 20),
                  pos + angularCoordinate(0, 0, lenght * 1 / 5, angle),
                  pos + angularCoordinate(0, 0, lenght, angle + 15),
                  pos + angularCoordinate(0, 0, lenght * 2 / 5, angle),
                  pos + angularCoordinate(0, 0, lenght, angle + 5),
                  pos + angularCoordinate(0, 0, lenght * 3 / 5, angle),
                  pos + angularCoordinate(0, 0, lenght, angle - 5),
                  pos + angularCoordinate(0, 0, lenght * 2 / 5, angle),
                  pos + angularCoordinate(0, 0, lenght, angle - 15),
                  pos + angularCoordinate(0, 0, lenght * 1 / 5, angle),
                  pos + angularCoordinate(0, 0, lenght, angle - 20)};
    } else if (shape == 6) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght, angle + 45),
                  pos + angularCoordinate(0, 0, lenght * 2 / 6, angle),
                  pos + angularCoordinate(0, 0, lenght, angle + 30),
                  pos + angularCoordinate(0, 0, lenght * 3 / 6, angle),
                  pos + angularCoordinate(0, 0, lenght, angle + 10),
                  pos + angularCoordinate(0, 0, lenght * 4 / 6, angle),
                  pos + angularCoordinate(0, 0, lenght, angle - 10),
                  pos + angularCoordinate(0, 0, lenght * 3 / 6, angle),
                  pos + angularCoordinate(0, 0, lenght, angle - 30),
                  pos + angularCoordinate(0, 0, lenght * 2 / 6, angle),
                  pos + angularCoordinate(0, 0, lenght, angle - 45)};
    } else if (shape == 7) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght / 2, angle + 70),
                  pos + angularCoordinate(0, 0, lenght / 2, angle + 50),
                  pos + angularCoordinate(0, 0, lenght / 2, angle + 10),
                  pos + angularCoordinate(0, 0, lenght / 2, angle - 10),
                  pos + angularCoordinate(0, 0, lenght / 2, angle - 50),
                  pos + angularCoordinate(0, 0, lenght / 2, angle - 70)};
    } else if (shape == 8) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght, angle + 20),
                  pos + angularCoordinate(0, 0, lenght, angle + 5),
                  pos + angularCoordinate(0, 0, lenght, angle - 20)};
    } else if (shape == 9) {
        points = {pos,
                  pos + angularCoordinate(0, 0, lenght, angle + 60),
                  pos + angularCoordinate(0, 0, lenght * 2 / 5, angle - 45),
                  pos,
                  pos + angularCoordinate(0, 0, lenght * 2 / 5, angle + 45),
                  pos + angularCoordinate(0, 0, lenght, angle - 60)};
    } else {
        points = {pos, pos + angularCoordinate(pos.x, pos.y, lenght, angle)};
    }
    if (!points.empty()) {
        if (points.size() == 2) {
            dc.DrawLine(points.front(), points.back());
        } else {
            dc.DrawPolygon(points.size(), &points[0]);
        }
    }
}
