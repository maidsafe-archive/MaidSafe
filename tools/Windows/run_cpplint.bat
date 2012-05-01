@echo off
setlocal
if (%1)==() (call :set_default_rootpath) else (set rootpath=%1)
echo Checking files - each "." represents a single file.
dir %rootpath%\src\*.cc /s /b >%temp%\filelist.txt
dir %rootpath%\src\*.h /s /b >>%temp%\filelist.txt
findstr /i /v "\boost sqlite3.h \cryptopp \libupnp \widgets \qwt_widgets .pb." %temp%\filelist.txt > %temp%\filelist2.txt
echo Setup>%temp%\code_style_errors.txt
set count=0
for /f %%g in (%temp%\filelist2.txt) do (
  @"cmd /c %2 "%%g" 2>>%temp%\code_style_errors.txt"
  <nul (set/p z=".")
)
findstr /i /v /b "Setup Done Total" %temp%\code_style_errors.txt > %temp%\code_style_errors2.txt
set count=0
for /f  %%g in (%temp%\code_style_errors2.txt) do (call :s_do_sums)
echo. & echo.
type %temp%\code_style_errors2.txt
echo. & echo.
if %count% geq 1 echo There are %count% errors! & call :function & exit /B 1
if %count% equ 0 echo There aren't any errors.
del %temp%\filelist.txt %temp%\filelist2.txt %temp%\code_style_errors.txt %temp%\code_style_errors2.txt
echo.
:s_do_sums
 set /a count+=1
 goto :eof
:function
 echo.
del %temp%\filelist.txt %temp%\filelist2.txt %temp%\code_style_errors.txt %temp%\code_style_errors2.txt
 exit /B 1
 goto :eof
:set_default_rootpath
 cd ..\..\
 set rootpath=%cd%
 cd build\Win_MSVC
