#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "shlwapi")

#include <windows.h>
#include <shlwapi.h>
#include <ctype.h>

TCHAR szClassName[] = TEXT("Window");

COLORREF GetColor(LPCTSTR lpszText)
{
	LPTSTR p = (LPTSTR)lpszText;
	int counter = 0;
	int r = 255;
	int g = 255;
	int b = 255;

	while (*p)
	{
		if (iswdigit(*p))
		{
			const int nValue = StrToInt(p);
			if (counter == 0)
			{
				r = min(nValue, 255);
				++counter;
				while (iswdigit(*(p + 1)))++p;
			}
			else if (counter == 1)
			{
				g = min(nValue, 255);
				++counter;
				while (iswdigit(*(p + 1)))++p;
			}
			else if (counter == 2)
			{
				b = min(nValue, 255);
				break;
			}
		}
		++p;
	}

	return RGB(r, g, b);
}

COLORREF GetComplementColor(COLORREF color)
{
	const int r = GetRValue(color);
	const int g = GetGValue(color);
	const int b = GetBValue(color);
	const int range = 60;
	const int minRange = 128 - range;
	const int maxRange = 128 + range;
	if ((minRange < r && r < maxRange) && (minRange < g && g < maxRange) && (minRange < b && b < maxRange))
	{
		if (r < 128 && g < 128 && b < 128)
			color = 0xffffff;
		else
			color = 0x0;
	}
	else
	{
		color = 0xffffff ^ color;
	}
	return color;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	static HBRUSH hBrush;
	static COLORREF color;
	switch (msg)
	{
	case WM_CREATE:
		color = RGB(255, 255, 255);
		hBrush = CreateSolidBrush(color);
		hEdit = CreateWindow(TEXT("EDIT"), TEXT("255 255 255"), WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		break;
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 100 && HIWORD(wParam) == EN_CHANGE)
		{
			const int nSize = GetWindowTextLength(hEdit);
			if (nSize > 0)
			{
				LPTSTR lpszText = (LPTSTR)GlobalAlloc(0, sizeof(TCHAR) * (nSize + 1));
				GetWindowText(hEdit, lpszText, nSize + 1);
				color = GetColor(lpszText);
				GlobalFree(lpszText);
				if (color == -1)
				{
					color = RGB(255, 255, 255);
				}
				else
				{
					DeleteObject(hBrush);
					hBrush = CreateSolidBrush(color);
					InvalidateRect(hEdit, 0, 1);
				}
			}
			else
			{
				DeleteObject(hBrush);
				color = RGB(255, 255, 255);
				hBrush = CreateSolidBrush(RGB(255, 255, 255));
				InvalidateRect(hEdit, 0, 1);
			}
		}
		break;
	case WM_CTLCOLOREDIT:
		SetTextColor((HDC)wParam, GetComplementColor(color));
		SetBkMode((HDC)wParam, TRANSPARENT);
		return(INT_PTR)hBrush;
	case WM_DESTROY:
		DeleteObject(hBrush);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		0,
		WndProc,
		0,
		0,
		hInstance,
		0,
		0,
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("What Color?"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
