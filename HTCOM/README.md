### HTCOM
Для работы с dll, используйте в консоли, запущенной от администратора, команду
```
 	regsvr32 /i OS13_HTCOM.dll
```
в папке _/x64/Debug_

После чего можно использовать другие скомпилированные **.exe** файлы через _visual studio_
Для запуска **.exe** через консоль нужно изменить строчку в **OS13_HTCOM_LIB.cpp** в функции **RecordingWork** *(строка 374)*: 
```C++
	std::wstring filePath(L"..\\Records\\" + typeOperation + L"\\" + date);
```
На строку:
```C++
	std::wstring filePath(L"..\\..\\Records\\" + typeOperation + L"\\" + date);
```
