#include "app.h"

#include "wx/dcsvg.h"

bool App::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    AppFrame *frame = new AppFrame("wxWidgtes App to Draw Trees", wxSize(960, 680));
    frame->Show();

    SetTopWindow(frame);

    return true;
}

AppFrame::AppFrame(const wxString &title, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, wxPoint(1, 1), size)
{
    // Menu
    wxMenu *submenu1 = new wxMenu;
    submenu1->Append(ID_Menu_SaveTxt, "&Save As TXT",
                     "Save TXT file using custom library.");
    submenu1->AppendSeparator();
    submenu1->Append(ID_Menu_SaveDCsvg, "&Save As SVG [wxWidgets]\tCtrl-S",
                     "Save SVG file using wxWidgets library.");
    submenu1->Append(ID_Menu_SaveHsvg, "&Save As SVG [custom]\tCtrl-Shift-S",
                     "Save SVG file using custom library.");

    menu[0] = new wxMenu;
    menu[0]->Append(ID_Menu_New, "&New", "New Drawing Area");
    menu[0]->AppendSubMenu(submenu1, "Save");
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

    Bind(wxEVT_MENU, &AppFrame::OnNew, this, ID_Menu_New);
    Bind(wxEVT_MENU, &AppFrame::OnSave, this, ID_Menu_SaveDCsvg);
    Bind(wxEVT_MENU, &AppFrame::OnSave, this, ID_Menu_SaveHsvg);
    Bind(wxEVT_MENU, &AppFrame::OnSave, this, ID_Menu_SaveTxt);
    Bind(wxEVT_MENU, [ = ](wxCommandEvent &) { AboutDialog(); }, wxID_ABOUT);
    Bind(wxEVT_MENU, [ = ](wxCommandEvent &) { Close(true); }, wxID_EXIT);
    Bind(wxEVT_MENU, [ = ](wxCommandEvent &) { Reset(); }, ID_Menu_Reset);
    Bind(wxEVT_MENU, [ = ](wxCommandEvent &) { drawingArea->OnRedo(); }, ID_Menu_Redo);
    Bind(wxEVT_MENU, [ = ](wxCommandEvent &) { drawingArea->OnUndo(); }, ID_Menu_Undo);

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

    // DrawingArea - Panel
    drawingArea = new DrawingArea(this, ID_DrawingArea, wxDefaultPosition, wxSize(850, 500));
    drawingArea->SetBackgroundColour(*wxWHITE);
    drawingArea->SetShape(0); // Only lines

    // Slider
    slider[0] = new wxSlider(this, ID_wxSlider + 0, 50, 0, 100);   // shapeAngle
    slider[1] = new wxSlider(this, ID_wxSlider + 1, 50, 0, 100);   // shapeLenght
    slider[2] = new wxSlider(this, ID_wxSlider + 2, 50, 0, 100);   // limitLength
    slider[3] = new wxSlider(this, ID_wxSlider + 3, 20, 0, 100);   // lineTickness

    for (unsigned i = 0; i < 4; ++i) {
        slider[i]->SetToolTip(std::to_string(drawingArea->GetValue(i)));
        slider[i]->Bind(wxEVT_SLIDER, [ = ](wxCommandEvent &event){
            drawingArea->SetValue(i, slider[i]->GetValue());
            slider[i]->SetToolTip(std::to_string(drawingArea->GetValue(i)));
        });
    }

    // ColourPickerCtrl
    colorPCtrl[0] = new wxColourPickerCtrl(this, ID_wxColourPickerCtrl + 0,
                                           wxColor(0, 102, 0, 255));
    colorPCtrl[1] = new wxColourPickerCtrl(this, ID_wxColourPickerCtrl + 1,
                                           wxColor(50, 200, 50, 255));
    colorPCtrl[2] = new wxColourPickerCtrl(this, ID_wxColourPickerCtrl + 2,
                                           wxColor(130, 60, 0, 255));

    colorPCtrl[0]->SetToolTip("Leaf border color");
    colorPCtrl[1]->SetToolTip("Leaf fill color");
    colorPCtrl[2]->SetToolTip("Branch color");

    for (unsigned i = 0; i < 3; ++i) {
        colorPCtrl[i]->Bind(wxEVT_COLOURPICKER_CHANGED, [ = ](wxCommandEvent &event){
            drawingArea->SetColor(i, colorPCtrl[i]->GetColour(), colorPCtrl[i]->GetColour());
        });
        drawingArea->SetColor(i, colorPCtrl[i]->GetColour(), colorPCtrl[i]->GetColour());
    }

    // Check Box
    checkBox[0] = new wxCheckBox(this, ID_chkBox_Spline, "SpLine");
    checkBox[0]->SetToolTip("Draw in Spline or Polygon.");
    checkBox[0]->SetValue(false);
    drawingArea->SetStyle();

    checkBox[0]->Bind(wxEVT_CHECKBOX, [ = ](wxCommandEvent &) {
            checkBox[1]->SetValue(false);
            drawingArea->SetStyle(checkBox[0]->GetValue());
            drawingArea->Refresh(); }, ID_chkBox_Spline);

    checkBox[1] = new wxCheckBox(this, ID_chkBox_Random, "Greens");
    checkBox[1]->SetToolTip("Randomize color.");
    checkBox[1]->SetValue(false);
    drawingArea->SetStyle();

    checkBox[1]->Bind(wxEVT_CHECKBOX, [ = ](wxCommandEvent &) {
            if (checkBox[1]->GetValue()) {
                drawingArea->SetRandomColor(wxColour(0, 80, 0), wxColour(0, 200, 0));
            } else {
                drawingArea->SetRandomColor();
            }
        }, ID_chkBox_Random);

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
        bmpBtn[i]->SetToolTip("Leaf " + std::to_string(i + 1));
        bmpBtn[i]->Bind(wxEVT_BUTTON, [ = ](wxCommandEvent &event){
            drawingArea->SetShape(event.GetId() % ID_wxBitmapButton);
        });
        vBox[1]->Add(bmpBtn[i], 0, wxLEFT, 5);
    }
    vBox[1]->Add(checkBox[0], 0, wxTOP | wxBOTTOM, 5); // Spline
    vBox[1]->Add(checkBox[1], 0, wxTOP | wxBOTTOM, 5); // Random Color

    hBox[0]->Add(vBox[1]);  // Buttons
    hBox[0]->Add(drawingArea, 0, wxLEFT, 10);

    hBox[1]->Add(colorPCtrl[0], 0); // Leaf border color
    hBox[1]->Add(colorPCtrl[1], 0); // Leaf color

    hBox[2]->Add(hBox[1], 0, wxRIGHT, 5);
    hBox[2]->Add(label[0]); // Shape Angle
    hBox[2]->Add(slider[0], 0, wxRIGHT, 5);
    hBox[2]->Add(label[1]); // Shape Lenght
    hBox[2]->Add(slider[1], 0, wxRIGHT, 5);
    hBox[2]->Add(label[2]); // Distance - Limit Lenght
    hBox[2]->Add(slider[2], 0, wxRIGHT, 5);
    hBox[2]->Add(colorPCtrl[2], 0, wxRIGHT, 5); // Branch color
    hBox[2]->Add(label[3]); // Line Tickness - Branch
    hBox[2]->Add(slider[3], 0, wxRIGHT, 5);

    vBox[0]->AddSpacer(10);
    vBox[0]->Add(hBox[0], 1, wxEXPAND); // Buttons and DrawingArea
    vBox[0]->AddSpacer(10);
    vBox[0]->Add(hBox[2]);  // Buttons and Sliders
    vBox[0]->AddSpacer(5);

    // Status Bar
    statusBar = new wxStatusBar(this, ID_wxStatuBar);
    statusBar->SetFont(font);
    statusBar->SetMinHeight(30);

    // Frame
    SetSizer(vBox[0]);
    SetMinSize(size);
    SetMaxSize(size);
    SetStatusBar(statusBar);
    SetStatusText("Welcome! Keep the Mouse Left pressed to draw a branch of the tree.");
    SetBackgroundColour(wxColour(127, 127, 127, 255));

    drawingArea->Bind(wxEVT_LEFT_DOWN, [ = ](wxMouseEvent &event){
        SetStatusText("");
        drawingArea->OnMouseClicked(event);
    });

    Bind(wxEVT_CHAR_HOOK, &AppFrame::OnKeyDown, this);
}

void AppFrame::OnSave(wxCommandEvent &event)
{
    if (drawingArea->IsEmpty()) {
        SetStatusText("Nothing to do!");
        return;
    }

    std::string filter;
    switch (event.GetId()) {
    case ID_Menu_SaveDCsvg:
    case ID_Menu_SaveHsvg:
        filter = "SVG vector picture (*.svg)|*.svg";
        break;
    case ID_Menu_SaveTxt:
        filter = "Text file (*.txt)|*.txt" ;
        break;
    default:
        filter = "All files | *.*";
        break;
    }

    wxFileDialog dialog(this, "Save Picture as", wxEmptyString,
                        "tree", filter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() == wxID_OK) {
        auto result = true;
        auto path = dialog.GetPath();
        auto size = drawingArea->GetBestSize();
        wxMessageOutputDebug().Printf("%d, %d", size.x, size.y);
        switch (event.GetId()) {
        case ID_Menu_SaveDCsvg:
            result = drawingArea->OnSaveSvgDC(path, size);
            break;
        case ID_Menu_SaveHsvg:
            result = drawingArea->OnSaveSvgCustom(path, size);
            break;
        case ID_Menu_SaveTxt:
            result = drawingArea->OnSaveTxT(path, size);
            break;
        default:;
            break;
        }
        if (result) {
            wxString filename = std::filesystem::path(std::string(path)).filename().string();
            SetStatusText("Save: " + filename);
        } else {
            SetStatusText("There was something wrong!");
        }
    }
}

void AppFrame::OnNew(wxCommandEvent &event)
{
    if (!drawingArea->Resize(NewDialog(drawingArea->GetSize()).GetSize())) {
        SetStatusText("Invalid size!");
    }
}

void AppFrame::OnKeyDown(wxKeyEvent &event)
{
    auto keyCode = event.GetKeyCode();
    //SetStatusText(std::to_string(keyCode));

    if (keyCode == 8) {     // Backspace
        drawingArea->OnUndo();
    }
    if (keyCode == 13) {    // Enter
        // Pass
    }
    if (keyCode == 27) {    // ESC
        // Pass
    }
    if (keyCode == 127) {   // Delete
        drawingArea->OnReset();
    }

    event.Skip();
}

void AppFrame::Reset()
{
    drawingArea->OnReset();
    drawingArea->SetShape(0);   // Default

    slider[3]->SetValue(10);    // Line Tickness - Branch
    drawingArea->SetValue(3, slider[3]->GetValue());

    SetStatusText(wxString(wxEmptyString));
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

AppFrame::NewDialog::NewDialog(wxSize drawingAreaSize)
    : wxDialog(NULL, wxID_ANY, "New", wxDefaultPosition, wxSize(450, 300))
{
    // Box
    vBox = new wxBoxSizer(wxVERTICAL);
    hBox[0] = new wxBoxSizer(wxHORIZONTAL);
    hBox[1] = new wxBoxSizer(wxHORIZONTAL);
    hBox[2] = new wxBoxSizer(wxHORIZONTAL);
    hBox[3] = new wxBoxSizer(wxHORIZONTAL);

    // Label
    wxFont font(14, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    label[0] = new wxStaticText(this, wxID_ANY, "Enter the new size."
                                , wxDefaultPosition,  wxDefaultSize,
                                wxTE_MULTILINE | wxTE_RICH);
    label[1] = new wxStaticText(this, wxID_ANY, "Width"
                                , wxDefaultPosition,  wxDefaultSize,
                                wxTE_MULTILINE | wxTE_RICH);
    label[2] = new wxStaticText(this, wxID_ANY, "Heigth"
                                , wxDefaultPosition,  wxDefaultSize,
                                wxTE_MULTILINE | wxTE_RICH);

    label[0]->SetFont(font);
    label[1]->SetFont(font);
    label[2]->SetFont(font);

    // TextCtrl
    width = new wxTextCtrl(this, ID_Text_width, wxEmptyString,
                           wxDefaultPosition, wxDefaultSize,
                           wxTE_PROCESS_ENTER);
    height = new wxTextCtrl(this, ID_Text_width, wxEmptyString,
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_PROCESS_ENTER);

    width->SetToolTip("Min, Max : 100 - " + std::to_string(drawingAreaSize.x));
    height->SetToolTip("Min, Max : 100 - " + std::to_string(drawingAreaSize.y));

    // Button
    okBtn = new wxButton(this, wxID_ANY, "Ok", wxDefaultPosition, wxSize(70, 30));
    okBtn->Bind(wxEVT_BUTTON, [ = ](wxCommandEvent &) { Close(true); });
    okBtn->SetFocus();

    // Dialog
    hBox[0]->Add(label[0], 1);
    hBox[1]->Add(label[1], 1);
    hBox[1]->Add(width, 1);
    hBox[2]->Add(label[2], 1);
    hBox[2]->Add(height, 1);
    hBox[3]->Add(okBtn, 1);

    vBox->AddSpacer(10);
    vBox->Add(hBox[0], 1, wxALIGN_CENTRE_HORIZONTAL);
    vBox->AddSpacer(10);
    vBox->Add(hBox[1], 1, wxALIGN_CENTRE_HORIZONTAL);
    vBox->AddSpacer(10);
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

wxSize AppFrame::NewDialog::GetSize()
{
    Show();

    try {
        return wxSize(wxAtoi(width->GetValue()), wxAtoi(height->GetValue()));
    } catch (...) {
        // pass
    }

    return wxSize(0, 0);
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
    if (isDrawing) {
        // Cursor
        dc.SetPen(colorCursorPen);
        dc.SetBrush(colorCursorBrush);
        dc.DrawCircle(cursorPosition.x, cursorPosition.y, cursorRadius);
        // Border
        dc.SetPen(colorBorderPen);
        dc.SetBrush(colorBorderBrush);
        dc.DrawRectangle(panelBorder, panelBorder,
                         GetSize().x - 2 * panelBorder,
                         GetSize().y - 2 * panelBorder);
    }

    wxPoint pos;
    shapes.clear();
    std::vector<wxPoint> pointsLine;
    std::vector<wxPoint> pointsShape;
    for (auto &line : path) {
        pos = line.end;
        for (unsigned i = line.points.size() - 1; i > 0; i--) {
            auto lenght = Distance(pos.x, pos.y, line.points[i].x, line.points[i].y);
            if (lenght > limitLength ) {
                auto sectionAngle = LineAngle(pos.x, pos.y, line.points[i].x, line.points[i].y);
                dc.SetPen(colorShapePen);
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
                dc.SetBrush(colorShapeBrush);
                for (auto& signal : {-1, 1}) {
                    auto angle = sectionAngle + signal * shapeAngle;
                    pointsShape = GetPoints(shape, pos + angularCoordinate(lineTickness, angle),
                                            shapeLenght, angle);
                    shapes.push_back(Shape(isSpline ? "Spline" : "Polygon",
                                           colorShapePen, colorShapeBrush, pointsShape));
                    if (isSpline) {
                        dc.DrawSpline(pointsShape.size(), &pointsShape[0]);
                    } else {
                        dc.DrawPolygon(pointsShape.size(), &pointsShape[0]);
                    }
                }
                pos = line.points[i];
                pointsLine.push_back(pos);
            }
        }
        pointsLine.push_back(line.begin);
        dc.SetPen(wxPen(colorLinePen, lineTickness));
        dc.SetBrush(colorLineBrush);
        if (pointsLine.size() > 2 && lineTickness > 0) {
            dc.DrawSpline(pointsLine.size(), &pointsLine[0]);
            shapes.push_back(Shape("Line", colorLinePen, colorLineBrush, pointsLine));
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
        } else {
            isDrawing = false;
        }
        if (event.Moving() && !randomColorShapeBrush) {
            randomColorShapeBrush = false;
        }
        Refresh();
    }
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
    cursorPosition = wxPoint(panelBorder, panelBorder);
    bkp.clear();
    path.clear();
    shapes.clear();
    Refresh();
}

bool DrawingArea::OnSaveSvgDC(wxString path, wxSize size)
{
    wxSVGFileDC svgDC (path, size.x, size.y);
    OnDraw (svgDC);

    return svgDC.IsOk();
}

bool DrawingArea::OnSaveSvgCustom(wxString path, wxSize size)
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

void DrawingArea::SetStyle(bool isSpline)
{
    this->isSpline = isSpline;
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

void DrawingArea::SetRandomColor(wxColour color1, wxColour color2)
{
    minColorShapeBrush = wxColour(std::min(color1.Red(), color2.Red()),
                                  std::min(color1.Green(), color2.Green()),
                                  std::min(color1.Blue(), color2.Blue()));
    maxColorShapeBrush = wxColour(std::max(color1.Red(), color2.Red()),
                                  std::max(color1.Green(), color2.Green()),
                                  std::max(color1.Blue(), color2.Blue()));
    randomColorShapeBrush = !(color1 == color2);
    Refresh();
}

unsigned DrawingArea::GetValue(unsigned number)
{
    std::vector<unsigned> result{shapeAngle, shapeLenght,
                                 limitLength, lineTickness};
    return number < result.size() ? result[number] : 0;
}

std::vector<wxPoint> DrawingArea::GetPoints(unsigned shape, wxPoint pos,
                                            unsigned lenght, unsigned angle)
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
