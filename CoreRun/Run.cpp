#include <iostream>
#include <Windows.h>
#include <QtGlobal>
#include <Environment.h>

using namespace std;
typedef void (STDMETHODCALLTYPE *PManagedEntryPoint)();

extern "C" {
	int main(int argc, char **argv) {
		Environment_load(argc, argv);
		try {
			TCHAR exeFullPath[MAX_PATH];
			memset(exeFullPath, 0, MAX_PATH);
			GetModuleFileName(NULL, exeFullPath, MAX_PATH);
			wstring runtimePath = exeFullPath;
			size_t index = runtimePath.rfind(L'\\');
			wstring exeFileName = runtimePath.substr(index + 1);
			SetEnvironmentVariable(L"QUASIMODO_BOOTSTRAP_ENTRYPOINT", exeFileName.data());
			runtimePath =  runtimePath.substr(0, index + 1);
			wstring coreClrPath = runtimePath + L"coreclr.dll";
			ICLRRuntimeHost2 *clrRuntimeHost = nullptr;
			// Load library
			HMODULE clrModule = ::LoadLibraryExW(coreClrPath.data(), NULL, 0);
			if(clrModule == NULL)
				throw "Unable to load 'coreclr.dll'";
			FnGetCLRRuntimeHost pfnGetCLRRuntimeHost = (FnGetCLRRuntimeHost)::GetProcAddress(clrModule, "GetCLRRuntimeHost");
			pfnGetCLRRuntimeHost(IID_ICLRRuntimeHost2, (IUnknown **)&clrRuntimeHost);
			clrRuntimeHost->SetStartupFlags((STARTUP_FLAGS)(STARTUP_FLAGS::STARTUP_LOADER_OPTIMIZATION_SINGLE_DOMAIN | STARTUP_FLAGS::STARTUP_SINGLE_APPDOMAIN | STARTUP_FLAGS::STARTUP_SERVER_GC | STARTUP_FLAGS::STARTUP_CONCURRENT_GC));
			clrRuntimeHost->Authenticate(CORECLR_HOST_AUTHENTICATION_KEY);
			clrRuntimeHost->Start();
			wstring trustedPlatformAssemblies = runtimePath + L"System.Private.CoreLib.dll;";
			wstring trustedPath = runtimePath + L"Assemblies\\Trusted\\*.dll";
			WIN32_FIND_DATA data;
			HANDLE findHandle = FindFirstFile(trustedPath.data(), &data);
			if (findHandle != INVALID_HANDLE_VALUE) {
				do {
					trustedPlatformAssemblies.append(runtimePath);
					trustedPlatformAssemblies.append(L"Assemblies\\Trusted\\");
					trustedPlatformAssemblies.append(data.cFileName);
					trustedPlatformAssemblies.append(L";");
				} while (FindNextFile(findHandle, &data) != 0);
				FindClose(findHandle);
			}
			const wchar_t *propertyKeys[] = {
				L"APPBASE",
				L"TRUSTED_PLATFORM_ASSEMBLIES",
				L"APP_PATHS",
			};
			wstring appPaths = runtimePath + L"Assemblies\\Trusted;" + runtimePath + L"Assemblies;" + L";" + runtimePath;
			const wchar_t *propertyValues[] = {
				runtimePath.data(),
				trustedPlatformAssemblies.data(),
				appPaths.data(),
			};
			int propertyCount = sizeof(propertyKeys) / sizeof(wchar_t *);
			DWORD appDomainFlags = APPDOMAIN_ENABLE_PLATFORM_SPECIFIC_APPS | APPDOMAIN_ENABLE_PINVOKE_AND_CLASSIC_COMINTEROP | APPDOMAIN_DISABLE_TRANSPARENCY_ENFORCEMENT;
			DWORD domainId;
			HRESULT hr = clrRuntimeHost->CreateAppDomainWithManager(L"Quasimodo.Bootstrap", appDomainFlags, NULL, NULL, propertyCount, propertyKeys, propertyValues, &domainId);
			if (FAILED(hr))
				throw "Failed to create application domain";
			PManagedEntryPoint pManagedEntryPoint;
			hr = clrRuntimeHost->CreateDelegate(domainId, L"Quasimodo.Bootstrap", L"Bootstrap.Global", L"CoreBootstrap", (INT_PTR*)&pManagedEntryPoint);
			if (FAILED(hr))
				throw "Entry point not found";
			pManagedEntryPoint();
			clrRuntimeHost->UnloadAppDomain(domainId, true);
			clrRuntimeHost->Stop();
			clrRuntimeHost->Release();
		} catch (const char *str) {
			cerr << str << endl;
			return 1;
		}
	}
}