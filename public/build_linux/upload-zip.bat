@echo off


set PATH="D:\Tools\putty";%PATH%

set HOST=172.31.244.4
set USER=preboy
set PASS=zhang

psftp %HOST% -l %USER% -pw %PASS% -be -b cmds.ftp
rem del public.zip

putty %HOST% -l %USER% -pw %PASS% -m cmds.putty

echo Done

pause