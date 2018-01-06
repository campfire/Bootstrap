using System;
using System.Runtime.InteropServices;

namespace Bootstrap
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    delegate void Callback();

    [Flags]
    enum ErrorModes : uint
    {
        SYSTEM_DEFAULT = 0x0,
        SEM_FAILCRITICALERRORS = 0x0001,
        SEM_NOALIGNMENTFAULTEXCEPT = 0x0004,
        SEM_NOGPFAULTERRORBOX = 0x0002,
        SEM_NOOPENFILEERRORBOX = 0x8000
    }
}