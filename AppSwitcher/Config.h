#pragma once
#include "RiftWindow.h"
#include <Windows.h>
#include <vector>
#include <OVR.h>

namespace AppSwitcher {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Diagnostics;
	using namespace System::Threading;
	using namespace System::Timers;

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

	private: int currentAppIx;
	private: Process^ currentProcess;



	private: System::Windows::Forms::Button^  button2;


	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::Button^  button3;
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
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
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
			// button2
			// 
			this->button2->Location = System::Drawing::Point(14, 319);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 3;
			this->button2->Text = L"Launch";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Config::button2_Click);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(14, 33);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(439, 19);
			this->textBox1->TabIndex = 6;
			this->textBox1->Text = L"C:\\Users\\xyx\\Desktop\\Oculus Demos\\Yunalus\\Yunalus.exe";
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
			this->listBox1->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"C:\\Users\\xyx\\Desktop\\Oculus Demos\\Yunalus\\Yunalus.exe", 
				L"C:\\Users\\xyx\\Desktop\\Oculus Demos\\Mikulus\\Mikulus.exe"});
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
			// Config
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(721, 364);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->listBox1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->labelRiftInfo);
			this->Name = L"Config";
			this->Text = L"AppSwitcher";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:
		System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			LaunchUnityRiftApplication(textBox1->Text);
		}

		System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			auto dialog = gcnew OpenFileDialog();
			dialog->Filter = "Executable Files (*.exe)|*.exe";
			if(dialog->ShowDialog() == Windows::Forms::DialogResult::OK){
				textBox1->Text = dialog->FileName;
			}

		}

		System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			listBox1->Items->Add(textBox1->Text);
			textBox1->Text = "";
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

				LaunchUnityRiftApplication(listBox1->Items[currentAppIx]->ToString());
				currentAppIx = (currentAppIx+1)%listBox1->Items->Count;
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
	private:
};
}

		 
