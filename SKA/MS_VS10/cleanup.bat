rem - Rather clumsy way to delete unwanted stuff that is created
rem - by the SKA Visual Studio project. Everything deleted is 
rem - within the current floder (MS_VS10) except for the last
rem - two files which are the SKA dll files that are copied out
rem - to SKA/lib.

rmdir /s/q app0001\Debug
del /q app0001\*.user
del /q app0001\system_log.txt

rmdir /s/q app0002\Debug
del /q app0002\*.user
del /q app0002\system_log.txt

rmdir /s/q app0003\Debug
del /q app0003\*.user
del /q app0003\system_log.txt

rmdir /s/q app0004\Debug
del /q app0004\*.user
del /q app0004\system_log.txt

rmdir /s/q app0005\Debug
del /q app0005\*.user
del /q app0005\system_log.txt

rmdir /s/q app1001\Debug
del /q app1001\*.user
del /q app1001\system_log.txt
del /q app1001\*.dot
del /q app1001\test.txt

rmdir /s/q skadll\Debug
del /q skadll\*.user

rmdir /s/q Debug
rmdir /s/q ipch
del /q SKA.sdf
del /q /ah SKA.suo

rmdir /s/q ipch
del ..\SKA\lib\skadll.lib
del ..\SKA\lib\skadll.dll
