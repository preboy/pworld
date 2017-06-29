@echo off
call 0.env.bat


putty %HOST% -l %USER% -pw %PASS% -m cmds.putty
