using System;

public enum BootstrapType : byte
{
    Console = 1, Gui = 2, Widgets = 3
}

[AttributeUsage(AttributeTargets.Assembly, AllowMultiple = false, Inherited = false)]
public sealed class MainAttribute : Attribute
{
    public Type EntryClass { get; private set; }
    public BootstrapType Type { get; private set; }

    public MainAttribute(Type entryClass, BootstrapType type = 0)
    {
        EntryClass = entryClass;
        Type = type;
    }
}