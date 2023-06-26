#pragma once
#include "imageloader.h"
#include <iostream>
#include <string>

#define ENABLE_TRACE  //can depend on _DEBUG or NDEBUG macros
#include "trace.h"

class CSplashScreen
{
private:
	// Window Class name
	std::basic_string <TCHAR> m_strSplashClass;
	HINSTANCE m_hInstance;
	BLENDFUNCTION m_blend ;
	DWORD m_nFadeoutEnd ;
	DWORD m_nFadeoutTime ;
	int m_intArraySize;
	std::basic_string <TCHAR> m_strFullPath;
	std::basic_string <TCHAR> m_strPrefix;
	std::basic_string <TCHAR> m_strAppFileName;
	CImageLoader *m_pImgLoader;
	CImageLoader* m_pLoadImgLoader[];
	
private:
	HBITMAP CreateAlphaTextBitmap(LPCWSTR inText, HFONT inFont, COLORREF inColour);
	void SetSplashImage(HWND hwndSplash, HBITMAP hbmpComSplash);

	void SetSplashImage(HWND hwndSplash, HBITMAP hbmpComSplash, const std::wstring& text, int x, int y);
	void RegisterWindowClass();
	void UnregisterWindowClass();
	HWND CreateSplashWindow();
	HANDLE LaunchWpfApplication(HWND hwndSplash, HBITMAP hbmpSplash);
	
	bool FadeWindowOut(HWND hwnd);
	inline DWORD PumpMsgWaitForMultipleObjects(HWND hWnd, DWORD nCount, LPHANDLE pHandles, DWORD dwMilliseconds, HBITMAP hbmpComSplash[], HBITMAP combinedBitmapAnimas[]);
public:
	CSplashScreen(HINSTANCE hInstance, DWORD nFadeoutTime, CImageLoader *pImgLoader,
		CImageLoader* imageLoaders[],
		LPCTSTR lpszPrefix, LPCTSTR lpszAppFileName);
	~CSplashScreen(void);
	HBITMAP CSplashScreen::CombineBitmaps(HBITMAP hb1, HBITMAP hb2);
	HBITMAP CSplashScreen::CombineTextBitmaps(HBITMAP hb1, const std::wstring& text);
	int CSplashScreen::AnimationCycle(HWND hWnd, HBITMAP combinedBitmapAnim[], int arraySize);
	void SetFullPath(LPCTSTR lpszPath) ;
	void CSplashScreen::Show();
	void CSplashScreen::CloseSplashScreen(HWND wnd);
};


