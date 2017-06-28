@echo off

cd .. 

set PATH="C:\program files\7-zip";%PATH%


set target=public


if exist %target% (
    rd /q /s %target%
)

if exist %target%.zip (
    del %target%.zip
)


mkdir %target%

xcopy /e /y common\* %target%\common\*

xcopy /e /y libcore\include\* %target%\libcore\include\*
xcopy /e /y libcore\src\* %target%\libcore\src\*
echo F | xcopy /e /y libcore\Makefile %target%\libcore\Makefile

7z.exe a %target%.zip %target%

rd /q /s %target%

move /Y %target%.zip build_linux\%target%.zip

cd build_linux
