#include <iostream>
#include <metahost.h>
#include <mscoree.h>
#include <Environment.h>
#pragma comment(lib, "mscoree.lib")

using namespace std;

extern "C" {
	int main(int argc, char **argv) {
		Environment_load(argc, argv);
		HRESULT hr;
		try {
			// Get coreclr.dll location
			TCHAR exeFullPath[MAX_PATH];
			memset(exeFullPath, 0, MAX_PATH);
			GetModuleFileName(NULL, exeFullPath, MAX_PATH);
			wstring runtimePath = exeFullPath;
			size_t index = runtimePath.rfind(L'\\');
			wstring exeFileName = runtimePath.substr(index + 1);
			// Initialize clr runtime
			ICLRMetaHost *metaHost = nullptr;
			hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID *)&metaHost);
			if(FAILED(hr))
				throw "Failed to create MetaHost instance";
			ICLRRuntimeInfo *runtimeInfo = nullptr;
			hr = metaHost->GetRuntime(L"v4.0.30319", IID_PPV_ARGS(&runtimeInfo));
			if(FAILED(hr))
				throw "Failed to get runtime";
			ICLRRuntimeHost *runtimeHost = nullptr;
			hr = runtimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&runtimeHost));
			if(FAILED(hr))
				throw "Failed to start runtime";
			ICLRControl *clrControl = nullptr;
			hr = runtimeHost->GetCLRControl(&clrControl);
			if(FAILED(hr))
				throw "Failed to get CLR control";
			hr = runtimeHost->Start();
			if(FAILED(hr))
				throw "Failed to start runtime";
			DWORD returnValue = 0;
			hr = runtimeHost->ExecuteInDefaultAppDomain(L"Quasimodo.Bootstrap", L"Bootstrap.Global", L"ClrBootstrap", exeFileName.data(), &returnValue);
			if(FAILED(hr))
				throw "Failed to load assembly";
			return returnValue;
		} catch (const char *str) {
			cerr << str << endl;
			return 1;
		}
	}
}