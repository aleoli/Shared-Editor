@echo off

set /p loglevel=Enter log level (none - error - warn - info - debug):

..\bin\testClient.exe -l %loglevel%

set /p=Hit ENTER to close the window..