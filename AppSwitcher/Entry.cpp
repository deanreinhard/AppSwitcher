#include <Windows.h>
#include "Config.h"
using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;

/// Get timestamp from the binary itself.
/// http://stackoverflow.com/questions/1600962/displaying-the-build-date
DateTime RetrieveLinkerTimestamp(){
	String^ filePath = System::Reflection::Assembly::GetCallingAssembly()->Location;
    const int c_PeHeaderOffset = 60;
    const int c_LinkerTimestampOffset = 8;
	array<uint8_t>^ b = gcnew array<uint8_t>(2048);
	Stream^ s = nullptr;

    try{
		s = gcnew FileStream(filePath, System::IO::FileMode::Open, System::IO::FileAccess::Read);
		s->Read(b, 0, b->Length);
    }
    finally{
        if(s)
			s->Close();
    }

    int i = System::BitConverter::ToInt32(b, c_PeHeaderOffset);
    int secondsSince1970 = System::BitConverter::ToInt32(b, i + c_LinkerTimestampOffset);
    DateTime dt = DateTime(1970, 1, 1, 0, 0, 0);
    dt = dt.AddSeconds(secondsSince1970);
	dt = dt.AddHours(TimeZone::CurrentTimeZone->GetUtcOffset(dt).Hours);
    return dt;
}

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
		file->WriteLine("### Build Date");
		file->WriteLine(RetrieveLinkerTimestamp().ToString());
		file->WriteLine("### Crash Date");
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