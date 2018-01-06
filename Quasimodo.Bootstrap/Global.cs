using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Bootstrap
{
    static class Global
    {
        static Callback callback;
        static object instance;

        [DllImport("kernel32.dll")]
        static extern ErrorModes SetErrorMode(ErrorModes mode);

        [DllImport("Environment")]
        static extern void Environment_initialize(BootstrapType type, Callback callback);

        static int ClrBootstrap(string arguments)
        {
            Main(arguments);
            return 0;
        }

        static void CoreBootstrap()
        {
            const string ENTRYPOINT_ENVIRONMENT_NAME = "QUASIMODO_BOOTSTRAP_ENTRYPOINT";
            var fileName = Environment.GetEnvironmentVariable(ENTRYPOINT_ENVIRONMENT_NAME);
            Environment.SetEnvironmentVariable(ENTRYPOINT_ENVIRONMENT_NAME, null);
            Main(fileName);
        }

        static void Main(string fileName)
        {
            if (fileName != null)
            {
                if (fileName.ToLower().EndsWith(".exe"))
                    fileName = fileName.Substring(0, fileName.Length - 4);

                var assembly = Assembly.Load(new AssemblyName(fileName));
                // Entry Point
                var mainAttribute = assembly.GetCustomAttribute<MainAttribute>();
                if (mainAttribute == null)
                    throw new NotSupportedException($"Cannot found the [MainAttribute] in assembly '{fileName}'");

                if (mainAttribute.EntryClass == null)
                    throw new NotSupportedException($"MainAttribute.EntryClass is null");

                //Platform Specifics
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    var windowsPlatform = assembly.GetCustomAttribute<WindowsPlatformAttribute>();
                    if (windowsPlatform?.ErrorReportHandle == true)
                        SetErrorMode(SetErrorMode(0) | ErrorModes.SEM_NOGPFAULTERRORBOX | ErrorModes.SEM_FAILCRITICALERRORS | ErrorModes.SEM_NOOPENFILEERRORBOX);
                }

                var constructors = mainAttribute.EntryClass.GetConstructors(BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance);
                foreach (var ctor in constructors)
                {
                    if (ctor.GetParameters().Length == 0)
                    {
                        void execute() => instance = ctor.Invoke(new object[0]);
                        if (mainAttribute.Type == 0)
                            execute();
                        else
                        {
                            callback = execute;
                            Environment_initialize(mainAttribute.Type, callback);
                        }
                        return;
                    }
                }
                throw new NotSupportedException($"Cannot found a non-parameter constructor in type '{mainAttribute.EntryClass.FullName}'");
            }
        }

        static void Dispose()
        {
            if (instance is IDisposable disposable)
                disposable?.Dispose();
        }
    }
}