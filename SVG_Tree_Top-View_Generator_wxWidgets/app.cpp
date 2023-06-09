#include "app.h"

bool App::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    AppFrame *frame = new AppFrame("wxWidgtes App to Draw Trees", wxSize(1024, 700));
    frame->Show();

    SetTopWindow(frame);

    return true;
}

AppFrame::AppFrame(const wxString &title, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, wxPoint(1, 1), size)
{
    // Menu
    wxMenu *submenu1 = new wxMenu;
    submenu1->Append(ID_Menu_SaveTxt, "&TXT", "Save TXT file using custom library.");
    submenu1->AppendSeparator();
    submenu1->Append(ID_Menu_SaveHsvg, "&SVG\tCtrl-S", "Save SVG file using custom library.");
    submenu1->Append(ID_Menu_SaveDCsvg, "&SVG [wxWidgets]\tCtrl-Shift-S", "Save SVG file using wxWidgets library.");

    menu[0] = new wxMenu;
    menu[0]->AppendSubMenu(submenu1, "Save As");
    menu[0]->AppendSeparator();
    menu[0]->Append(wxID_EXIT);

    menu[1] = new wxMenu;
    menu[1]->Append(ID_Menu_Undo, "&Undo\tCtrl-Z", "Remove the last branch.");
    menu[1]->Append(ID_Menu_Redo, "&Redo\tCtrl-Y", "Reconstruct removed branch.");
    menu[1]->AppendSeparator();
    menu[1]->Append(ID_Menu_Reset, "&Reset\tDelete", "Clear drawing area.");

    std::vector<std::vector<unsigned> > daSize = {{150, 150}, {300, 300}, {480, 480},
                                                  {500, 500}, {600, 480}, {800, 500}, {900, 500}};
    wxMenu *submenu2 = new wxMenu;
    for (unsigned i = 0; i < daSize.size(); i++) {
        wxString s = std::to_string(daSize[i].front()) + " x " + std::to_string(daSize[i].back());
        submenu2->Append(ID_Array_Menu_Size + i, s);
        submenu2->Bind(wxEVT_MENU, [ = ](wxCommandEvent & event) {
                drawingArea->Resize(wxSize(daSize[i].front(), daSize[i].back()));
            }, ID_Array_Menu_Size + i);
    }
    submenu2->Append(ID_Array_Menu_Size + daSize.size(), "Custom\tCtrl-N");
    submenu2->Bind(wxEVT_MENU, [ = ](wxCommandEvent & event) {
            if (!drawingArea->Resize(NewDialog(drawingArea->GetSize()).GetSize())) {
                SetStatusText("Invalid size!");
            }
        }, ID_Array_Menu_Size + daSize.size());

    menu[2] = new wxMenu;
    menu[2]->AppendSubMenu(submenu2, "New");

    menu[3] = new wxMenu;
    menu[3]->Append(wxID_ABOUT, "&About\tF1", "Show about dialog.");

    menuBar = new wxMenuBar;
    menuBar->Append(menu[0], "&File");
    menuBar->Append(menu[1], "&Edit");
    menuBar->Append(menu[2], "&Image");
    menuBar->Append(menu[3], "&Help");
    SetMenuBar(menuBar);

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
    wxFont font1(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont font2(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    // Text
    std::vector<wxString> labels = {"Welcome!", "Creator", "Title", "Publisher"};
    std::vector<wxString> tips = {"Drawing", "SVG creator name.", "Drawing title.", "Publisher or website."};
    for (unsigned i = 0; i < 4; i++) {
        info[i] = new wxStaticText(this, ID_Array_Label_Info + i, labels[i]);
        info[i]->SetToolTip(tips[i]);
        info[i]->SetFont(font);
        if (i > 0) {
            info[i]->Show(false);
        }
    }
    for (unsigned i = 0; i < 3; i++) {
        txtCtrl[i] = new wxTextCtrl(this,  ID_Array_TextCtrl + i, wxEmptyString,
                                    wxDefaultPosition, wxSize(100, 24), wxTE_RICH,
                                    wxDefaultValidator, wxTextCtrlNameStr);
        txtCtrl[i]->SetLabelText("");
        txtCtrl[i]->SetFont(font);
        txtCtrl[i]->Show(false);
    }

    labels = {"Angle", "Lenght", "Distance", "Width"};
    tips = {"Angle of the leaf on the branch of the tree", "Tree leaf length.",
            "Distance between tree leaves.", "Branch thickness."};
    for (unsigned i = 0; i < 4; i++) {
        label[i] = new wxStaticText(this, ID_Array_Label_Info + i, labels[i]);
        label[i]->SetToolTip(tips[i]);
        label[i]->SetFont(font1);
    }

    // DrawingArea - Panel
    drawingArea = new DrawingArea(this, ID_DrawingArea, wxDefaultPosition, wxSize(900, 500));
    drawingArea->SetBackgroundColour(*wxWHITE);
    drawingArea->SetShape(0); // Only lines

    // Slider
    slider[0] = new wxSlider(this, ID_Array_Slider + 0, 60, 0, 180);   // shapeAngle
    slider[1] = new wxSlider(this, ID_Array_Slider + 1, 50, 0, 150);   // shapeLenght
    slider[2] = new wxSlider(this, ID_Array_Slider + 2, 10, 0,  50);   // limitLength
    slider[3] = new wxSlider(this, ID_Array_Slider + 3, 2,  0,  20);   // lineTickness

    tips = {"Angle of the leaf on the branch.", "Leaf lenght.", "Minimum distance between sheets.",
            "Branch thickness."};
    for (unsigned i = 0; i < 4; ++i) {
        slider[i]->SetToolTip(tips[i]);
    }

    slider[0]->Bind(wxEVT_SLIDER, [ = ](wxCommandEvent & event) {
        drawingArea->SetValue(0, slider[0]->GetValue(), checkBox[3]->GetValue());
        slider[0]->SetToolTip(std::to_string(drawingArea->GetValue(0)));
    });

    slider[1]->Bind(wxEVT_SLIDER, [ = ](wxCommandEvent & event) {
        drawingArea->SetValue(1, slider[1]->GetValue(), checkBox[4]->GetValue());
        slider[1]->SetToolTip(std::to_string(drawingArea->GetValue(1)));
    });

    slider[2]->Bind(wxEVT_SLIDER, [ = ](wxCommandEvent & event) {
        drawingArea->SetValue(2, slider[2]->GetValue(), checkBox[5]->GetValue());
        slider[2]->SetToolTip(std::to_string(drawingArea->GetValue(2)));
    });

    slider[3]->Bind(wxEVT_SLIDER, [ = ](wxCommandEvent & event) {
        drawingArea->SetValue(3, slider[3]->GetValue());
        slider[3]->SetToolTip(std::to_string(drawingArea->GetValue(3)));
    });

    // Check Box
    checkBox[0] = new wxCheckBox(this, ID_ChkBox_Spline, "SpLine");
    checkBox[0]->SetToolTip("Draw in Spline or Polygon.");
    checkBox[0]->SetValue(false);
    drawingArea->SetStyle();

    checkBox[0]->Bind(wxEVT_CHECKBOX, [ = ](wxCommandEvent &) {
            checkBox[1]->SetValue(false);
            drawingArea->SetStyle(checkBox[0]->GetValue());
            drawingArea->Refresh();
        }, ID_ChkBox_Spline);

    checkBox[1] = new wxCheckBox(this, ID_ChkBox_Random, "Greens");
    checkBox[1]->SetToolTip("Randomize color.");
    checkBox[1]->SetValue(false);
    drawingArea->SetStyle();

    checkBox[1]->Bind(wxEVT_CHECKBOX, [ = ](wxCommandEvent &) {
            checkBox[0]->SetValue(false);
            drawingArea->SetStyle(checkBox[0]->GetValue());
            if (checkBox[1]->GetValue()) {
                SetStatusText("Use the R key to randomize again.");
                drawingArea->SetRandomColor(wxColour(0, 80, 0), wxColour(0, 200, 0));
            }
            else {
                SetStatusText("");
                drawingArea->SetRandomColor();
                drawingArea->SetColor(1, colorPCtrl[1]->GetColour(), colorPCtrl[1]->GetColour());
            }
        }, ID_ChkBox_Random);

    checkBox[2] = new wxCheckBox(this, ID_ChkBox_Metadata, "");
    checkBox[2]->SetToolTip("Metadata.");
    checkBox[2]->SetValue(false);
    checkBox[2]->Bind(wxEVT_CHECKBOX, [ = ](wxCommandEvent &) {
        info[1]->Show(checkBox[2]->GetValue());
        info[2]->Show(checkBox[2]->GetValue());
        info[3]->Show(checkBox[2]->GetValue());
        txtCtrl[0]->Show(checkBox[2]->GetValue());
        txtCtrl[1]->Show(checkBox[2]->GetValue());
        txtCtrl[2]->Show(checkBox[2]->GetValue());
    });

    checkBox[3] = new wxCheckBox(this, ID_ChkBox_Angle, "");
    checkBox[3]->SetToolTip("Redraw all branches of the tree with the current angle.");
    checkBox[3]->SetValue(false);

    checkBox[4] = new wxCheckBox(this, ID_ChkBox_Angle, "");
    checkBox[4]->SetToolTip("Redraw all tree leaves with the current length.");
    checkBox[4]->SetValue(false);

    checkBox[5] = new wxCheckBox(this, ID_ChkBox_Angle, "");
    checkBox[5]->SetToolTip("Redraw all tree leaves with the current distance.");
    checkBox[5]->SetValue(false);

    // ColourPickerCtrl
    colorPCtrl[0] = new wxColourPickerCtrl(this, ID_Array_ColourPickerCtrl + 0, wxColor(0, 102, 0, 255));
    colorPCtrl[1] = new wxColourPickerCtrl(this, ID_Array_ColourPickerCtrl + 1, wxColor(50, 200, 50, 255));
    colorPCtrl[2] = new wxColourPickerCtrl(this, ID_Array_ColourPickerCtrl + 2, wxColor(130, 60, 0, 255));

    tips = {"Leaf border color", "Leaf fill color", "Branch color"};
    for (unsigned i = 0; i < 3; ++i) {
        colorPCtrl[i]->SetToolTip(tips[i]);
        drawingArea->SetColor(i, colorPCtrl[i]->GetColour(), colorPCtrl[i]->GetColour());
    }

    colorPCtrl[0]->Bind(wxEVT_COLOURPICKER_CHANGED, [ = ](wxCommandEvent & event) {
        drawingArea->SetColor(0, colorPCtrl[0]->GetColour(), colorPCtrl[0]->GetColour());
    });
    colorPCtrl[1]->Bind(wxEVT_COLOURPICKER_CHANGED, [ = ](wxCommandEvent & event) {
        checkBox[1]->SetValue(false);
        drawingArea->SetColor(1, colorPCtrl[1]->GetColour(), colorPCtrl[1]->GetColour());
    });
    colorPCtrl[2]->Bind(wxEVT_COLOURPICKER_CHANGED, [ = ](wxCommandEvent & event) {
        drawingArea->SetColor(2, colorPCtrl[2]->GetColour(), colorPCtrl[2]->GetColour());
    });

    // Box and Controllers
    vBox[0] = new wxBoxSizer(wxVERTICAL);
    vBox[1] = new wxBoxSizer(wxVERTICAL);
    hBox[0] = new wxBoxSizer(wxHORIZONTAL);
    hBox[1] = new wxBoxSizer(wxHORIZONTAL);
    hBox[2] = new wxBoxSizer(wxHORIZONTAL);
    hBox[3] = new wxBoxSizer(wxHORIZONTAL);

    bmpBtn[0] = new wxBitmapButton(this, ID_Array_BitmapButton,
                                   wxBitmap(wxBitmap("Resources/icon_line0.png", wxBITMAP_TYPE_ANY)).ConvertToImage().Rescale(16, 16));
    bmpBtn[0]->SetToolTip("Redraws all branches.");
    bmpBtn[0]->Bind(wxEVT_BUTTON, [ = ](wxCommandEvent & event) {
        drawingArea->SetShape(currentShape, true);
    });
    for (unsigned i = 1; i < 11; ++i) {
        wxString img = "Resources/icon_line" + std::to_string(i) + ".png";
        bmpBtn[i] = new wxBitmapButton(this, ID_Array_BitmapButton + i, wxBitmap(img, wxBITMAP_TYPE_ANY));
        bmpBtn[i]->SetSize(bmpBtn[i]->GetBestSize());
        bmpBtn[i]->SetToolTip("Leaf " + std::to_string(i));
        bmpBtn[i]->Bind(wxEVT_BUTTON, [ = ](wxCommandEvent & event) {
            drawingArea->BreakPath();
            currentShape = event.GetId() % ID_Array_BitmapButton;
            drawingArea->SetShape(currentShape);
            wxImage icon = wxBitmap(img, wxBITMAP_TYPE_ANY).ConvertToImage().Rescale(16, 16);
            bmpBtn[0]->SetBitmap(wxBitmap(icon));
        });
        vBox[1]->Add(bmpBtn[i], 0, wxLEFT, 5);
    }

    vBox[1]->Add(checkBox[0], 0, wxTOP | wxBOTTOM, 2); // Spline
    vBox[1]->Add(checkBox[1], 0, wxTOP | wxBOTTOM, 2); // Random Color

    hBox[0]->Add(info[0], 1, wxEXPAND);             // Area
    hBox[0]->Add(info[1]);                          // Creator
    hBox[0]->Add(txtCtrl[0], 0, wxLEFT, 5);
    hBox[0]->Add(info[2]);                          // Title
    hBox[0]->Add(txtCtrl[1], 0, wxLEFT, 5);
    hBox[0]->Add(info[3]);                          // Publisher
    hBox[0]->Add(txtCtrl[2], 0, wxLEFT, 5);
    hBox[0]->Add(checkBox[2], 0, wxLEFT, 15);
    hBox[1]->Add(vBox[1]);                          // Buttons
    hBox[1]->Add(drawingArea, 0, wxLEFT, 10);

    hBox[2]->Add(bmpBtn[0], 0, wxRIGHT, 2);         // Current shape
    hBox[2]->Add(colorPCtrl[0], 0);                 // Leaf border color
    hBox[2]->Add(colorPCtrl[1], 0);                 // Leaf color

    hBox[3]->Add(hBox[2], 0, wxRIGHT, 5);
    hBox[3]->Add(label[0], 0, wxRIGHT, 4);          // Shape Angle
    hBox[3]->Add(checkBox[3]);
    hBox[3]->Add(slider[0], 0, wxRIGHT, 4);
    hBox[3]->Add(label[1], 0, wxRIGHT, 4);          // Shape Lenght
    hBox[3]->Add(checkBox[4]);
    hBox[3]->Add(slider[1],0, wxRIGHT, 4);
    hBox[3]->Add(label[2], 0, wxRIGHT, 4);          // Distance - Limit Lenght
    hBox[3]->Add(checkBox[5]);
    hBox[3]->Add(slider[2], 0, wxRIGHT, 4);
    hBox[3]->Add(colorPCtrl[2], 0, wxRIGHT, 5);     // Branch color
    hBox[3]->Add(label[3]);                         // Line Tickness - Branch
    hBox[3]->Add(slider[3]);

    vBox[0]->AddSpacer(10);
    vBox[0]->Add(hBox[0], 1, wxEXPAND);             // Info
    vBox[0]->Add(hBox[1], 1, wxEXPAND);             // Buttons and DrawingArea
    vBox[0]->AddSpacer(10);
    vBox[0]->Add(hBox[3]);                          // Buttons and Sliders
    vBox[0]->AddSpacer(10);

    // Status Bar
    statusBar = new wxStatusBar(this, ID_StatuBar);
    statusBar->SetFont(font);
    statusBar->SetMinHeight(30);

    // Frame
    SetSizer(vBox[0]);
    SetMinSize(size);
    SetMaxSize(size);
    SetStatusBar(statusBar);
    SetStatusText("Click or Press Left Mouse Button to draw the tree branch and Esc to release.");
    SetBackgroundColour(wxColour(50, 50, 50, 255));

    drawingArea->Bind(wxEVT_LEFT_DOWN, [ = ](wxMouseEvent & event) {
        drawingArea->OnMouseClicked(event);
        SetStatusText("");
        info[0]->SetLabelText("Area: " + std::to_string(drawingArea->GetSize().GetWidth())
                              + " x " + std::to_string(drawingArea->GetSize().GetHeight()));
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

    wxFileDialog dialog(this, "Save Picture as", wxEmptyString, "tree", filter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() == wxID_OK) {
        auto result = false;
        auto path = dialog.GetPath();
        switch (event.GetId()) {
        case ID_Menu_SaveDCsvg:
            result = drawingArea->OnSaveSvgDC(path);
            break;
        case ID_Menu_SaveHsvg:
            result = drawingArea->OnSaveSvg(path, SVG::Metadata(
                                                      std::string(txtCtrl[0]->GetValue()),
                                                      std::string(txtCtrl[1]->GetValue()),
                                                      std::string(txtCtrl[2]->GetValue())));
            break;
        case ID_Menu_SaveTxt:
            result = drawingArea->OnSaveTxT(path);
            break;
        default:
            break;
        }
        if (result) {
            wxString filename = std::filesystem::path(std::string(path)).filename().string();
            SetStatusText("Save: " + filename);
        }
        else {
            SetStatusText("There was something wrong!");
        }
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
        drawingArea->BreakPath();
    }
    if (keyCode == 82) {     // R
        if (checkBox[1]->GetValue()) {
            drawingArea->SetShape(currentShape);
        }
    }
    if (keyCode == 127) {   // Delete
        drawingArea->OnReset();
    }

    event.Skip();
}

void AppFrame::Reset()
{
    drawingArea->OnReset();
    drawingArea->SetShape(currentShape);

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
    label = new wxStaticText(this, wxID_ANY, ABOUT, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH);
    label->SetFont(wxFont(14, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    // Hyperlink
    hyperlink1 = new wxHyperlinkCtrl(this, wxID_ANY, ABOUT_HLINK1, ABOUT_HLINK1, wxDefaultPosition, wxDefaultSize,
                                     wxHL_DEFAULT_STYLE);
    hyperlink1->SetFont(wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    hyperlink2 = new wxHyperlinkCtrl(this, wxID_ANY, ABOUT_HLINK2, ABOUT_HLINK2, wxDefaultPosition, wxDefaultSize,
                                     wxHL_DEFAULT_STYLE);
    hyperlink2->SetFont(wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

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
    label[0] = new wxStaticText(this, wxID_ANY, "Enter the new size.", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH);
    label[1] = new wxStaticText(this, wxID_ANY, "Width", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH);
    label[2] = new wxStaticText(this, wxID_ANY, "Heigth", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH);
    label[0]->SetFont(font);
    label[1]->SetFont(font);
    label[2]->SetFont(font);

    // TextCtrl
    width = new wxTextCtrl(this, ID_Text_width, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    height = new wxTextCtrl(this, ID_Text_width, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

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
    }
    catch (...) {
        // pass
    }

    return wxSize(0, 0);
}
