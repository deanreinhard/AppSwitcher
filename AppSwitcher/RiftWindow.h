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

	// mostly copied from Deskope https://github.com/AngelJA/Deskope
	public class RiftWindow
	{
	public:
		RiftWindow(String ^device)
		{
			DEVMODEW devmode;
			pin_ptr<const wchar_t> wch = PtrToStringChars(device);
			EnumDisplaySettingsW(wch, ENUM_CURRENT_SETTINGS, &devmode);


			WNDCLASSW wc = {0};
			wc.hInstance = GetModuleHandle(NULL);
			wc.lpfnWndProc = reinterpret_cast<WNDPROC>(WndProc);
			wc.lpszClassName = L"Deskope Window Class";
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);

			RegisterClassW(&wc);
			m_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW , // | WS_EX_TRANSPARENT,
				L"Deskope Window Class",
				L"Deskope Window",
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

		LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
		{	
			switch (message)
			{	
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			}
			return DefWindowProc(m_hwnd, message, wParam, lParam);
		}


		UINT_PTR m_uipCaptureTimer; // ID of timer for capturing the screen
		UINT_PTR m_uipDrawTimer;	// ID of timer for drawing the Scope window
		UINT_PTR m_uipSendRSDTimer;	// ID of timer for sending Rift sensor data to caller window
	public:
		HWND m_hwnd;				// hwnd for Scope window
		HWND m_hwndCaller;			// hwnd to caller window
		CURSORINFO m_GlobalCursor;	// mouse cursor info
		DEVMODE m_RiftDisplayInfo;
		HDC m_winDC;				// screen DC
		HDC m_winCopyDC;			// copy of screen for drawing on
		HBITMAP m_winCopyBM;		// copy of screen for drawing on
	public: HDC m_hdc;					// Scope window DC
		HDC m_BackDC;				// back buffer DC
		HBITMAP m_BackBM;			// back buffer BM


	};
}


