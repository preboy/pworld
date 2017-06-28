@echo off

echo preparing ...
call gen-zip.bat

echo upload ...
upload-zip.bat

echo done