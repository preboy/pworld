@echo off

cd .. 

set PATH="C:\program files\7-zip";%PATH%


set target=server


if exist %target% (
    rd /q /s %target%
)

if exist %target%.zip (
    del %target%.zip
)


mkdir %target%

xcopy /e /y src\* %target%\src\*
xcopy /e /y bin\world\* %target%\bin\world\*

7z.exe a %target%.zip %target%

rd /q /s %target%

move /Y %target%.zip build_linux\%target%.zip

cd build_linux
