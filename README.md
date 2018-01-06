# Bootstrap - a CoreCLR self-host bootstrapper to run netstandard library directly

## 这个解决什么问题

通常来说，要运行一个 .Net Core 程序，需要一个启动器来运行，例如 CoreCLR 自带的 corerun.exe、coreconsole.exe，或 .Net Core SDK 自带的 dotnet.exe，他们都分别有各自的问题：

### corerun.exe、dotnet.exe

1. 只能运行带 Main 入口的 exe 程序集；
2. 进程名称就是 corerun.exe、dotnet.exe，当运行了多个程序而某个程序资源占用太高时，无法清晰地判断是哪个程序出问题，使用 Visual Studio 附加到进程时，也无法快速定位到哪个进程；

### corerun.exe、coreconsole.exe、dotnet.exe

1. 运行程序会弹出命令提示符窗口，这样对一个 Gui 程序来说很不友好；
2. BCL 库跟应用程序放在一起，难以管理；
3. 运行的是 netcoreapp，依赖一大堆实际上用不上的程序集，包括 libuv，当我们只是写一个普通非 Web 程序时，这些都是不需要的；

## 这个有什么好处

1. 可以直接运行 netstandard 库，用到什么功能就依赖什么程序集，最小依赖原则；
2. 对 Gui 程序友好，可以编译成控制台程序，也可以编译成 Gui 程序（对于 .Net Core Gui，敬请关注 Quark 项目 - 使用 C# 开发 Qt Gui/Widgets 程序）；
3. 各程序使用各自进程名字，容易区分不易混淆；
4. BCL 库跟应用程序分别存放，分开慢速迭代和快速迭代的 DLL；
5. 可以把 tcmalloc 注入到进程，提高内存操作性能；

## 运行机制、各部件如何协调、如何使用

编译后产生3个文件：Run.exe、Environment.dll、Quasimodo.Bootstrap.dll：

### Run.exe

1. 这个程序是入口，用于初始化 CoreCLR 并加载要运行的程序集 - 并且这个程序可以改名，当文件名是 Run.exe 时，它加载 Run.dll 程序集，当文件名是 Benchmark.exe 时，它加载 Benchmark.dll 程序集，这个跟 CoreCLR 的 coreconsole.exe 相似；
2. 由于 netstandard library 没有 Main 入口，所以会读取程序集的 MainAttribute 找到入口，如 [assembly: Main(typeof(Benchmark.Global))]，并创建 Benchmark.Global 类的实例，这样控制器就已经交接给托管程序了；

### Environment.dll

1. 这个是为 Qt 集成预留的，Run.exe 会把程序的运行参数传给 Environment.dll 记录下来，当运行的程序集指定了需要 Qt 运行环境，Environment.dll 会取出这些运行参数作为 Qt 运行环境的配置参数；
2. 如何指定 Qt 运行环境：MainAttribute 有一个参数，接收一个 BootstrapType：Console 初始化 QCoreApplication、Gui 初始化 QGuiApplication、Widgets 初始化 QApplication，如果不指定这个参数，则不会启用 Qt 运行环境，Benchmark.Global 在构造完成后程序就会退出，若有需要驻留程序，请自行阻塞构造函数，如果指定了这个参数，那么主线程会被 Qt 的事件循环阻塞，Benchmark.Global 在构造后仍然驻留程序；
3. Qt 集成是懒加载的，如果不指定 Qt 集成，Qt 相关的 DLL 不需要部署；

### Quasimodo.Bootstrap.dll

1. 这是一个 netstandard1.3 程序集，请到 https://www.myget.org/F/quasimodo/api/v3/index.json 安装这个包；
2. 这个 DLL 定义了 MainAttribute，还有一些平台相关的 Attribute，例如启用了 [assembly: WindowsPlatformAttribute(errorReportHandle = true)]，一旦程序出错退出，则不会引发 Windows 错误报告；

### 文件布局

Assemblies\ （这个文件夹放置应用程序的 DLL）

Assemblies\Quasimodo.Bootstrap.dll

Assemblies\Benchmark.dll

Assemblies\Trusted\ （这个文件夹放置 BCL 的 DLL）

Assemblies\Trusted\System.Reflection.dll

Assemblies\Trusted\System.Reflection.Extensions.dll

Assemblies\Trusted\System.Reflection.TypeExtensions.dll

Assemblies\Trusted\System.Runtime.dll

Assemblies\Trusted\System.Runtime.Extensions.dll

Assemblies\Trusted\System.Runtime.InteropServices.RuntimeInformation.dll

clretwrc.dll

clrjit.dll

coreclr.dll

dbgshim.dll

mscordaccore.dll

mscordbi.dll

mscorrc.debug.dll

mscorrc.dll

sos.dll

System.Private.CoreLib.dll

Environment.dll

Benchmark.exe （这个文件由 Run.exe 改名而来）

## 源代码

这个项目使用 LGPLv3，当前版本为 1.0.5，请在分支 dev/1.0.5 上查看源代码。