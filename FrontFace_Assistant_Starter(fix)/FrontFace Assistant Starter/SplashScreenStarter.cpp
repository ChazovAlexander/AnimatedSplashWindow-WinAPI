// SplashScreenStarter.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SplashScreenStarter.h"

#include "SplashScreen.h"
#include "Objidl.h"
#include <iostream>
#include <string>
#include "ResourceImageLoader.h"
#include "shlwapi.h"
#include "FileImageLoader.h"
#include "IO.h"


#define MAX_LOADSTRING 100

TCHAR szStartFile[MAX_LOADSTRING];
TCHAR szAppPrefix[MAX_LOADSTRING];

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow) {

    LoadString(hInstance, IDC_START_APP, szStartFile, MAX_LOADSTRING);
    LoadString(hInstance, IDC_APP_PREFIX, szAppPrefix, MAX_LOADSTRING);
    CImageLoader* imageLoaders[] = {
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG1), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG2), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG3), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG4), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG5), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG6), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG7), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG8), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG9), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG10), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG11), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG12), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG13), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG14), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG15), _T("PNG")),
    new CResourceImageLoader(MAKEINTRESOURCE(IDB_PNG16), _T("PNG"))
    };
    int arraySize = sizeof(imageLoaders) / sizeof(imageLoaders[0]);

    CSplashScreen splash(
        hInstance,
        // Length of time in milliseconds to display the splash screen fading
        1000,
        // Specifies the way to load the image for the splash screen
        // To load from the resources, provide the resource name and the resource type name.
        // We have used PNG as an example.
        // If you use a PNG, its opacity should be honored to allow you to display partially transparent splash screens.
        // To edit the resources in a C++ application, go to the resource view tab.
        // To load from a file, use this line instead, where 'filename' is the file you wish to load:
        // new CFileImageLoader(filename),
        new CResourceImageLoader(MAKEINTRESOURCE(IDR_SPLASH), _T("PNG")),
        imageLoaders,
        // Application prefix. This will be added to the event name to avoid conflicts between applications.
        szAppPrefix,
        // File name of your executable to run. The extension does not need to be .exe.  
        // If you want to stop your users from starting your application without displaying the splash screen,
        // you could use a different extension.
        // It is assumed that the executable is in the same folder as this program.
        // If it is not, you can call splash.SetFullPath.
        szStartFile
    );

    // Display the splash screen
    splash.Show();

}


// in WPF code
//private void CloseSplashScreen()
//{
//    // signal the native process (that launched us) to close the splash screen
//    using (var closeSplashEvent = new EventWaitHandle(false,
//        EventResetMode.ManualReset, "CloseSplashScreenEvent"+Prefix[from Resource]))
//    {
//        closeSplashEvent.Set();
//    }
//}

