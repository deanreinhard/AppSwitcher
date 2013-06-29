#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
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

			// prepare Direct2D
			direct2DFactory = nullptr;
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2DFactory);
			RECT rect;
			GetClientRect(m_hwnd, &rect);

			renderTarget = nullptr;
			direct2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(rect.right-rect.left, rect.bottom-rect.top)),
				&renderTarget);
		}

	protected:
		~RiftWindow()
		{
			if(renderTarget)
				renderTarget->Release();

			if(direct2DFactory)
				direct2DFactory->Release();
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

		HBITMAP CaptureScreen(int x0, int y0){
			HDC dc_screen = GetDC(0);
			HDC dc_bitmap = CreateCompatibleDC(dc_screen);

			HBITMAP bitmap = CreateCompatibleBitmap(dc_screen, 1280, 800);
			SelectObject(dc_bitmap, bitmap);

			BitBlt(dc_bitmap, 0, 0, 1280, 800, dc_screen, x0, y0, SRCCOPY);

			ReleaseDC(0, dc_screen);
			DeleteDC(dc_bitmap);

			return bitmap;
		}

		void RenderFrame(){
			const int sz = 100;
			const int d = 70;

			// capture portion of screen
			ID2D1Bitmap* bitmap_d2d = nullptr;
			if(window_app){
				POINT pt;
				pt.x = 0;
				pt.y = 0;
				ClientToScreen(window_app, &pt);

				HBITMAP bitmap = CaptureScreen(pt.x, pt.y);

				// convert to D2D bitmap
				IWICImagingFactory* iFactory = nullptr;
				CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<LPVOID*>(&iFactory));

				IWICBitmap* bitmap_iwic = nullptr;
				iFactory->CreateBitmapFromHBITMAP(bitmap, NULL, WICBitmapAlphaChannelOption::WICBitmapIgnoreAlpha, &bitmap_iwic);

				renderTarget->CreateBitmapFromWicBitmap(bitmap_iwic, &bitmap_d2d);

				bitmap_iwic->Release();
				iFactory->Release();
				DeleteObject(bitmap);
			}

			// render
			ID2D1SolidColorBrush* brush = nullptr;
			renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Beige), &brush);

			renderTarget->BeginDraw();

			if(bitmap_d2d){
				renderTarget->DrawBitmap(bitmap_d2d);
				bitmap_d2d->Release();
			}

			renderTarget->DrawRectangle(D2D1::RectF(320+d-sz, 250, 320+d+sz, 400), brush);
			renderTarget->DrawRectangle(D2D1::RectF(640+320-d-sz, 250, 640+320-d+sz, 400), brush);
					
			renderTarget->EndDraw();

			if(brush)
				brush->Release();
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
	private:
		ID2D1Factory* direct2DFactory;
		ID2D1HwndRenderTarget* renderTarget;
	};
}


