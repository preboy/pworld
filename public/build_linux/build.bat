@echo off

echo preparing ...
call gen-zip.bat

echo upload ...
call upload-zip.bat

echo building ...