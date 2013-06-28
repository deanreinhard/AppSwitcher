#include <Windows.h>
#include "Config.h"
using namespace System;
using namespace System::Windows::Forms;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Application::Run(gcnew AppSwitcher::Config());
	return 0;
}