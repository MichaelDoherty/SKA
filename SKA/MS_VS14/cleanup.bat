rem - Rather clumsy way to delete unwanted stuff that is created
rem - by the SKA Visual Studio project. Everything deleted is 
rem - within the current floder (MS_VS10) except for the last
rem - two files which are the SKA dll files that are copied out
rem - to SKA/lib.

rmdir /s/q app0001\Debug
rmdir /s/q app0001\Release
del /q app0001\*.user
del /q app0001\system_log.txt

rmdir /s/q app0002\Debug
rmdir /s/q app0002\Release
del /q app0002\*.user
del /q app0002\system_log.txt

rmdir /s/q app0003\Debug
rmdir /s/q app0003\Release
del /q app0003\*.user
del /q app0003\system_log.txt

rmdir /s/q app0004\Debug
rmdir /s/q app0004\Release
del /q app0004\*.user
del /q app0004\system_log.txt

rmdir /s/q app0005\Debug
rmdir /s/q app0005\Release
del /q app0005\*.user
del /q app0005\system_log.txt

rmdir /s/q app1001\Debug
rmdir /s/q app1001\Release
del /q app1001\*.user
del /q app1001\system_log.txt
del /q app1001\*.dot
del /q app1001\test.txt

rmdir /s/q MotionDescriptors\Debug
rmdir /s/q MotionDescriptors\Release
del /q MotionDescriptors\*.user
del /q MotionDescriptors\system_log.txt
del /q MotionDescriptors\results_*.txt

rmdir /s/q PTAnalysis\Debug
rmdir /s/q PTAnalysis\Release
del /q PTAnalysis\*.user
del /q PTAnalysis\system_log.txt
del /q PTAnalysis\results_*.txt

rmdir /s/q NetworkProcessing\Debug
rmdir /s/q NetworkProcessing\Release
del /q NetworkProcessing\*.user
del /q NetworkProcessing\system_log.txt
del /q NetworkProcessing\results_*.txt

rmdir /s/q ska\Debug
rmdir /s/q ska\Release
del /q ska\*.user

rmdir /s/q Debug
rmdir /s/q Release
rmdir /s/q ipch
del /q SKA.sdf
del /q /ah SKA.suo

rmdir /s/q ipch
del ..\SKA\lib\skad.lib
del ..\SKA\lib\skad.dll
del ..\SKA\lib\ska.lib
del ..\SKA\lib\ska.dll