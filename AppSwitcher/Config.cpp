#include "Config.h"

bool CALLBACK AppSwitcher::list_proc(HWND hwnd, LPARAM param){
	std::vector<HWND>* plist = reinterpret_cast<std::vector<HWND>*>(param);
	plist->push_back(hwnd);
	return true;
}
