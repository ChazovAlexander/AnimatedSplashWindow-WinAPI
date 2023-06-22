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
	std::basic_string <TCHAR> m_strFullPath;
	std::basic_string <TCHAR> m_strPrefix;
	std::basic_string <TCHAR> m_strAppFileName;
	CImageLoader *m_pImgLoader;
	CImageLoader *m_pLoadImgLoader;
	CImageLoader *m_pLoadImgLoader2;
	CImageLoader *m_pLoadImgLoader3;
	CImageLoader *m_pLoadImgLoader4;
	CImageLoader *m_pLoadImgLoader5;
	CImageLoader *m_pLoadImgLoader6;
	CImageLoader *m_pLoadImgLoader7;
	CImageLoader *m_pLoadImgLoader8;
	CImageLoader *m_pLoadImgLoader9;
	CImageLoader *m_pLoadImgLoader10;
	CImageLoader *m_pLoadImgLoader11;
	CImageLoader *m_pLoadImgLoader12;
	CImageLoader *m_pLoadImgLoader13;
	CImageLoader *m_pLoadImgLoader14;
	CImageLoader *m_pLoadImgLoader15;
	CImageLoader *m_pLoadImgLoader16;
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
		CImageLoader* pLoadImgLoader,
		CImageLoader* pLoadImgLoader2,
		CImageLoader* pLoadImgLoader3,
		CImageLoader* pLoadImgLoader4,
		CImageLoader* pLoadImgLoader5,
		CImageLoader* pLoadImgLoader6,
		CImageLoader* pLoadImgLoader7,
		CImageLoader* pLoadImgLoader8,
		CImageLoader* pLoadImgLoader9,
		CImageLoader* pLoadImgLoader10,
		CImageLoader* pLoadImgLoader11,
		CImageLoader* pLoadImgLoader12,
		CImageLoader* pLoadImgLoader13,
		CImageLoader* pLoadImgLoader14,
		CImageLoader* pLoadImgLoader15,
		CImageLoader* pLoadImgLoader16,
		LPCTSTR lpszPrefix, LPCTSTR lpszAppFileName);
	~CSplashScreen(void);
	HBITMAP CSplashScreen::CombineBitmaps(HBITMAP hb1, HBITMAP hb2);
	HBITMAP CSplashScreen::CombineTextBitmaps(HBITMAP hb1, const std::wstring& text);
	int CSplashScreen::AnimationCycle(HWND hWnd, HBITMAP combinedBitmapAnim[]);
	void SetFullPath(LPCTSTR lpszPath) ;
	void CSplashScreen::Show();
	void CSplashScreen::CloseSplashScreen(HWND wnd);
};


