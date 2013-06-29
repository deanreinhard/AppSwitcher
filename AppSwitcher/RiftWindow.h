#pragma once
#include "AppConfig.h"
#include <gcroot.h>
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <vector>
#include <memory>
#include <OVR.h>
#include <vcclr.h>

namespace AppSwitcher {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace Runtime::InteropServices;
	
	enum EffectMode{
		ST_APP,
		ST_HOME,
		ST_ENTERING,
		ST_LEAVING
	};

	delegate void app_change_request(int);

	/// Stream video to rift by creating large window there.
	public class RiftWindow {
	public:
		RiftWindow(String^ device, app_change_request^ req) : window_app(nullptr), changed(false), mode(ST_HOME), alpha(0),
			hud_visible(true), hud_cursor(0), hud_cursor_running(-1), request_change(req) {

			hud_items = gcnew Generic::List<String^>();

			InitializeWindow(device);

			// InvalidateRect & WindowUpdate just set some internal flags, and actual # of WM_PAINT is lower.
			// So it's ok to call it more than possible to make it draw as often as possible.
			// See http://blogs.msdn.com/b/oldnewthing/archive/2011/12/19/10249000.aspx for detail.
			SetTimer(handle, 1, 10, nullptr); // 100 fps at most
			
			RegisterHotKey(handle, 0, MOD_CONTROL | MOD_SHIFT, 'Z');

			InitializeD2D();
		}
	private:
		// Create Window and blahblah. handle will be non-null after this.
		void InitializeWindow(String^ display_device){
			DEVMODEW devmode;
			pin_ptr<const wchar_t> wch = PtrToStringChars(display_device);
			EnumDisplaySettingsW(wch, ENUM_CURRENT_SETTINGS, &devmode);

			WNDCLASSW wc = {0};
			wc.hInstance = GetModuleHandle(NULL);
			wc.lpfnWndProc = reinterpret_cast<WNDPROC>(WndProc);
			wc.lpszClassName = L"AppSwitcher Window Class";
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);

			RegisterClassW(&wc);
			handle = CreateWindowExW(WS_EX_TOOLWINDOW , // | WS_EX_TRANSPARENT,
				L"AppSwitcher Window Class",
				L"AppSwitcher Window",
				WS_POPUP,
				devmode.dmPosition.x, devmode.dmPosition.y,
				devmode.dmPelsWidth, devmode.dmPelsHeight,
				nullptr,
				nullptr,
				GetModuleHandle(nullptr),
				this);	//pass the window a pointer to this Scope object
			ShowWindow(handle, SW_SHOW);
		}

		// requires handle to be set
		void InitializeD2D(){
			direct2DFactory = nullptr;
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2DFactory);
			RECT rect;
			GetClientRect(handle, &rect);

			renderTarget = nullptr;
			direct2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(handle, D2D1::SizeU(rect.right-rect.left, rect.bottom-rect.top)),
				&renderTarget);

			// create bitmap
			bitmap_d2d = nullptr;
			if(FAILED(renderTarget->CreateBitmap(
				D2D1::SizeU(1280,800),
				D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
				&bitmap_d2d)))
				throw gcnew NotSupportedException("Couldn't allocate B8G8R8A8 premultiplied-alpha bitmap");
		}

		~RiftWindow() {
			if(bitmap_d2d)
				bitmap_d2d->Release();

			if(renderTarget)
				renderTarget->Release();

			if(direct2DFactory)
				direct2DFactory->Release();
		}

	public:
		void NotifyAppChange(HWND app){
			window_app = app;
			changed = true; // "wait for content ready" flag
		}

		void NotifyAppTerminate(){
			window_app = nullptr;
			mode = ST_LEAVING;
		}

		void NotifyModelChange(Generic::List<AppConfig^>^ config){
			Generic::List<String^>^ items = hud_items;

			items->Clear();
			for each(AppConfig^ cfg in config){
				String^ name = cfg->path->Substring(cfg->path->LastIndexOf("\\")+1);
				items->Add(name);
			}
		}

	private:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			RiftWindow * pThis = NULL;
			if (message == WM_CREATE)
			{
				CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
				pThis = reinterpret_cast<RiftWindow*>(pCreate->lpCreateParams);
				SetWindowLongW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(pThis));

				pThis->handle = hwnd;
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
			
			const int d = 70;

			// capture portion of screen
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
				
				WICRect rc = {0, 0, 1280, 800};
				IWICBitmapLock* lock = nullptr;
				bitmap_iwic->Lock(&rc, WICBitmapLockRead, &lock);
				if(lock){
					UINT bufferSize, stride;
					BYTE* buffer = nullptr;
					lock->GetDataPointer(&bufferSize, &buffer);
					lock->GetStride(&stride);

					if(buffer[0]<100){ // it's likely to be showing oculus content
						bitmap_d2d->CopyFromMemory(nullptr, buffer, stride);
						if(changed){ // initiate fade-in if this is the first time
							changed = false;
							mode = ST_ENTERING;
						}
					}
					lock->Release();
				}

				bitmap_iwic->Release();
				iFactory->Release();
				DeleteObject(bitmap);
			}

			// render
			renderTarget->BeginDraw();

			// background
			renderTarget->DrawBitmap(bitmap_d2d);
			if(alpha<1){
				ID2D1SolidColorBrush* brush = nullptr;
				renderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0,1.0f-alpha), &brush);
				renderTarget->FillRectangle(D2D1::RectF(0,0,1280,800), brush);
				brush->Release();
			}

			// HUD
			if(hud_visible){
				// left eye
				renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(320+d, 400));
				RenderHUDCentered();

				// right eye
				renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(640+320-d, 400));
				RenderHUDCentered();

				// reset
				renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			}
			
			renderTarget->EndDraw();

			// animate global
			switch(mode){
			case ST_HOME:
				alpha = 0;
				break;
			case ST_APP:
				alpha = 1.0f;
				break;
			case ST_ENTERING:
				alpha += 0.01f;
				if(alpha>1.0f){
					mode = ST_APP;
					alpha = 1.0f;
				}
				break;
			case ST_LEAVING:
				alpha -= 0.01f;
				if(alpha<0){
					mode = ST_HOME;
					alpha = 0;
				}
				break;
			}

			// animate HUD
			bool curr_up = GetAsyncKeyState(VK_UP);
			if(curr_up && !prev_up)
				hud_cursor = max(0, hud_cursor-1);
			prev_up = curr_up;

			bool curr_down = GetAsyncKeyState(VK_DOWN);
			if(curr_down && !prev_down)
				hud_cursor = min(static_cast<Generic::List<String^>^>(hud_items)->Count-1, hud_cursor+1);
			prev_down = curr_down;

			bool curr_enter = GetAsyncKeyState(VK_RETURN);
			if(curr_enter && !prev_enter){
				if(hud_cursor_running != hud_cursor){
					hud_cursor_running = hud_cursor;
					app_change_request^ req = request_change;
					req->Invoke(hud_cursor);
				}
			}
			prev_enter = curr_enter;
		}

		// 
		void RenderHUDCentered(){
			Generic::List<String^>^ items = hud_items;

			// draw
			const float f_width = 250;
			const float margin = 3;
			const float height = 30;

			const float margin_text_h = 15;
			const float margin_text_v = 7;

			const int n = items->Count;
			const float f_height = (n-1) * margin + n*height;

			// items
			IDWriteFactory* wfactory;
			DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(wfactory), reinterpret_cast<IUnknown**>(&wfactory));

			IDWriteTextFormat* tformat;
			wfactory->CreateTextFormat(L"Meiryo UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_CONDENSED, 16, L"", &tformat);


			ID2D1SolidColorBrush* brush_base;
			ID2D1SolidColorBrush* brush_sel;
			ID2D1SolidColorBrush* brush_fg_base;
			ID2D1SolidColorBrush* brush_fg_sel;
			renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.97,0.97,0.97,0.67), &brush_base);
			renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.96,0.64,0.08,0.67), &brush_sel);
			renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.1,0.1,0.1), &brush_fg_base);
			renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.97,0.97,0.97), &brush_fg_sel);

			for(int i=0; i<n; i++){
				float x0 = -f_width/2;
				float y0 = -f_height/2 + i*(margin+height);

				pin_ptr<const wchar_t> wch = PtrToStringChars(items[i]);

				D2D1_RECT_F rc_box = D2D1::RectF(x0, y0, x0+f_width, y0+height);
				D2D1_RECT_F rc_text = D2D1::RectF(x0+margin_text_h, y0+margin_text_v, x0+f_width, y0+height-2*margin_text_v);

				if(i!=hud_cursor){
					renderTarget->FillRectangle(rc_box, brush_base);
					renderTarget->DrawText(wch, items[i]->Length, tformat, rc_text, brush_fg_base);
				}
				else{
					renderTarget->FillRectangle(rc_box, brush_sel);
					renderTarget->DrawText(wch, items[i]->Length, tformat, rc_text, brush_fg_sel);
				}

				if(i==hud_cursor_running){
					renderTarget->FillRectangle(D2D1::RectF(x0+f_width+margin, y0, x0+f_width+margin+10, y0+height), brush_sel);
				}
			}
			brush_base->Release();
			brush_sel->Release();
			brush_fg_base->Release();
			brush_fg_sel->Release();
			tformat->Release();
			wfactory->Release();
		}

		LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam){
			switch (message){
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_PAINT:
				RenderFrame();
				return 0;
			case WM_TIMER:
				InvalidateRect(handle, nullptr, FALSE);
				UpdateWindow(handle);
				return 0;
			case WM_HOTKEY:
				hud_visible = !hud_visible;
				return 0;
			}
			return DefWindowProc(handle, message, wParam, lParam);
		}
	
	private:
		HWND handle;
	private: // D2D things
		ID2D1Bitmap* bitmap_d2d;
		ID2D1Factory* direct2DFactory;
		ID2D1HwndRenderTarget* renderTarget;
	private: // global state (currently there's no muxing)
		HWND window_app;
		bool changed;
		EffectMode mode;
		float alpha; // 1:app 0:home
	private: // HUD state
		bool hud_visible;
		int hud_cursor;
		int hud_cursor_running;
		bool prev_up, prev_down, prev_enter;
		gcroot<Generic::List<String^>^> hud_items;
		gcroot<app_change_request^> request_change;
	};
}


