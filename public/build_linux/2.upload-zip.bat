@echo off
call 0.env.bat


psftp %HOST% -l %USER% -pw %PASS% -be -b cmds.ftp
del %target%.zip
