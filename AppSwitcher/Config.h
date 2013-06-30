#pragma once
#include "RiftWindow.h"
#include "AppConfig.h"
#include <Windows.h>
#include <vector>
#include <OVR.h>

namespace AppSwitcher {

	using namespace System;
	using namespace System::IO;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Diagnostics;
	using namespace System::Threading;
	using namespace System::Web::Script::Serialization;
	

	bool CALLBACK list_proc(HWND hwnd, LPARAM param);

	/// <summary>
	/// Config is the configuration window for AppSwitcher. Config also handles actual application execution.
	/// </summary>
	public ref class Config : public System::Windows::Forms::Form
	{
	public:
		Config() {
			InitializeComponent();
			
			// get rift info
			OVR::System::Init();

			device_manager = OVR::DeviceManager::Create();
			OVR::HMDDevice *hmd = device_manager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();

			OVR::HMDInfo info;
			hmd->GetDeviceInfo(&info);
			
			String^ displayDeviceName = gcnew String(info.DisplayDeviceName);
			String^ device = displayDeviceName->Substring(0, displayDeviceName->LastIndexOf("\\"));
			labelRiftInfo->Text = String::Format("({0},{1}) {2} {3}", info.DesktopX, info.DesktopY, displayDeviceName, device);
			// example of displaydevicename "\\.\DISPLAY2\Monitor0"

			// create rift window
			desktop_zoom = new float;
			desktop_ipd = new float;
			*desktop_zoom = 1;
			*desktop_ipd = 70;
			rw = new RiftWindow(device, gcnew app_change_request(this, &Config::SwitchApp), gcnew app_running_request(this, &Config::CheckApp),
				desktop_zoom, desktop_ipd);

			// load model & apply to view
			config = LoadConfig("config.json");
			ApplyConfigToView(false);
		}

	protected:
		// Load config from specified path. Return default one if failed to open / invalid config etc. occurs.
		Generic::List<AppConfig^>^ LoadConfig(String^ path){
			auto ls = gcnew Generic::List<AppConfig^>();

			try{
				
				auto file = File::OpenText(path);
				String^ json = file->ReadToEnd();
				file->Close();

				auto ser = gcnew JavaScriptSerializer();
				auto ls_path = ser->Deserialize<Generic::List<AppConfig^>^>(json);
				if(ls_path == nullptr){
					return ls;
				}

				return ls_path;
			}
			catch(FileNotFoundException^ e){
			}

			return ls;
		}

		void SaveConfig(String^ path, Generic::List<AppConfig^>^ config){
			auto ser = gcnew JavaScriptSerializer();
			auto json = ser->Serialize(config);
			auto file = File::CreateText(path);
			file->Write(json);
			file->Flush();
			file->Close();
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~Config()
		{
			if (components)
			{
				delete components;
			}
		}

	
	private: float* desktop_ipd;
	private: float* desktop_zoom;

	private: OVR::DeviceManager* device_manager;
	private: RiftWindow* rw;
	private: System::Windows::Forms::Label^  labelRiftInfo;

	private: Generic::List<AppConfig^>^ config;
	private: Process^ currentProcess;



	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  buttonRemove;
	private: System::Windows::Forms::TrackBar^  trackBarIPD;
	private: System::Windows::Forms::TrackBar^  trackBarZoom;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  labelIPD;
	private: System::Windows::Forms::Label^  labelZoom;

	protected: 

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->labelRiftInfo = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->buttonRemove = (gcnew System::Windows::Forms::Button());
			this->trackBarIPD = (gcnew System::Windows::Forms::TrackBar());
			this->trackBarZoom = (gcnew System::Windows::Forms::TrackBar());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->labelIPD = (gcnew System::Windows::Forms::Label());
			this->labelZoom = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarIPD))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarZoom))->BeginInit();
			this->SuspendLayout();
			// 
			// labelRiftInfo
			// 
			this->labelRiftInfo->AutoSize = true;
			this->labelRiftInfo->Location = System::Drawing::Point(12, 277);
			this->labelRiftInfo->Name = L"labelRiftInfo";
			this->labelRiftInfo->Size = System::Drawing::Size(35, 12);
			this->labelRiftInfo->TabIndex = 2;
			this->labelRiftInfo->Text = L"label1";
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(14, 33);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(439, 19);
			this->textBox1->TabIndex = 6;
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &Config::textBox1_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 18);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(47, 12);
			this->label2->TabIndex = 7;
			this->label2->Text = L"FilePath";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(12, 265);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(89, 12);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Rift Display Info";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(459, 31);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 9;
			this->button1->Text = L"Browse";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Config::button1_Click);
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->ItemHeight = 12;
			this->listBox1->Location = System::Drawing::Point(14, 83);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(520, 172);
			this->listBox1->TabIndex = 10;
			this->listBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Config::listBox1_SelectedIndexChanged);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(14, 54);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 11;
			this->button3->Text = L"Add";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Config::button3_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label1->Location = System::Drawing::Point(12, 289);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(234, 12);
			this->label1->TabIndex = 12;
			this->label1->Text = L"Press Ctrl+Shift+Z to show/hide HUD";
			// 
			// buttonRemove
			// 
			this->buttonRemove->Enabled = false;
			this->buttonRemove->Location = System::Drawing::Point(95, 54);
			this->buttonRemove->Name = L"buttonRemove";
			this->buttonRemove->Size = System::Drawing::Size(75, 23);
			this->buttonRemove->TabIndex = 13;
			this->buttonRemove->Text = L"Remove";
			this->buttonRemove->UseVisualStyleBackColor = true;
			this->buttonRemove->Click += gcnew System::EventHandler(this, &Config::buttonRemove_Click);
			// 
			// trackBarIPD
			// 
			this->trackBarIPD->LargeChange = 10;
			this->trackBarIPD->Location = System::Drawing::Point(43, 314);
			this->trackBarIPD->Maximum = 130;
			this->trackBarIPD->Minimum = 30;
			this->trackBarIPD->Name = L"trackBarIPD";
			this->trackBarIPD->Size = System::Drawing::Size(287, 45);
			this->trackBarIPD->SmallChange = 5;
			this->trackBarIPD->TabIndex = 14;
			this->trackBarIPD->TickFrequency = 5;
			this->trackBarIPD->Value = 70;
			this->trackBarIPD->ValueChanged += gcnew System::EventHandler(this, &Config::trackBarIPD_ValueChanged);
			// 
			// trackBarZoom
			// 
			this->trackBarZoom->LargeChange = 200;
			this->trackBarZoom->Location = System::Drawing::Point(43, 356);
			this->trackBarZoom->Maximum = 1200;
			this->trackBarZoom->Minimum = 300;
			this->trackBarZoom->Name = L"trackBarZoom";
			this->trackBarZoom->Size = System::Drawing::Size(287, 45);
			this->trackBarZoom->SmallChange = 100;
			this->trackBarZoom->TabIndex = 15;
			this->trackBarZoom->TickFrequency = 100;
			this->trackBarZoom->Value = 1000;
			this->trackBarZoom->ValueChanged += gcnew System::EventHandler(this, &Config::trackBarZoom_ValueChanged);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(12, 315);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(23, 12);
			this->label4->TabIndex = 16;
			this->label4->Text = L"IPD";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(12, 356);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(33, 12);
			this->label5->TabIndex = 17;
			this->label5->Text = L"Zoom";
			// 
			// labelIPD
			// 
			this->labelIPD->AutoSize = true;
			this->labelIPD->Location = System::Drawing::Point(327, 315);
			this->labelIPD->Name = L"labelIPD";
			this->labelIPD->Size = System::Drawing::Size(33, 12);
			this->labelIPD->TabIndex = 18;
			this->labelIPD->Text = L"70 px";
			// 
			// labelZoom
			// 
			this->labelZoom->AutoSize = true;
			this->labelZoom->Location = System::Drawing::Point(327, 356);
			this->labelZoom->Name = L"labelZoom";
			this->labelZoom->Size = System::Drawing::Size(31, 12);
			this->labelZoom->TabIndex = 19;
			this->labelZoom->Text = L"x1.00";
			// 
			// Config
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(548, 394);
			this->Controls->Add(this->labelZoom);
			this->Controls->Add(this->labelIPD);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->trackBarZoom);
			this->Controls->Add(this->trackBarIPD);
			this->Controls->Add(this->buttonRemove);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->listBox1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->labelRiftInfo);
			this->Name = L"Config";
			this->Text = L"AppSwitcher";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Config::Config_FormClosed);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarIPD))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarZoom))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:
		/// Update listBox1 to show current config.
		/// <param name="preserveIndex">keep current selected index if true. Otherwise it'll be unselected</param>
		void ApplyConfigToView(bool preserveIndex){
			// View in Config window
			int ix = listBox1->SelectedIndex;
			listBox1->Items->Clear();
			
			for each(AppConfig^ cfg in config){
				listBox1->Items->Add(cfg->path);
			}

			if(ix>=0 && preserveIndex){
				listBox1->SelectedIndex = ix;
			}
			else{
				listBox1->SelectedIndex = -1;
			}

			// View in HUD window
			rw->NotifyModelChange(config);
		}

		System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			auto dialog = gcnew OpenFileDialog();
			dialog->Filter = "Executable Files (*.exe)|*.exe";
			if(dialog->ShowDialog() == Windows::Forms::DialogResult::OK){
				textBox1->Text = dialog->FileName;
			}
		}

		System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			config->Add(gcnew AppConfig(textBox1->Text));
			textBox1->Text = "";
			ApplyConfigToView(true);
		}

		System::Void Config_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			try{
				SaveConfig("config.json", config);
			}
			catch(Exception^ e){
			} // squash error
		 }

		// ix=-1: change to desktop ix>=0:change to specified app id
		void SwitchApp(int ix){
			if(currentProcess){
				rw->NotifyAppTerminate();
				currentProcess->Kill();
				currentProcess = nullptr;
			}

			if(ix>=0)
				LaunchUnityRiftApplication(config[ix]->path);
		}

		// return true if app is really running
		bool CheckApp(){
			if(!currentProcess)
				return false;
			return !(currentProcess->WaitForExit(0));
		}
		
		protected:
		void LaunchUnityRiftApplication(String^ filepath){
			const int max_wait_ms = 2500; // maximum duration for trying
			const int step_wait = 100; // time until retry

			Process^ proc = gcnew Process();
			proc->StartInfo->FileName = filepath;
			proc->StartInfo->UseShellExecute = false; // this will also inhibit security warning
			proc->Start();

			// Try to configure window associated with proc. Give up after 2.5 sec.
			// This approach is retry(2.5s)(find window + configure). Avoid retry(2.5s)(find window) + configure because it's unstable.
			HWND target = nullptr;
			
			ULONGLONG t0 = GetTickCount64();
			while(GetTickCount64() < t0 + max_wait_ms){
				target = GetTopLevelWindowForProcessId(proc->Id);
				if(target){
					try{
						ConfigureAndRunUnityRiftApplication(target);
						break; // successfully launched
					}
					catch(Exception^ e){
						target = nullptr; // failed; reset target to show failure
					}
				}
				Thread::Sleep(step_wait);
			}
			if(!target){
				int pid = proc->Id;
				proc->Kill();
				throw gcnew Exception(String::Format("Couldn't configure window for pid={0}.", pid));
			}


			// Try to find new window associated with proc. Give up after 2.5 sec. (Unity main window has different HWND from launcher)
			HWND window = nullptr;

			t0 = GetTickCount64();
			while(GetTickCount64() < t0 + max_wait_ms){
				window = GetTopLevelWindowForProcessId(proc->Id);
				if(window && window != target)
					break;
				Thread::Sleep(step_wait);
			}
			if(!window || (window == target)){
				int pid = proc->Id;
				proc->Kill();
				throw gcnew Exception(String::Format("Couldn't find launched window for pid={0}.", pid));
			}

			// we launch it anyway if window isn't big (since we don't set window parameters, it's possible)
			currentProcess = proc;
			rw->NotifyAppChange(window);
		}

		HWND GetTopLevelWindowForProcessId(int pid){
			std::vector<HWND> top_windows;
			EnumWindows(reinterpret_cast<WNDENUMPROC>(list_proc), reinterpret_cast<LPARAM>(&top_windows));

			HWND target = nullptr;
			for(auto top_window : top_windows){
				DWORD pid_current;
				GetWindowThreadProcessId(top_window, &pid_current);
				if(pid_current == pid){
					target = top_window;
					break;
				}
			}
			return target;
		}

		// Send messages to configure given launcher window.
		// throws Exception when failed
		void ConfigureAndRunUnityRiftApplication(HWND target){
			// get all child windows
			ArrayList^ children = GetAllChildren(target);

			// checkbox
			bool config_checkbox = false;
			for each(Object^ hwnd_pre in children){
				HWND hwnd = reinterpret_cast<HWND>(Convert::ToInt32(hwnd_pre));

				// get title string
				const int length = 256;
				wchar_t str[length];
				SendMessageW(hwnd, WM_GETTEXT, length, reinterpret_cast<LPARAM>(str));
				String^ title = gcnew String(str);

				GetClassNameW(hwnd, str, length);
				String^ cls = gcnew String(str);

				if(title=="Windowed" && cls=="Button"){
					SendMessage(hwnd, BM_SETCHECK, BST_CHECKED, 0);
					config_checkbox = true;
				}
			}

			if(!config_checkbox)
				throw gcnew Exception("Failed to set windowed flag.");

			// resolution
			bool config_resolution = false;
			for each(Object^ hwnd_pre in children){
				HWND hwnd = reinterpret_cast<HWND>(Convert::ToInt32(hwnd_pre));

				// get title string
				const int length = 256;
				wchar_t str[length];
				GetClassNameW(hwnd, str, length);
				String^ cls = gcnew String(str);

				if(cls=="ComboBox"){
					Generic::List<String^>^ entries = EnumerateComboBox(hwnd);
					for(int i=0; i<entries->Count; i++){
						if(entries[i] == "1280 x 800"){
							if(SendMessage(hwnd, CB_SETCURSEL, i, 0) == i){ // correctly set
								config_resolution = true;
							}
						}
					}
				}
			}

			if(!config_resolution)
				throw gcnew NotSupportedException("Failed to set resolution to 1280x800.");

			// run it
			bool launched = false;
			for each(Object^ hwnd_pre in children){
				HWND hwnd = reinterpret_cast<HWND>(Convert::ToInt32(hwnd_pre));

				// get title string
				const int length = 256;
				wchar_t str[length];
				SendMessageW(hwnd, WM_GETTEXT, length, reinterpret_cast<LPARAM>(str));
				String^ title = gcnew String(str);

				GetClassNameW(hwnd, str, length);
				String^ cls = gcnew String(str);

				if(title=="Play!" && cls=="Button"){
					SendMessage(hwnd, BM_CLICK, 0, 0);
					launched = true;
				}
			}

			if(!launched)
				throw gcnew Exception(String::Format("Failed to find \"Play!\" button."));
		}

		Generic::List<String^>^ EnumerateComboBox(HWND hwnd){
			auto result = gcnew Generic::List<String^>();

			int num = SendMessageW(hwnd, CB_GETCOUNT, 0, 0);
			if(num == CB_ERR)
				throw gcnew Exception("cannot enumerate combobox items");

			for(int i=0; i<num; i++){
				const int length = 256;
				wchar_t str[length];
				if(SendMessageW(hwnd, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(str)) == CB_ERR) // TODO: possibility of overflow
					throw gcnew Exception("cannot enumerate combobox items");

				result->Add(gcnew String(str));
			}

			return result;
		}


		/// <summary>
		/// Run depth first search to get all children of given HWND. (including deep children)
		/// </summary>
		ArrayList^ GetAllChildren(HWND parent){
			return GetAllChildren(parent, gcnew ArrayList());
		}

		ArrayList^ GetAllChildren(HWND parent, ArrayList^ children){
			HWND child = 0;
			while(true){
				child = FindWindowExW(parent, child, nullptr, nullptr);
				if(child){
					children->Add(reinterpret_cast<int>(child));
					GetAllChildren(child, children);
				}
				else{
					break;
				}
			}
			return children;
		}
		
private: System::Void listBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 int ix = listBox1->SelectedIndex;
			 if(ix>=0){
				textBox1->Text = config[ix]->path;
				buttonRemove->Enabled = true;
			 }
			 else{
				 buttonRemove->Enabled = false;
			 }
		 }
private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			 int ix = listBox1->SelectedIndex;
			 if(ix>=0){
				config[ix]->path = textBox1->Text;
				ApplyConfigToView(true);
			}
		 }
private: System::Void buttonRemove_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(listBox1->SelectedIndex>=0){
				 config->RemoveAt(listBox1->SelectedIndex);
				 ApplyConfigToView(false);
			 }
		 }

private: System::Void trackBarIPD_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 *desktop_ipd = trackBarIPD->Value;
			labelIPD->Text = String::Format("{0} px", *desktop_ipd);
		 }
private: System::Void trackBarZoom_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 *desktop_zoom = trackBarZoom->Value*1e-3f;
			 labelZoom->Text = String::Format("x{0:F2}", *desktop_zoom);
		 }
};
}

		 
