#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
#include <libgen.h>
#include <Environment.h>

using namespace std;

typedef int (*Initialize)(const char* exePath, const char* appDomainFriendlyName, int propertyCount, const char** propertyKeys, const char** propertyValues, void** hostHandle, unsigned int* domainId);
typedef int (*CreateDelegate)(void *hostHandle, unsigned int domainId, const char *entryPointAssemblyName, const char *entryPointTypeName, const char *entryPointMethodName, void **delegate);
typedef int (*Shutdown)(void* hostHandle, unsigned int domainId);
typedef void (*MainMethod)();

extern "C" {
	int main(int argc, char **argv) {
		Environment_load(argc, argv);
		try {
			// Load library
			void *coreclr = dlopen("/Pharos/libcoreclr.so", RTLD_NOW | RTLD_GLOBAL);
			// TODO failed
			Initialize initialize = (Initialize)dlsym(coreclr, "coreclr_initialize");
			if(!initialize)
				throw "Unable to load 'libcoreclr.so'";
			CreateDelegate createDelegate = (CreateDelegate)dlsym(coreclr, "coreclr_create_delegate");
			Shutdown shutdown = (Shutdown)dlsym(coreclr, "coreclr_shutdown");
			const char *propertyKeys[] = {
				"APPBASE",
				"TRUSTED_PLATFORM_ASSEMBLIES",
				"APP_PATHS"
			};
			const char *propertyValues[] = {
				"/Pharos",
				"/Pharos/mscorlib.dll:/Pharos/System.Runtime.dll:/Pharos/System.Console.dll:/Pharos/System.IO.dll:/Pharos/System.Text.Encoding.dll:/Pharos/System.Threading.Tasks.dll",
				"/Pharos"
			};
			int propertyCount = sizeof(propertyKeys) / sizeof(const char *);
			void* hostHandle;
			unsigned int domainId;
			int result = initialize("/Pharos/Run", "Pharos.Run", propertyCount, propertyKeys, propertyValues, &hostHandle, &domainId);
			if (result < 0)
				throw "Failed to create application domain";
			void *mainMethod;
			result = createDelegate(hostHandle, domainId, "Quasimodo.Bootstrap", "Bootstrap.Global", "CoreBootstrap", &mainMethod);
			if (result < 0)
				throw "Entry point not found";
			((MainMethod)mainMethod)();
			shutdown(hostHandle, domainId);
		} catch (const char *ex) {
			cerr << str << endl;
			return 1;
		}
		return 0;
	}
}