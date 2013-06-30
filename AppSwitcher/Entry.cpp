#include <Windows.h>
#include "Config.h"
using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	try{
		Application::Run(gcnew AppSwitcher::Config());
	}
	catch(Exception^ e){
		auto file = File::CreateText("AppSwitcher-error-report.txt");

		file->WriteLine("# App Switcher Error Report");
		
		file->WriteLine("## Environment");
		file->WriteLine("### Date");
		file->WriteLine(DateTime::Now.ToString());

		file->WriteLine("## Exception");
		file->WriteLine("### Message");
		file->WriteLine(e->Message);
		file->WriteLine("### Source");
		file->WriteLine(e->Source);
		file->WriteLine("### StackTrace");
		file->WriteLine(e->StackTrace);
		file->WriteLine("### TargetSite");
		file->WriteLine(e->TargetSite);

		file->Flush();
		file->Close();
	}
	return 0;
}