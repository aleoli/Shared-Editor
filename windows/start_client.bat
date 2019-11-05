@echo off

set /p loglevel=Enter log level (none - error - warn - info - debug):

..\bin\client.exe -l %loglevel%

set /p=Hit ENTER to close the window..
