/*
 * Simple tree generator in top view.
 *
 * References:
 *
 *      https://www.wxwidgets.org/
 *      https://www.w3.org/TR/SVG2/
 *
 */

#include "app.h"

#include <iostream>

IMPLEMENT_APP_NO_MAIN(App)

int main(int argc, char *argv[])
{
    // Operating system information
    auto osInfo = wxGetOsDescription().Upper();
#ifdef __LINUX__
    osInfo = wxGetLinuxDistributionInfo().Description.Upper();
#endif
    wxMessageOutputDebug().Printf("OS: %s", osInfo);

    // Application
    wxApp *pApp = new App();
    App::SetInstance(pApp);

    if (pApp != nullptr) {
        wxEntryStart(argc, argv);
        if (wxTheApp->OnInit()) {
            wxMessageOutputDebug().Printf("%s is open!", pApp->GetAppName().Upper());
            wxTheApp->OnRun();
        };
        wxMessageOutputDebug().Printf("%s is closed!", pApp->GetAppName().Upper());
        wxEntryCleanup();
    }
    else {
        std::cerr << "There was something wrong\n";
    }

    return 0;
}
