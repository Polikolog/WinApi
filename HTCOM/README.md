### HTCOM
Для работы с dll, используйте в консоли, запущенной от администратора, команду #__regsvr32 /i OS13_HTCOM.dll__ в папке _/x64/Debug_
После чего можно использовать другие скомпилированные **.exe** файлы через _visual studio_
Для запуска **.exe** через консоль нужно изменить строчку: 
```C++
	std::wstring filePath(L"..\\Records\\" + typeOperation + L"\\" + date);
```
На строку:
```C++
	std::wstring filePath(L"..\\..\\Records\\" + typeOperation + L"\\" + date);
```
