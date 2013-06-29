#pragma once
#include <vector>

namespace AppSwitcher {

	using namespace System;
	using namespace System::IO;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Diagnostics;
	using namespace System::Timers;
	using namespace System::Web::Script::Serialization;
	
	/// <summary>
	/// AppConfig holds configuration for launching & configurating one VR application. Currently, only unity is supported.
	/// </summary>
	public ref class AppConfig {
	public:
		AppConfig(){
		}
		AppConfig(String^ path) : path(path) {
		}
	public:
		String^ path;
	};
}

		 
