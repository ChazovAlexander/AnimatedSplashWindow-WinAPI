#include "StdAfx.h"
#include "resource.h"
#include "SplashScreen.h"
#include "shlwapi.h"
#include <thread>
#include <future>


#pragma comment(lib, "msimg32.lib")


CSplashScreen::CSplashScreen(HINSTANCE hInstance, DWORD nFadeoutTime, CImageLoader *pImgLoader, 
	CImageLoader* imageLoaders[],
	LPCTSTR lpszPrefix, LPCTSTR lpszAppFileName)
{
	m_strSplashClass = _T("SplashWindow");
	m_strSplashClass += lpszPrefix;
	m_hInstance = hInstance;
	m_nFadeoutTime = nFadeoutTime;
	m_strPrefix = lpszPrefix;
	m_strAppFileName = lpszAppFileName;
	m_pImgLoader = pImgLoader;

	memset(&m_blend, 0, sizeof(m_blend));
	m_nFadeoutEnd = 0;
	for (int i = 0; i < 16; i++) {
		m_pLoadImgLoader[i] = imageLoaders[i];
	}
}

CSplashScreen::~CSplashScreen(void)
{
}

HBITMAP CSplashScreen::CreateAlphaTextBitmap(LPCWSTR inText, HFONT inFont, COLORREF inColour) {
	int TextLength = (int)wcslen(inText);
	if (TextLength <= 0) return NULL;

	// Create DC and select font into it
	HDC hTextDC = CreateCompatibleDC(NULL);
	HFONT hOldFont = (HFONT)SelectObject(hTextDC, inFont);
	HBITMAP hMyDIB = NULL;

	// Get text area
	RECT TextArea = { 0, 0, 0, 0 };
	DrawText(hTextDC, inText, TextLength, &TextArea, DT_CALCRECT);

	if ((TextArea.right > TextArea.left) && (TextArea.bottom > TextArea.top)) {
		BITMAPINFOHEADER BMIH;
		memset(&BMIH, 0x0, sizeof(BITMAPINFOHEADER));

		void *pvBits = NULL;

		// Specify DIB setup
		BMIH.biSize = sizeof(BMIH);
		BMIH.biWidth = TextArea.right - TextArea.left;
		BMIH.biHeight = TextArea.bottom - TextArea.top;
		BMIH.biPlanes = 1;
		BMIH.biBitCount = 32;
		BMIH.biCompression = BI_RGB;

		// Create and select DIB into DC
		hMyDIB = CreateDIBSection(hTextDC, (LPBITMAPINFO)&BMIH, 0, (LPVOID*)&pvBits, NULL, 0);
		HBITMAP hOldBMP = (HBITMAP)SelectObject(hTextDC, hMyDIB);

		if (hOldBMP != NULL) {
			// Set up DC properties
			SetTextColor(hTextDC, 0x00FFFFFF);
			SetBkColor(hTextDC, 0x00000000);
			SetBkMode(hTextDC, OPAQUE);

			// Draw text to buffer
			DrawText(hTextDC, inText, TextLength, &TextArea, DT_NOCLIP);

			BYTE* DataPtr = (BYTE*)pvBits;
			BYTE FillR = GetRValue(inColour);
			BYTE FillG = GetGValue(inColour);
			BYTE FillB = GetBValue(inColour);
			BYTE ThisA;

			for (int LoopY = 0; LoopY < BMIH.biHeight; LoopY++) {
				for (int LoopX = 0; LoopX < BMIH.biWidth; LoopX++) {
					ThisA = *DataPtr; // Move alpha and pre-multiply with RGB
					*DataPtr++ = (FillB * ThisA) >> 8;
					*DataPtr++ = (FillG * ThisA) >> 8;
					*DataPtr++ = (FillR * ThisA) >> 8;
					*DataPtr++ = ThisA; // Set Alpha
				}
			}

			// De-select bitmap
			SelectObject(hTextDC, hOldBMP);
		}
	}

	// De-select font and destroy temp DC
	SelectObject(hTextDC, hOldFont);
	DeleteDC(hTextDC);
	// Return DIBSection
	return hMyDIB;
}

// Calls UpdateLayeredWindow to set a bitmap (with alpha) as the content of the splash window.
void CSplashScreen::SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash)
{
	// get the size of the bitmap
	BITMAP bm;
	POINT ptZero = { 0 };

	GetObject(hbmpSplash, sizeof(bm), &bm);

	SIZE sizeSplash = { bm.bmWidth, bm.bmHeight };

	// get the primary monitor's info
	HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorinfo = { 0 };
	monitorinfo.cbSize = sizeof(monitorinfo);
	GetMonitorInfo(hmonPrimary, &monitorinfo);

	// center the splash screen in the middle of the primary work area
	const RECT& rcWork = monitorinfo.rcWork;
	POINT ptOrigin;

	ptOrigin.x = rcWork.left + (rcWork.right - rcWork.left - sizeSplash.cx) / 2;
	ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - sizeSplash.cy) / 2;

	// create a memory DC holding the splash bitmap
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpSplash);

	// use the source image's alpha channel for blending
	m_blend.BlendOp = AC_SRC_OVER;
	m_blend.SourceConstantAlpha = 0xff;
	m_blend.AlphaFormat = AC_SRC_ALPHA;

	// paint the window (in the right location) with the alpha-blended bitmap
	UpdateLayeredWindow(hwndSplash, hdcScreen, &ptOrigin, &sizeSplash, hdcMem, &ptZero, RGB(0, 0, 0), &m_blend, ULW_ALPHA);

	// delete temporary objects
	SelectObject(hdcMem, hbmpOld);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	::SetWindowPos(hwndSplash,       // handle to window
		//HWND_TOPMOST,  // placement-order handle
		HWND_TOP,
		ptOrigin.x,     // horizontal position
		ptOrigin.y,      // vertical position
		sizeSplash.cx,  // width
		sizeSplash.cy, // height
		SWP_SHOWWINDOW); // window-positioning options);
}
void CSplashScreen::SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash, const std::wstring& text, int x, int y)
{

	// get the size of the bitmap
	BITMAP bm;
	POINT ptZero = { 0 };

	GetObject(hbmpSplash, sizeof(bm), &bm);

	SIZE sizeSplash = { bm.bmWidth, bm.bmHeight };

	// get the primary monitor's info
	HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorinfo = { 0 };
	monitorinfo.cbSize = sizeof(monitorinfo);
	GetMonitorInfo(hmonPrimary, &monitorinfo);

	// center the splash screen in the middle of the primary work area
	const RECT& rcWork = monitorinfo.rcWork;
	POINT ptOrigin;

	ptOrigin.x = rcWork.left + (rcWork.right - rcWork.left - sizeSplash.cx) / 2;
	ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - sizeSplash.cy) / 2;


	// create a memory DC holding the splash bitmap
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpSplash);

	RECT TextArea = { 0, 0, 0, 0 };
	HFONT TempFont = CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, L"Arial\0");
	HBITMAP MyBMP = CreateAlphaTextBitmap(text.c_str(), TempFont, RGB(0, 0, 0));
	DeleteObject(TempFont);

	if (MyBMP) { // Create temporary DC and select new Bitmap into it
		HDC hTempDC = CreateCompatibleDC(hdcMem);
		HBITMAP hOldBMP = (HBITMAP)SelectObject(hTempDC, MyBMP);

		if (hOldBMP) {
			BITMAP BMInf; // Get Bitmap image size
			GetObject(MyBMP, sizeof(BITMAP), &BMInf);

			// Fill blend function and blend new text to window
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 0xBE; // 0x80 "alpha wert"
			bf.AlphaFormat = AC_SRC_ALPHA;

			AlphaBlend(hdcMem, x, y, BMInf.bmWidth, BMInf.bmHeight,
				hTempDC, 0, 0, BMInf.bmWidth, BMInf.bmHeight, bf);

			// Clean up
			SelectObject(hTempDC, hOldBMP);
			DeleteObject(MyBMP);
			DeleteDC(hTempDC);
		}
	}

	// use the source image's alpha channel for blending
	m_blend.BlendOp = AC_SRC_OVER;
	m_blend.SourceConstantAlpha = 0xff;
	m_blend.AlphaFormat = AC_SRC_ALPHA;

	// paint the window (in the right location) with the alpha-blended bitmap
	UpdateLayeredWindow(hwndSplash, hdcScreen, &ptOrigin, &sizeSplash, hdcMem, &ptZero, RGB(0, 0, 0), &m_blend, ULW_ALPHA);

	// delete temporary objects
	SelectObject(hdcMem, hbmpOld);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	::SetWindowPos(hwndSplash,       // handle to window
		//HWND_TOPMOST,  // placement-order handle
		HWND_TOP,
		ptOrigin.x,     // horizontal position
		ptOrigin.y,      // vertical position
		sizeSplash.cx,  // width
		sizeSplash.cy, // height
		SWP_SHOWWINDOW); // window-positioning options);
}

// Creates the splash owner window and the splash window.
HWND CSplashScreen::CreateSplashWindow()
{
	HWND hwndOwner = CreateWindowEx(WS_EX_TOOLWINDOW, m_strSplashClass.c_str(), NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL, m_hInstance, NULL);
	return CreateWindowEx(WS_EX_LAYERED + WS_EX_TOOLWINDOW, m_strSplashClass.c_str(), NULL, WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, hwndOwner, NULL, m_hInstance, NULL);
}

// Registers a window class for the splash and splash owner windows.
void CSplashScreen::RegisterWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = m_hInstance;
	//wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_SPLASHICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = m_strSplashClass.c_str();

	RegisterClass(&wc);
}
// Registers a window class for the splash and splash owner windows.
void CSplashScreen::UnregisterWindowClass() {
	UnregisterClass(m_strSplashClass.c_str(), m_hInstance);
}

HANDLE CSplashScreen::LaunchWpfApplication(HWND hwndSplash, HBITMAP hbmpSplash)
{
	// get folder of the current process
	TCHAR szCurrentFolder[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szCurrentFolder, MAX_PATH);

	PathRemoveFileSpec(szCurrentFolder);

	// add the application name to the path
	TCHAR szApplicationPath[MAX_PATH];
	if (m_strFullPath.length() > 0) {
		lstrcpy(szApplicationPath, m_strFullPath.c_str());
	}
	else {
		PathCombine(szApplicationPath, szCurrentFolder, m_strAppFileName.c_str());
	}


	// check if the file exists
	if (!PathFileExists(szApplicationPath))
	{
		std::wstring startappname(m_strAppFileName.c_str());
		std::wstring concatted_stdstr = L"The required application file \"" + startappname + L"\" was not found! The installation seems to be corrupted.";
		LPCWSTR concatted = concatted_stdstr.c_str();

		MessageBox(NULL, concatted, (LPCWSTR)L"FrontFace Assistant", MB_ICONERROR | MB_OK);
		return NULL;
	}
	// start the application
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };
	CreateProcess(szApplicationPath, GetCommandLine(), NULL, NULL, FALSE, 0, NULL, szCurrentFolder, &si, &pi);

	return pi.hProcess;
}
bool CSplashScreen::FadeWindowOut(HWND hWnd) {
	DWORD dtNow = GetTickCount64();
	if (dtNow >= m_nFadeoutEnd)
	{
		return true;
	}
	else
	{
		double fade = ((double)m_nFadeoutEnd - (double)dtNow) / (double)m_nFadeoutTime;
		m_blend.SourceConstantAlpha = (byte)(255 * fade);

		UpdateLayeredWindow(hWnd, NULL, NULL, NULL, NULL, NULL, RGB(0, 0, 0), &m_blend, ULW_ALPHA);
		return false;
	}

}
std::atomic<bool> interruptAnimation(false);
std::mutex animationMutex;
bool exitanimation = false;
inline DWORD CSplashScreen::PumpMsgWaitForMultipleObjects(HWND hWnd, DWORD nCount, LPHANDLE pHandles, DWORD dwMilliseconds, HBITMAP combinedBitmapAnim[], HBITMAP combinedBitmapAnimas[])
{
	// useful variables
	const DWORD dwStartTickCount = ::GetTickCount64();
	// loop until done
	if (exitanimation == false) {
		// Добавить блокировку мьютекса перед вызовом AnimationCycle
		std::lock_guard<std::mutex> lock(animationMutex);
		int arraySize = 16;

		// Create a thread for the animation
		std::thread animationThread([&]() {
			while (true) {
				AnimationCycle(hWnd, combinedBitmapAnim, arraySize);
			}
		});
		// Detach the thread if you don't need to wait for it to finish
		animationThread.detach();
	}
	for (;;)
	{
		const DWORD dwElapsed = GetTickCount64() - dwStartTickCount;
		const DWORD dwTimeout = dwMilliseconds == INFINITE ? INFINITE : dwElapsed < dwMilliseconds ? dwMilliseconds - dwElapsed : 0;

		// wait for a handle to be signaled or a message
		const DWORD dwWaitResult = MsgWaitForMultipleObjects(nCount, pHandles, FALSE, dwTimeout, QS_ALLINPUT);

		if (dwWaitResult == WAIT_OBJECT_0 + nCount)
		{
			// pump messages
			MSG msg;
			
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != FALSE)
			{
				
				if (msg.message == WM_QUIT)
				{
					// repost quit message and return
					PostQuitMessage((int)msg.wParam);
					return WAIT_OBJECT_0 + nCount;// Прервать выполнение анимации
					interruptAnimation.store(true);
					exitanimation = true;
				}
				
				// dispatch thread message
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (dwWaitResult == WAIT_OBJECT_0)
		{
			interruptAnimation.store(true);
			exitanimation = true;
			Return:
			MSG msg;
			// timeout on actual wait or any other object
			SetTimer(hWnd, 1, 30, NULL);
			m_nFadeoutEnd = GetTickCount64() + m_nFadeoutTime;
			BOOL bRet;

			while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0)
			{
				
				if (bRet == -1)
				{
					// handle the error and possibly exit
				}
				else
				{
					if (msg.message == WM_TIMER)
					{
						interruptAnimation.store(true);
						exitanimation = true;
						if (FadeWindowOut(hWnd))
						{ // finished
							return dwWaitResult;
						}
					}
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		else
		{
			interruptAnimation.store(true);
			exitanimation = true;
			goto Return;
		}
	}
}
int CSplashScreen::AnimationCycle(HWND hWnd, HBITMAP combinedBitmapAnim[], int arraySize)
{
	for (int i = 0; i < arraySize; i++, Sleep(1)) {
		// Проверка флага прерывания анимации
		if (interruptAnimation.load())
		{
			return 0; // Выход из функции
		}
		if (combinedBitmapAnim[0] != NULL) {
			std::thread splashThread([this, hWnd, bitmap = combinedBitmapAnim[i]]() {
				SetSplashImage(hWnd, bitmap);
				});
			splashThread.detach();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	return 0;
}

void CSplashScreen::Show()
{
	CoInitialize(0);
	// create the named close splash screen event, making sure we're the first process to create it
	SetLastError(ERROR_SUCCESS);

	std::basic_string<TCHAR> strEvent1 = _T("CloseSplashScreenEvent") + m_strPrefix;
	HANDLE hCloseSplashEvent = CreateEvent(NULL, TRUE, FALSE, strEvent1.c_str());

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		ExitProcess(0);
	}

	std::basic_string<TCHAR> strEvent2 = _T("CloseSplashScreenWithoutFadeEvent") + m_strPrefix;
	HANDLE hCloseSplashWithoutFadeEvent = CreateEvent(NULL, TRUE, FALSE, strEvent2.c_str());

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		ExitProcess(0);
	}

	HBITMAP hbFrames[16];
	int numFrames = 0;
	for (int i = 0; i < 16; i++) {
		if (m_pLoadImgLoader[i] != NULL) {
			hbFrames[numFrames] = m_pLoadImgLoader[i]->LoadSplashImage();
			numFrames++;
		}
	}

	HWND wnd = NULL;

	RegisterWindowClass();
	TCHAR szCurrentFolder[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szCurrentFolder, MAX_PATH);

	PathRemoveFileSpec(szCurrentFolder);

	// add the application name to the path
	TCHAR szApplicationPath[MAX_PATH];
	if (m_strFullPath.length() > 0) {
		lstrcpy(szApplicationPath, m_strFullPath.c_str());
	}
	else {
		PathCombine(szApplicationPath, szCurrentFolder, m_strAppFileName.c_str());
	}

	// check if the file exists
	if (!PathFileExists(szApplicationPath))
	{
		std::wstring startappname(m_strAppFileName.c_str());
		std::wstring concatted_stdstr = L"The required application file \"" + startappname + L"\" was not found! The installation seems to be corrupted.";
		LPCWSTR concatted = concatted_stdstr.c_str();

		MessageBox(NULL, concatted, (LPCWSTR)L"FrontFace Assistant", MB_ICONERROR | MB_OK);
	}
	// Get version info
	DWORD verHandle = 0;
	UINT size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD verSize = GetFileVersionInfoSize(szApplicationPath, &verHandle);
	TCHAR version_buffer[32] = {};
	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfo(szApplicationPath, verHandle, verSize, verData))
		{
			if (VerQueryValue(verData, L"\\", (VOID FAR * FAR*) & lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{
						DWORD v_major = (verInfo->dwProductVersionMS >> 16) & 0xffff;
						DWORD v_minor = (verInfo->dwProductVersionMS >> 0) & 0xffff;
						DWORD v_build = (verInfo->dwProductVersionLS >> 16) & 0xffff;

#if defined(PREVIEW_BUILD) || defined(_DEBUG)
						swprintf_s(version_buffer, _T("Version %u.%u.%u (PREVIEW)"), v_major, v_minor, v_build);
#else
						swprintf_s(version_buffer, _T("Version %u.%u.%u"), v_major, v_minor, v_build);
#endif
					}
				}
			}
		}
		delete[] verData;
	}

	HBITMAP combinedBitmapAnim[16];

	HBITMAP hb1 = m_pImgLoader->LoadSplashImage();

if (hb1 != NULL) {
	wnd = CreateSplashWindow();
	SetSplashImage(wnd, hb1);
}
	for (int i = 0; i < numFrames; i++) {
		HBITMAP frame = hbFrames[i];
		HBITMAP combinedBitmapText = CombineTextBitmaps(hb1, std::wstring(version_buffer));
		HBITMAP combinedBitmap = CombineBitmaps(combinedBitmapText, frame);
		combinedBitmapAnim[i] = combinedBitmap;
	}
	std::thread splashThread([this, wnd, combinedBitmapAnim]() {
		SetSplashImage(wnd, combinedBitmapAnim[0]);
		});
	splashThread.detach();
	HANDLE hProcess = LaunchWpfApplication(wnd, *combinedBitmapAnim);
	AllowSetForegroundWindow(GetProcessId(hProcess));
	if (wnd != NULL) {
		HANDLE handles[3] = { hProcess, hCloseSplashEvent, hCloseSplashWithoutFadeEvent };
		PumpMsgWaitForMultipleObjects(wnd, 3, &handles[0], INFINITE, combinedBitmapAnim, combinedBitmapAnim);
	}
	CloseHandle(hCloseSplashEvent);
	CloseHandle(hCloseSplashWithoutFadeEvent);
	UnregisterWindowClass();
}

void CSplashScreen::CloseSplashScreen(HWND wnd)
{
	DestroyWindow(wnd);
}

HBITMAP CSplashScreen::CombineBitmaps(HBITMAP hb1, HBITMAP hb2) {
	BITMAP bm1, bm2;

	// Get information about the first bitmap
	GetObject(hb1, sizeof(BITMAP), &bm1);

	// Get information about the second bitmap
	GetObject(hb2, sizeof(BITMAP), &bm2);

	// Calculate the center position within the combined bitmap
	int centerX = (bm1.bmWidth - bm2.bmWidth) / 2;
	int centerY = (bm1.bmHeight - bm2.bmHeight) / 2;

	// Create a new bitmap with the combined dimensions
	HBITMAP hCombinedBitmap = CreateCompatibleBitmap(GetDC(NULL), bm1.bmWidth, bm1.bmHeight);

	// Create device contexts for the source bitmaps and the combined bitmap
	HDC hdcSrc1 = CreateCompatibleDC(NULL);
	HDC hdcSrc2 = CreateCompatibleDC(NULL);
	HDC hdcDest = CreateCompatibleDC(NULL);

	// Select the bitmaps into their respective device contexts
	HBITMAP hOld1 = (HBITMAP)SelectObject(hdcSrc1, hb1);
	HBITMAP hOld2 = (HBITMAP)SelectObject(hdcSrc2, hb2);
	HBITMAP hOldDest = (HBITMAP)SelectObject(hdcDest, hCombinedBitmap);

	// Copy the first bitmap to the combined bitmap
	BitBlt(hdcDest, 0, 0, bm1.bmWidth, bm1.bmHeight, hdcSrc1, 0, 0, SRCCOPY);

	// Set up alpha blending parameters
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255;  // Use full opacity (255) for the second bitmap
	blendFunc.AlphaFormat = AC_SRC_ALPHA;

	// Perform alpha blending of the second bitmap onto the combined bitmap
	// and set animation posititon
	AlphaBlend(hdcDest, centerX, centerY+40, bm2.bmWidth, bm2.bmHeight, hdcSrc2, 0, 0, bm2.bmWidth, bm2.bmHeight, blendFunc);

	// Clean up and release the device contexts
	SelectObject(hdcSrc1, hOld1);
	SelectObject(hdcSrc2, hOld2);
	SelectObject(hdcDest, hOldDest);

	DeleteDC(hdcSrc1);
	DeleteDC(hdcSrc2);
	DeleteDC(hdcDest);

	return hCombinedBitmap;
}
HBITMAP CSplashScreen::CombineTextBitmaps(HBITMAP hb1, const std::wstring& text)
{
	BITMAP bm1;
	POINT ptZero = { 0 };

	// Get information about the first bitmap
	GetObject(hb1, sizeof(bm1), &bm1);

	// Calculate the size of the combined bitmap
	SIZE combinedSize = { bm1.bmWidth, bm1.bmHeight };

	// Create a new bitmap with the combined dimensions
	HBITMAP hCombinedBitmap = CreateCompatibleBitmap(GetDC(NULL), combinedSize.cx, combinedSize.cy);

	// Create device contexts for the source bitmap and the combined bitmap
	HDC hdcSrc = CreateCompatibleDC(NULL);
	HDC hdcDest = CreateCompatibleDC(NULL);

	// Select the bitmaps into their respective device contexts
	HBITMAP hOldSrc = (HBITMAP)SelectObject(hdcSrc, hb1);
	HBITMAP hOldDest = (HBITMAP)SelectObject(hdcDest, hCombinedBitmap);

	// Copy the first bitmap to the combined bitmap
	BitBlt(hdcDest, 0, 0, combinedSize.cx, combinedSize.cy, hdcSrc, 0, 0, SRCCOPY);

	// Create a text bitmap for the specified text and font
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 16;  // Font height in pixels
	lf.lfWeight = FW_BOLD;  // Font weight
	wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Arial");  // Font name

	HFONT hFont = CreateFontIndirect(&lf);
	SelectObject(hdcDest, hFont);
	// Calculate the position to place the text bitmap
	RECT textRect;
	textRect.left = 200;  // adjusting the position of the text, x
	textRect.top = 405;  // adjusting the position of the text, y
	textRect.right = combinedSize.cx;
	textRect.bottom = combinedSize.cy;
	SetTextColor(hdcDest, 0x00000000);
	SetBkColor(hdcDest, TRANSPARENT);
	SetBkMode(hdcDest, TRANSPARENT);
	DrawText(hdcDest, text.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Create a text bitmap with alpha blending
	HDC hdcText = CreateCompatibleDC(NULL);
	HBITMAP hbText = CreateCompatibleBitmap(hdcDest, combinedSize.cx, combinedSize.cy);
	HBITMAP hOldText = (HBITMAP)SelectObject(hdcText, hbText);
	BitBlt(hdcText, 0, 0, combinedSize.cx, combinedSize.cy, hdcDest, 0, 0, SRCCOPY);

	// Define the blending parameters
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 128;  // Adjust the alpha value as desired (0-255)
	blendFunc.AlphaFormat = AC_SRC_ALPHA;

	// Perform alpha blending
	AlphaBlend(hdcDest, 0, 0, combinedSize.cx, combinedSize.cy, hdcText, 0, 0, combinedSize.cx, combinedSize.cy, blendFunc);

	// Clean up the text-related resources
	SelectObject(hdcText, hOldText);
	DeleteObject(hbText);
	DeleteDC(hdcText);

	// Clean up the font
	DeleteObject(hFont);

	// Clean up and release the device contexts
	SelectObject(hdcSrc, hOldSrc);
	SelectObject(hdcDest, hOldDest);
	DeleteDC(hdcSrc);
	DeleteDC(hdcDest);

	// Return the combined bitmap
	return hCombinedBitmap;
}

void CSplashScreen::SetFullPath(LPCTSTR lpszPath) {
	m_strFullPath = lpszPath;
}

