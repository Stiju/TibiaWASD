#define ISOLATION_AWARE_ENABLED 1
#include <Windows.h>
#include <commctrl.h>
#include <cstdio>
#include "ConfigWnd.h"
#include "Console.h"
#include "Settings.h"
#include "TibiaWASD.h"
#include "Updater.h"
#include "resource.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

WNDPROC wpEditProc;

enum {
	HWND_NORTHWEST,
	HWND_NORTH,
	HWND_NORTHEAST,
	HWND_WEST,
	HWND_EAST,
	HWND_SOUTHWEST,
	HWND_SOUTH,
	HWND_SOUTHEAST,
	HWND_F1,
	HWND_F2,
	HWND_F3,
	HWND_F4,
	HWND_F5,
	HWND_F6,
	HWND_F7,
	HWND_F8,
	HWND_F9,
	HWND_F10,
	HWND_F11,
	HWND_F12,
	HWND_MMB,
	HWND_XMB1,
	HWND_XMB2,
	HWND_WRITEMODE,
	HWND_CANCEL,
	HWND_CONNECTIONSTATUS,
	HWND_ACTIONSTATE,
	HWND_LAST
};

HWND g_hConfigWnd;
HWND g_hEditControls[HWND_LAST];

enum {
	CID_SAVE = 0x1000,
	CID_LOAD,
	CID_DEFAULT,
	CID_TITLE,
	CID_VERSION,
	CID_UPDATE
};

inline void SetControlKey(HWND hWnd, unsigned short key) {
	char buf[4];
	sprintf(buf, "%.2X", key);
	SetWindowText(hWnd, buf);
}

inline unsigned short GetControlKey(HWND hWnd) {
	char buf[4];
	GetWindowText(hWnd, buf, 4);
	return strtol(buf, 0, 16) & 0xFFFF;
}

inline void SetControlAddr(HWND hWnd, unsigned int addr) {
	char buf[12];
	sprintf(buf, "%.8X", addr);
	SetWindowText(hWnd, buf);
}

inline unsigned int GetControlAddr(HWND hWnd) {
	char buf[12];
	GetWindowText(hWnd, buf, 12);
	return strtol(buf, 0, 16) & 0xFFFFFFFF;
}

inline void SetCheckBox(int nIDButton, bool check) {
	HWND hWnd = GetDlgItem(g_hConfigWnd, nIDButton);
	if(hWnd)
		SendMessage(hWnd, BM_SETCHECK, check ? BST_CHECKED : BST_UNCHECKED, 0);
}

inline bool IsChecked(int nIDButton) {
	return IsDlgButtonChecked(g_hConfigWnd, nIDButton) == BST_CHECKED;
}

void ResetFields() {
	SetControlKey(g_hEditControls[HWND_NORTHWEST], settings->Config.Keys.NorthWest);
	SetControlKey(g_hEditControls[HWND_NORTH], settings->Config.Keys.North);
	SetControlKey(g_hEditControls[HWND_NORTHEAST], settings->Config.Keys.NorthEast);
	SetControlKey(g_hEditControls[HWND_WEST], settings->Config.Keys.West);
	SetControlKey(g_hEditControls[HWND_EAST], settings->Config.Keys.East);
	SetControlKey(g_hEditControls[HWND_SOUTHWEST], settings->Config.Keys.SouthWest);
	SetControlKey(g_hEditControls[HWND_SOUTH], settings->Config.Keys.South);
	SetControlKey(g_hEditControls[HWND_SOUTHEAST], settings->Config.Keys.SouthEast);

	SetControlKey(g_hEditControls[HWND_F1], settings->Config.Keys.F1);
	SetControlKey(g_hEditControls[HWND_F2], settings->Config.Keys.F2);
	SetControlKey(g_hEditControls[HWND_F3], settings->Config.Keys.F3);
	SetControlKey(g_hEditControls[HWND_F4], settings->Config.Keys.F4);
	SetControlKey(g_hEditControls[HWND_F5], settings->Config.Keys.F5);
	SetControlKey(g_hEditControls[HWND_F6], settings->Config.Keys.F6);
	SetControlKey(g_hEditControls[HWND_F7], settings->Config.Keys.F7);
	SetControlKey(g_hEditControls[HWND_F8], settings->Config.Keys.F8);
	SetControlKey(g_hEditControls[HWND_F9], settings->Config.Keys.F9);
	SetControlKey(g_hEditControls[HWND_F10], settings->Config.Keys.F10);
	SetControlKey(g_hEditControls[HWND_F11], settings->Config.Keys.F11);
	SetControlKey(g_hEditControls[HWND_F12], settings->Config.Keys.F12);

	SetControlKey(g_hEditControls[HWND_MMB], settings->Config.Keys.MMouseButton);
	SetControlKey(g_hEditControls[HWND_XMB1], settings->Config.Keys.XMouseButton1);
	SetControlKey(g_hEditControls[HWND_XMB2], settings->Config.Keys.XMouseButton2);

	SetControlKey(g_hEditControls[HWND_WRITEMODE], settings->Config.Keys.WriteMode);
	SetControlKey(g_hEditControls[HWND_CANCEL], settings->Config.Keys.Cancel);

	SetCheckBox(CID_TITLE, settings->Config.IsWasdTitle);
	SetCheckBox(CID_VERSION, settings->Config.IsVersionIndependent);

	SetControlAddr(g_hEditControls[HWND_CONNECTIONSTATUS], settings->CurrentVI.ConnectionStatus);
	SetControlAddr(g_hEditControls[HWND_ACTIONSTATE], settings->CurrentVI.ActionState);
}

void ApplySettings() {
	settings->Config.Keys.NorthWest = GetControlKey(g_hEditControls[HWND_NORTHWEST]);
	settings->Config.Keys.North = GetControlKey(g_hEditControls[HWND_NORTH]);
	settings->Config.Keys.NorthEast = GetControlKey(g_hEditControls[HWND_NORTHEAST]);
	settings->Config.Keys.West = GetControlKey(g_hEditControls[HWND_WEST]);
	settings->Config.Keys.East = GetControlKey(g_hEditControls[HWND_EAST]);
	settings->Config.Keys.SouthWest = GetControlKey(g_hEditControls[HWND_SOUTHWEST]);
	settings->Config.Keys.South = GetControlKey(g_hEditControls[HWND_SOUTH]);
	settings->Config.Keys.SouthEast = GetControlKey(g_hEditControls[HWND_SOUTHEAST]);

	settings->Config.Keys.F1 = GetControlKey(g_hEditControls[HWND_F1]);
	settings->Config.Keys.F2 = GetControlKey(g_hEditControls[HWND_F2]);
	settings->Config.Keys.F3 = GetControlKey(g_hEditControls[HWND_F3]);
	settings->Config.Keys.F4 = GetControlKey(g_hEditControls[HWND_F4]);
	settings->Config.Keys.F5 = GetControlKey(g_hEditControls[HWND_F5]);
	settings->Config.Keys.F6 = GetControlKey(g_hEditControls[HWND_F6]);
	settings->Config.Keys.F7 = GetControlKey(g_hEditControls[HWND_F7]);
	settings->Config.Keys.F8 = GetControlKey(g_hEditControls[HWND_F8]);
	settings->Config.Keys.F9 = GetControlKey(g_hEditControls[HWND_F9]);
	settings->Config.Keys.F10 = GetControlKey(g_hEditControls[HWND_F10]);
	settings->Config.Keys.F11 = GetControlKey(g_hEditControls[HWND_F11]);
	settings->Config.Keys.F12 = GetControlKey(g_hEditControls[HWND_F12]);

	settings->Config.Keys.MMouseButton = GetControlKey(g_hEditControls[HWND_MMB]);
	settings->Config.Keys.XMouseButton1 = GetControlKey(g_hEditControls[HWND_XMB1]);
	settings->Config.Keys.XMouseButton2 = GetControlKey(g_hEditControls[HWND_XMB2]);

	settings->Config.Keys.WriteMode = GetControlKey(g_hEditControls[HWND_WRITEMODE]);
	settings->Config.Keys.Cancel = GetControlKey(g_hEditControls[HWND_CANCEL]);

	settings->Config.IsWasdTitle = IsChecked(CID_TITLE);
	settings->Config.IsVersionIndependent = IsChecked(CID_VERSION);

	settings->CurrentVI.ConnectionStatus = GetControlAddr(g_hEditControls[HWND_CONNECTIONSTATUS]);
	settings->CurrentVI.ActionState = GetControlAddr(g_hEditControls[HWND_ACTIONSTATE]);
}

void InitTrayIcon(HICON hIcon) {
	NOTIFYICONDATA niData;
	memset(&niData, 0, sizeof(NOTIFYICONDATA));
	niData.cbSize = sizeof(niData);
	niData.hIcon = hIcon;
	niData.hWnd = g_hConfigWnd;
	niData.uCallbackMessage = WM_USER;
	niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	niData.uID = WM_USER;
	strcpy(niData.szTip, tibiaWasd);
	strcat(niData.szTip, " by Stiju.com");
	Shell_NotifyIcon(NIM_ADD, &niData);
}

void DestroyTrayIcon() {
	NOTIFYICONDATA niData;
	memset(&niData, 0, sizeof(NOTIFYICONDATA));
	niData.cbSize = sizeof(niData);
	niData.hWnd = g_hConfigWnd;
	niData.uID = WM_USER;
	Shell_NotifyIcon(NIM_DELETE, &niData);
}

LRESULT CALLBACK ConfigWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_COMMAND:
		{
			switch(LOWORD(wParam)) {
			case CID_SAVE:
				{
					ApplySettings();
					settings->Save();
					UpdateTitle();
					break;
				}
			case CID_LOAD:
				{
					settings->Load();
					ResetFields();
					UpdateTitle();
					break;
				}
			case CID_DEFAULT:
				{
					settings->LoadDefault();
					ResetFields();
					UpdateTitle();
					settings->Load();
					break;
				}
			case CID_TITLE:
				{
					settings->Config.IsWasdTitle = IsChecked(CID_TITLE);
					UpdateTitle();
					break;
				}
			case CID_VERSION:
				{
					settings->Config.IsVersionIndependent = IsChecked(CID_VERSION);
					break;
				}
			case CID_UPDATE:
				{
					static bool isDisabled = false;
					if(!isDisabled) {
						isDisabled = true;
						unsigned int addr;
						Updater u(g_hConfigWnd);
						addr = u.GetConnectionStatus();
						if(addr != 0) {
							settings->CurrentVI.ConnectionStatus = addr;
							SetControlAddr(g_hEditControls[HWND_CONNECTIONSTATUS], addr);
						}
						addr = u.GetActionState();
						if(addr != 0) {
							settings->CurrentVI.ActionState = addr;
							SetControlAddr(g_hEditControls[HWND_ACTIONSTATE], addr);
						}
						isDisabled = false;
					}
					break;
				}
			}
			break;
		}
	case WM_NOTIFY:
		{
			switch(((LPNMHDR)lParam)->code) {
			case NM_CLICK:
			case NM_RETURN:
				{
					PNMLINK pnmLink = (PNMLINK)lParam;
					LITEM item = pnmLink->item;
					if(item.iLink == 0) {
						ShellExecuteW(0, L"open", item.szUrl, 0, 0, SW_SHOWNORMAL);
					}
					break;
				}
			}
			break;
		}
	case WM_USER:
		{
			if(lParam == WM_LBUTTONDBLCLK) {
				if(IsWindowVisible(hWnd))
					ShowWindow(hWnd, SW_HIDE);
				else
					ShowWindow(hWnd, SW_SHOW);
			}
			return 0;
		}
	case WM_CLOSE:
		{
			ShowWindow(hWnd, SW_HIDE);
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT APIENTRY EditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_CHAR:
		{
			return 0;
		}
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			DbgLog("WM_KEYDOWN %.8X %.8X\n", wParam, lParam);
			if(wParam == VK_BACK)
				wParam = 0;
			SetControlKey(hWnd, (unsigned short)wParam);
			return 0;
		}
	case WM_SYSKEYUP:
	case WM_KEYUP:
		{
			return 0;
		}
	}
	return CallWindowProc(wpEditProc, hWnd, uMsg, wParam, lParam);
}

void InitConfigControls(HWND hWnd, HINSTANCE hInstance) {
	LOGFONT lf = {0};
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
	HFONT hFont = CreateFontIndirect(&lf);

	auto createWindow = [hWnd, hInstance, hFont](DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
		int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstanceNull, LPVOID lpParam)->HWND {
			UNREFERENCED_PARAMETER(hWndParent);
			UNREFERENCED_PARAMETER(hInstanceNull);
			HWND ret = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
				nWidth, nHeight, hWnd, hMenu, hInstance, lpParam);
			SendMessage(ret, WM_SETFONT, (WPARAM)hFont, true);
			return ret;
	};
	auto createEditEx = [createWindow](int X, int Y, int nWidth, int nHeight)->HWND {
		HWND ret = createWindow(WS_EX_STATICEDGE, "edit", 0, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_CENTER,
			X, Y, nWidth, nHeight, 0, (HMENU)0, 0, 0);
		wpEditProc = (WNDPROC)SetWindowLongPtr(ret, GWLP_WNDPROC, (LONG)EditProc);
		return ret;
	};
	auto createEdit = [createWindow](int X, int Y, int nWidth, int nHeight)->HWND {
		HWND ret = createWindow(WS_EX_STATICEDGE, "edit", 0, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_CENTER,
			X, Y, nWidth, nHeight, 0, (HMENU)0, 0, 0);
		return ret;
	};
	auto createStatic = [createWindow](LPCSTR lpWindowName, int X, int Y, int nWidth, int nHeight)->HWND {
		HWND ret = createWindow(0, "static", lpWindowName, WS_CHILD | WS_VISIBLE, X, Y,
			nWidth, nHeight, 0, (HMENU)0, 0, 0);
		return ret;
	};
	auto createButton = [createWindow](LPCSTR lpWindowName, int X, int Y, int nWidth, int nHeight, HMENU hMenu)->HWND {
		HWND ret = createWindow(0, "button", lpWindowName, WS_CHILD | WS_VISIBLE, X, Y,
			nWidth, nHeight, 0, (HMENU)hMenu, 0, 0);
		return ret;
	};
	auto createCheckBox = [createWindow](LPCSTR lpWindowName, int X, int Y, int nWidth, int nHeight, HMENU hMenu)->HWND {
		HWND ret = createWindow(0, "button", lpWindowName, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			X, Y, nWidth, nHeight, 0, (HMENU)hMenu, 0, 0);
		return ret;
	};

	createStatic("Movement", 10, 60, 100, 20);
	int it = 0;
	for(int j = 0; j < 3; j++) {
		for(int i = 0; i < 3; i++) {
			if(i == 1 && j == 1)
				continue;
			g_hEditControls[HWND_NORTHWEST + it++] = createEditEx(10 + (24 * i), 80 + (24 * j), 18, 18);
		}
	}
	for(int i = 0; i < 12; i++) {
		char buf[4];
		sprintf(buf, "F%d", i + 1);
		createStatic(buf, 10 + (24 * i), 10, 20, 20);
		g_hEditControls[HWND_F1 + i] = createEditEx(10 + (24 * i), 30, 18, 18);
	}

	createStatic("MB3", 100, 60, 30, 20);
	createStatic("MB4", 130, 60, 30, 20);
	createStatic("MB5", 160, 60, 30, 20);
	g_hEditControls[HWND_MMB] = createEditEx(100, 80, 18, 18);
	g_hEditControls[HWND_XMB1] = createEditEx(130, 80, 18, 18);
	g_hEditControls[HWND_XMB2] = createEditEx(160, 80, 18, 18);

	createStatic("Write", 100, 104, 60, 20);
	createStatic("Cancel", 100, 128, 60, 20);
	g_hEditControls[HWND_WRITEMODE] = createEditEx(160, 104, 18, 18);
	g_hEditControls[HWND_CANCEL] = createEditEx(160, 128, 18, 18);

	createCheckBox("WASD Title", 10, 150, 80, 20, (HMENU)CID_TITLE);
	createCheckBox("Version Independent", 100, 150, 130, 20, (HMENU)CID_VERSION);

	createButton("Save", 306, 10, 60, 20, (HMENU)CID_SAVE);
	createButton("Load", 306, 38, 60, 20, (HMENU)CID_LOAD);
	createButton("Default", 306, 66, 60, 20, (HMENU)CID_DEFAULT);
	createButton("Update", 306, 94, 60, 20, (HMENU)CID_UPDATE);

	createStatic("ConnectionStatus", 202, 60, 100, 20);
	createStatic("ActionState", 202, 104, 90, 20);
	g_hEditControls[HWND_CONNECTIONSTATUS] = createEdit(202, 80, 90, 18);
	g_hEditControls[HWND_ACTIONSTATE] = createEdit(202, 128, 90, 18);

	createWindow(0, "SysLink", "<a href=\"http://www.stiju.com\">Stiju.com</a>",
		WS_CHILD | WS_VISIBLE, 320, 160, 60, 20, 0, (HMENU)0, 0, 0);


	auto tooltip = [createWindow, hInstance](HWND hWnd, char *text)->void {
		static HWND tip = createWindow(0, "tooltips_class32", 0, TTS_ALWAYSTIP | TTS_NOPREFIX | TTS_NOANIMATE | TTS_NOFADE,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0);

		TOOLINFO toolInfo = {0};
		toolInfo.cbSize = sizeof(toolInfo);
		toolInfo.hwnd = hWnd;
		toolInfo.uFlags = TTF_SUBCLASS | TTF_CENTERTIP;
		toolInfo.hinst = hInstance;
		toolInfo.lpszText = text;

		GetClientRect(hWnd, &toolInfo.rect);

		SendMessage(tip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&toolInfo);
	};

	tooltip(g_hEditControls[HWND_NORTHWEST], "North-west");
	tooltip(g_hEditControls[HWND_NORTH], "North");
	tooltip(g_hEditControls[HWND_NORTHEAST], "North-east");
	tooltip(g_hEditControls[HWND_WEST], "West");
	tooltip(g_hEditControls[HWND_EAST], "East");
	tooltip(g_hEditControls[HWND_SOUTHWEST], "South-west");
	tooltip(g_hEditControls[HWND_SOUTH], "South");
	tooltip(g_hEditControls[HWND_SOUTHEAST], "South-east");

	tooltip(g_hEditControls[HWND_MMB], "Middle mouse button");
	tooltip(g_hEditControls[HWND_XMB1], "Mouse button 4");
	tooltip(g_hEditControls[HWND_XMB2], "Mouse button 5");

	tooltip(g_hEditControls[HWND_WRITEMODE], "Enter write mode");
	tooltip(g_hEditControls[HWND_CANCEL], "Exit write mode");
}

DWORD WINAPI ConfigWndThread(LPVOID lpParam) {
	HWND hWnd = 0;
	HINSTANCE hInstance = *(HINSTANCE*)lpParam;

	int width = 380, heigth = 200;
	int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - heigth) / 2;

	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)ConfigWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hIconSm = wcex.hIcon;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(16);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = tibiaWasd;

	if(!RegisterClassEx(&wcex))
		return 0;

	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, tibiaWasd, tibiaWasd,
		WS_SYSMENU | WS_CAPTION, x, y, width, heigth, 0, 0, hInstance, 0);
	g_hConfigWnd = hWnd;

	InitConfigControls(hWnd, hInstance);

	ResetFields();

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	InitTrayIcon(wcex.hIconSm);

	MSG msg;
	int ret;
	while((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if(ret != -1) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

void InitConfigWnd(void *hModule) {
	CreateThread(0, 0, ConfigWndThread, hModule, 0, 0);
}

void DestroyConfigWnd() {
	DestroyTrayIcon();
}