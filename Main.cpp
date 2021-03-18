// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c  

#include <windows.h>  
#include <stdlib.h> 
#include <stdio.h>
#include <strsafe.h>
#include <string.h>  
#include <tchar.h>  
#include <assert.h>
#include "definitions.h"
#include "draw.h"


#define WIDTH 800
#define HEIGHT 800

// Global variables  

// The main window class name.  
static TCHAR szWindowClass[] = _T("win32app");

// The string that appears in the application's title bar.  
static TCHAR szTitle[] = _T("CS474 PEX Framework");

HINSTANCE hInst;

HDC hdcMem;
HBITMAP hbmMem;
HANDLE hOld;

PAINTSTRUCT ps;
HDC hdc;

Scene scene;

char fileNameBuff[255];

bool timeToDraw = false;
bool fileRead = false;

// Forward declarations of functions included in this code module:  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

void CreateMainMenu(HWND hWnd)
{
	HMENU hMenu = CreateMenu();
	HMENU hSubMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, L"File");

	AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, L"Open Scene");
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_RELOAD, L"Reload Scene");
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_DRAW, L"Draw");
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, L"Exit");
	SetMenu(hWnd, hMenu);
}

BOOL SaveBitmapFile(HDC p_hDC, LPCTSTR p_pchFileName)
{

	HBITMAP hBmp = (HBITMAP)GetCurrentObject(p_hDC, OBJ_BITMAP);

	BITMAPINFO stBmpInfo;
	stBmpInfo.bmiHeader.biSize = sizeof(stBmpInfo.bmiHeader);
	stBmpInfo.bmiHeader.biBitCount = 0;
	GetDIBits(p_hDC, hBmp, 0, 0, NULL, &stBmpInfo, DIB_RGB_COLORS);

	ULONG iBmpInfoSize;
	switch (stBmpInfo.bmiHeader.biBitCount)
	{
	case 24:
		iBmpInfoSize = sizeof(BITMAPINFOHEADER);
		break;
	case 16:
	case 32:
		iBmpInfoSize = sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 3;
		break;
	default:
		iBmpInfoSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1i64 << stBmpInfo.bmiHeader.biBitCount);
		break;
	}

	PBITMAPINFO pstBmpInfo;
	if (iBmpInfoSize != sizeof(BITMAPINFOHEADER))
	{
		pstBmpInfo = (PBITMAPINFO)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, iBmpInfoSize);
		PBYTE pbtBmpInfoDest = (PBYTE)pstBmpInfo;
		PBYTE pbtBmpInfoSrc = (PBYTE)&stBmpInfo;
		ULONG iSizeTmp = sizeof(BITMAPINFOHEADER);

		while (iSizeTmp--)
		{
			*((pbtBmpInfoDest)++) = *((pbtBmpInfoSrc)++);
		}
	}

	HANDLE hFile = CreateFile(p_pchFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);

	BITMAPFILEHEADER stBmpFileHder;
	stBmpFileHder.bfType = 0x4D42; // 'BM' 
	stBmpFileHder.bfSize = sizeof(BITMAPFILEHEADER)	+ sizeof(BITMAPINFOHEADER) + iBmpInfoSize + pstBmpInfo->bmiHeader.biSizeImage;
	stBmpFileHder.bfReserved1 = 0;
	stBmpFileHder.bfReserved2 = 0;
	stBmpFileHder.bfOffBits = sizeof(BITMAPFILEHEADER) + iBmpInfoSize;

	DWORD dRet;
	WriteFile(hFile, (LPCVOID)&stBmpFileHder, sizeof(BITMAPFILEHEADER), &dRet, NULL);

	PBYTE pBits	= (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, stBmpInfo.bmiHeader.biSizeImage);

	HBITMAP hBmpOld;
	HBITMAP hTmpBmp	= CreateCompatibleBitmap(p_hDC, pstBmpInfo->bmiHeader.biWidth, pstBmpInfo->bmiHeader.biHeight);
	hBmpOld = (HBITMAP)SelectObject(p_hDC, hTmpBmp);
	GetDIBits(p_hDC, hBmp, 0, pstBmpInfo->bmiHeader.biHeight, (LPSTR)pBits, pstBmpInfo, DIB_RGB_COLORS);

	WriteFile(hFile, (LPCVOID)pstBmpInfo, iBmpInfoSize, &dRet, NULL);

	WriteFile(hFile, (LPCVOID)pBits, pstBmpInfo->bmiHeader.biSizeImage, &dRet, NULL);

	SelectObject(p_hDC, hBmpOld);
	DeleteObject(hTmpBmp);
	CloseHandle(hFile);
	GlobalFree(pstBmpInfo);
	GlobalFree(pBits);
	return TRUE;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	scene.clear();
	
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("CS474 PEX Framework"),
			NULL);

		return 1;
	}

	hInst = hInstance; // Store instance handle in our global variable  

					   // The parameters to CreateWindow explained:  
					   // szWindowClass: the name of the application  
					   // szTitle: the text that appears in the title bar  
					   // WS_OVERLAPPEDWINDOW: the type of window to create  
					   // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)  
					   // 500, 100: initial size (width, length)  
					   // NULL: the parent of this window  
					   // NULL: this application does not have a menu bar  
					   // hInstance: the first parameter from WinMain  
					   // NULL: not used in this application  
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		100, 100,
		WIDTH+16, HEIGHT+59,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("CS474 PEX Framework"),
			NULL);

		return 1;
	}

	CreateMainMenu(hWnd);

	// The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);



	// Main message loop:  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

//  
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)  
//  
//  PURPOSE:  Processes messages for the main window.  
//  
//  WM_PAINT    - Paint the main window  
//  WM_DESTROY  - post a quit message and return  
//  
//  
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int ret;
	HBITMAP output;

	switch (message)
	{
	case WM_PAINT:
		if (timeToDraw)
		{
			hdc = BeginPaint(hWnd, &ps);

			hdcMem = CreateCompatibleDC(hdc);
			hbmMem = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);

			hOld = SelectObject(hdcMem, hbmMem);


			draw(WIDTH, HEIGHT, hdcMem, scene);
			SaveBitmapFile(hdcMem, L"output.bmp");

			ret = BitBlt(hdc, 0, 0, WIDTH, HEIGHT, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hOld);
			DeleteObject(hbmMem);
			DeleteDC(hdcMem);

			EndPaint(hWnd, &ps);

			timeToDraw = false;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_RELOAD:
			if (scene.read(fileNameBuff))
			{
				if (!timeToDraw)
				{
					timeToDraw = true;
					InvalidateRect(hWnd, NULL, true);
				}
				fileRead = true;
			}
			else {
				fileRead = false;
			}
			break;
		case ID_FILE_OPEN:

			OPENFILENAME ofn;
			wchar_t wSzFile[255];

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = wSzFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(wSzFile);
			ofn.lpstrFilter = L"All Files\0*.*\0\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn))
			{
				wcstombs(fileNameBuff, ofn.lpstrFile, 255);
				if (scene.read(fileNameBuff))
				{
					if (!timeToDraw)
					{
						timeToDraw = true;
						InvalidateRect(hWnd, NULL, true);
					}
					fileRead = true;					
				}
				else {
					fileRead = false;
				}
			}
			break;

		case ID_FILE_DRAW:
			if (!timeToDraw)
			{
				timeToDraw = true;
				InvalidateRect(hWnd, NULL, true);
			}

			break;
		case ID_FILE_EXIT: 
			PostQuitMessage(0);
			break;
		default:
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}