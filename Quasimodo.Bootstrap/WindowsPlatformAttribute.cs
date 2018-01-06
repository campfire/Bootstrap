using System;

[AttributeUsage(AttributeTargets.Assembly, AllowMultiple = false, Inherited = false)]
public sealed class WindowsPlatformAttribute : Attribute
{
    public bool ErrorReportHandle { get; private set; }

    public WindowsPlatformAttribute(bool errorReportHandle = false)
    {
        ErrorReportHandle = errorReportHandle;
    }
}