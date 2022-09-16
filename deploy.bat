echo Compile, Copy the WinNotify DLL

set DLL_SOURCE=x64\Debug\portmaster-wintoast.dll
del DLL_SOURCE

msbuild /t:Build /p:Configuration=Debug /p:Platform=x64

echo Copy the dll to updates dir as 0-1-0 version
copy %DLL_SOURCE% C:\ProgramData\Safing\Portmaster\updates\windows_amd64\notifier\portmaster-wintoast_v0-1-0.dll