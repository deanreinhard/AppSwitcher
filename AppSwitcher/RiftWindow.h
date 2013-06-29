#pragma once
#include <Windows.h>
#include <vector>
#include <OVR.h>
#include <vcclr.h>

namespace AppSwitcher {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	// mostly copied from AppSwitcher https://github.com/AngelJA/AppSwitcher
	public class RiftWindow
	{
	public:
		RiftWindow(String ^device) : window_app(0)
		{
			DEVMODEW devmode;
			pin_ptr<const wchar_t> wch = PtrToStringChars(device);
			EnumDisplaySettingsW(wch, ENUM_CURRENT_SETTINGS, &devmode);


			WNDCLASSW wc = {0};
			wc.hInstance = GetModuleHandle(NULL);
			wc.lpfnWndProc = reinterpret_cast<WNDPROC>(WndProc);
			wc.lpszClassName = L"AppSwitcher Window Class";
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);

			RegisterClassW(&wc);
			m_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW , // | WS_EX_TRANSPARENT,
				L"AppSwitcher Window Class",
				L"AppSwitcher Window",
				WS_POPUP,
				devmode.dmPosition.x, devmode.dmPosition.y,
				devmode.dmPelsWidth, devmode.dmPelsHeight,
				NULL,
				NULL,
				GetModuleHandle(NULL),
				this);	//pass the window a pointer to this Scope object
			ShowWindow(m_hwnd, SW_SHOW);
		}

	protected:
		~RiftWindow()
		{
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			RiftWindow * pThis = NULL;
			if (message == WM_CREATE)
			{
				CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
				pThis = reinterpret_cast<RiftWindow*>(pCreate->lpCreateParams);
				SetWindowLongW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(pThis));

				pThis->m_hwnd = hwnd;
			}
			else
			{
				pThis = reinterpret_cast<RiftWindow*>(GetWindowLongW(hwnd, GWLP_USERDATA));
			}

			if (pThis)
			{
				return pThis->HandleMessage(message, wParam, lParam);
			}
			else
			{
				return DefWindowProcW(hwnd, message, wParam, lParam);
			}
		}

		void RenderFrame(){
			int sz = 100;
			int d = 70;

			PAINTSTRUCT ps;
			HDC dc_targ = BeginPaint(m_hwnd, &ps);

			
			if(window_app){
				POINT pt;
				pt.x = 0;
				pt.y = 0;
				ClientToScreen(window_app, &pt);

				HDC dc_all = GetDC(0);
				BitBlt(dc_targ, 0, 0, 1280, 800, dc_all, pt.x, pt.y, SRCCOPY);
				ReleaseDC(0, dc_all);
			}


			RoundRect(dc_targ, 320+d-sz, 250, 320+d+sz, 400, 3, 3);
			RoundRect(dc_targ, 640+320-d-sz, 250, 640+320-d+sz, 400, 3, 3);
			
			TextOutW(dc_targ, 320+d, 300, L"Yunalus", 7);
			TextOutW(dc_targ, 640+320-d, 300, L"Yunalus", 7);

			TextOutW(dc_targ, 320+d, 300+30, L"Mikulus", 7);
			TextOutW(dc_targ, 640+320-d, 300+30, L"Mikulus", 7);
			EndPaint(m_hwnd, &ps);
		}

		LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
		{	
			switch (message)
			{	
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_PAINT:
				RenderFrame();
				break;
			}
			return DefWindowProc(m_hwnd, message, wParam, lParam);
		}

	public:
		HWND m_hwnd;
		HWND window_app;
	};
}


