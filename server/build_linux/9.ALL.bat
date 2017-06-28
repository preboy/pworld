@echo off

echo preparing ...
call 1.gen-zip.bat

echo upload ...
call 2.upload-zip.bat

echo building ...
call 3.build.bat

echo Done !!!
puase
