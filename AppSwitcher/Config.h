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
	using namespace System::Timers;
	using namespace System::Web::Script::Serialization;
	

	bool CALLBACK list_proc(HWND hwnd, LPARAM param);

	/// <summary>
	/// Config is the configuration window for AppSwitcher. Config also handles actual application execution.
	/// </summary>
	public ref class Config : public System::Windows::Forms::Form
	{
	public:
		Config()
		{
			InitializeComponent();

			// load model & apply to view
			config = LoadConfig("config.json");
			auto ie = config->GetEnumerator();
			while(ie.MoveNext()){
				listBox1->Items->Add(ie.Current->path);
			}
			
			// get rift info
			OVR::System::Init();

			device_manager = OVR::DeviceManager::Create();
			OVR::HMDDevice *hmd = device_manager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();

			OVR::HMDInfo info;
			hmd->GetDeviceInfo(&info);
			
			String ^displayDeviceName = gcnew String(info.DisplayDeviceName);
			String ^device = displayDeviceName->Substring(0, displayDeviceName->LastIndexOf("\\"));
			labelRiftInfo->Text = String::Format("({0},{1}) {2} {3}", info.DesktopX, info.DesktopY, displayDeviceName, device);

			// example of displaydevicename "\\.\DISPLAY2\Monitor0"
			


			// setup rift video stream
			rw = new RiftWindow(device);

			t = gcnew System::Timers::Timer(30); // 30ms -> 33fps
			t->Elapsed += gcnew ElapsedEventHandler(this, &Config::CopyFrame);
			t->Enabled = true;

			// register hotkey
			RegisterHotKey(static_cast<HWND>(Handle.ToPointer()), 0, MOD_CONTROL | MOD_SHIFT, 'Z');
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

	
	private: OVR::DeviceManager *device_manager;
	private: RiftWindow *rw;
	//private: HWND window_app;
	private: System::Timers::Timer^ t;
	private: System::Windows::Forms::Label^  labelRiftInfo;

	private: Generic::List<AppConfig^>^ config;
	private: int currentAppIx;
	private: Process^ currentProcess;






	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::Button^  button3;
private: System::Windows::Forms::Label^  label1;
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
			this->label1->Location = System::Drawing::Point(14, 340);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(173, 12);
			this->label1->TabIndex = 12;
			this->label1->Text = L"Press Ctrl+Shift+Z to switch app";
			// 
			// Config
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(721, 364);
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
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:
		System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			auto dialog = gcnew OpenFileDialog();
			dialog->Filter = "Executable Files (*.exe)|*.exe";
			if(dialog->ShowDialog() == Windows::Forms::DialogResult::OK){
				textBox1->Text = dialog->FileName;
			}

		}

		System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			config->Add(gcnew AppConfig(textBox1->Text));
			listBox1->Items->Add(textBox1->Text);
			textBox1->Text = "";
		}

		System::Void Config_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			try{
				SaveConfig("config.json", config);
			}
			catch(Exception^ e){
			} // squash error
		 }

		
		protected:
		virtual System::Void WndProc(Message% msg) override {
			Form::WndProc(msg);

			if(msg.Msg == WM_HOTKEY){
				if(currentProcess){
					rw->window_app = 0;
					currentProcess->Kill();
					currentProcess = nullptr;
				}

				LaunchUnityRiftApplication(config[currentAppIx]->path);
				currentAppIx = (currentAppIx+1)%config->Count;
			}
		}

		void LaunchUnityRiftApplication(String^ filepath){
			Process^ proc = gcnew Process();
			proc->StartInfo->FileName = filepath;
			proc->StartInfo->UseShellExecute = false; // this will also inhibit security warning
			proc->Start();

			Thread::Sleep(1500); // wait 1500ms for window to show up (TODO: come up with more robust method)
			
			HWND target = GetTopLevelWindowForProcessId(proc->Id);
			if(!target){
				throw gcnew Exception(String::Format("Couldn't find window for pid={0}.", proc->Id));
			}
		
			ConfigureAndRunUnityRiftApplication(target);

			Thread::Sleep(1500); // wait 1500ms for Direct3D window to show up (it has different HWND!)

			currentProcess = proc;
			rw->window_app = GetTopLevelWindowForProcessId(proc->Id);
		}

		HWND GetTopLevelWindowForProcessId(int pid){
			std::vector<HWND> top_windows;
			EnumWindows(reinterpret_cast<WNDENUMPROC>(list_proc), reinterpret_cast<LPARAM>(&top_windows));

			HWND target = 0;
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

			// now try to configure
			bool config_checkbox = false;
			IEnumerator^ it = children->GetEnumerator();
			while(it->MoveNext()){
				HWND hwnd = reinterpret_cast<HWND>(Convert::ToInt32(it->Current));

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
				throw gcnew Exception(String::Format("Failed to check windowed checkbox."));

			// run it
			bool launched = false;
			it = children->GetEnumerator();
			while(it->MoveNext()){
				HWND hwnd = reinterpret_cast<HWND>(Convert::ToInt32(it->Current));

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


		/// <summary>
		/// Run depth first search to get all children of given HWND. (including deep children)
		/// </summary>
		ArrayList^ GetAllChildren(HWND parent){
			return GetAllChildren(parent, gcnew ArrayList());
		}

		ArrayList^ GetAllChildren(HWND parent, ArrayList^ children){
			HWND child = 0;
			while(true){
				child = FindWindowExW(parent, child, NULL, NULL);
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
		
		void CopyFrame(Object^ source, ElapsedEventArgs ^e){
			InvalidateRect(rw->m_hwnd, NULL, FALSE);
			UpdateWindow(rw->m_hwnd);
		}
};
}

		 
