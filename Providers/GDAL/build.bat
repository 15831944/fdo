@echo off

SET TYPEACTIONRFP=build
SET MSACTIONRFP=Build
SET TYPEBUILDRFP=release
SET FDOORGPATHRFP=%cd%
SET FDOINSPATHRFP=%cd%\Fdo
SET FDOBINPATHRFP=%cd%\Fdo\Bin
SET FDOINCPATHRFP=%cd%\Fdo\Inc
SET FDOLIBPATHRFP=%cd%\Fdo\Lib
SET FDODOCPATHRFP=%cd%\Fdo\Docs
SET DOCENABLERFP=skip
SET FDOERROR=0

:study_params
if (%1)==() goto start_build

if "%1"=="-help"    goto help_show
if "%1"=="-h"       goto help_show

if "%1"=="-o"       goto get_path
if "%1"=="-outpath" goto get_path

if "%1"=="-c"       goto get_conf
if "%1"=="-config"  goto get_conf

if "%1"=="-a"       goto get_action
if "%1"=="-action"  goto get_action

if "%1"=="-d"       goto get_docs
if "%1"=="-docs"    goto get_docs

goto custom_error

:get_docs
SET DOCENABLERFP=%2
if "%2"=="build" goto next_param
if "%2"=="skip" goto next_param
goto custom_error

:get_action
SET TYPEACTIONRFP=%2
if "%2"=="install" goto next_param
if "%2"=="build" goto next_param
if "%2"=="buildinstall" goto next_param
if "%2"=="clean" goto next_param
goto custom_error

:get_conf
SET TYPEBUILDRFP=%2
if "%2"=="release" goto next_param
if "%2"=="debug" goto next_param
goto custom_error

:get_path
if (%2)==() goto custom_error
SET FDOORGPATHRFP=%~2
SET FDOINSPATHRFP=%~2\Fdo
SET FDOBINPATHRFP=%~2\Fdo\Bin
SET FDOINCPATHRFP=%~2\Fdo\Inc
SET FDOLIBPATHRFP=%~2\Fdo\Lib
SET FDODOCPATHRFP=%~2\Fdo\Docs

:next_param
shift
shift
goto study_params

:start_build
SET FDOACTENVSTUDY="FDO"
if ("%FDO%")==("") goto env_error
if not exist "%FDO%" goto env_path_error
SET FDOACTENVSTUDY="FDOTHIRDPARTY"
if ("%FDOTHIRDPARTY%")==("") goto env_error
if not exist "%FDOTHIRDPARTY%" goto env_path_error
SET FDOACTENVSTUDY="FDOUTILITIES"
if ("%FDOUTILITIES%")==("") goto env_error
if not exist "%FDOUTILITIES%" goto env_path_error

if "%TYPEACTIONRFP%"=="build" goto start_exbuild
if "%TYPEACTIONRFP%"=="clean" goto start_exbuild
if not exist "%FDOINSPATHRFP%" mkdir "%FDOINSPATHRFP%"
if not exist "%FDOBINPATHRFP%" mkdir "%FDOBINPATHRFP%"
if not exist "%FDOINCPATHRFP%" mkdir "%FDOINCPATHRFP%"
if not exist "%FDOLIBPATHRFP%" mkdir "%FDOLIBPATHRFP%"
if not exist "%FDODOCPATHRFP%" mkdir "%FDODOCPATHRFP%"

:start_exbuild
time /t
if "%TYPEACTIONRFP%"=="clean" SET MSACTIONRFP=Clean
if "%TYPEACTIONRFP%"=="install" goto install_files_rfp

echo %MSACTIONRFP% %TYPEBUILDRFP% GdalFile provider dlls
pushd Src
SET FDOACTIVEBUILD=%cd%\RFP
cscript //job:prepare ../../../preparebuilds.wsf
msbuild RFP_temp.sln /t:%MSACTIONRFP% /p:Configuration=%TYPEBUILDRFP% /p:Platform="Win32" /nologo /consoleloggerparameters:NoSummary
SET FDOERROR=%errorlevel%
if exist RFP_temp.sln del /Q /F RFP_temp.sln
popd
if "%FDOERROR%"=="1" goto error
if "%TYPEACTIONRFP%"=="clean" goto end
if "%TYPEACTIONRFP%"=="build" goto generate_docs

:install_files_shp
echo Copy %TYPEBUILDRFP% GdalFile Provider Output Files
copy /y "Bin\Win32\%TYPEBUILDRFP%\GRFPMessage.dll" "%FDOBINPATHRFP%"
copy /y "Bin\Win32\%TYPEBUILDRFP%\GRFPOverrides.dll" "%FDOBINPATHRFP%"
copy /y "Lib\Win32\%TYPEBUILDRFP%\GRFPOverrides.lib" "%FDOLIBPATHRFP%"
copy /y "Bin\Win32\%TYPEBUILDRFP%\GRFPProvider.dll" "%FDOBINPATHRFP%"

echo Copy GdalFile SDK Header Files
xcopy /S /C /Q /R /Y Inc\GdalFile\*.h "%FDOINCPATHRFP%\GdalFile\"

:generate_docs
echo No documentation to Generate for GdalFile Provider
:install_docs
echo No documentation to Intsall for GdalFile Provider

:end
time /t
echo End RFP %MSACTIONRFP%
exit /B 0

:env_error
echo Environment variable undefined: %FDOACTENVSTUDY%
SET FDOERROR=1
time /t
exit /B 1

:env_path_error
echo Invalid path contained in FDO environment variable: %FDOACTENVSTUDY%
SET FDOERROR=1
time /t
exit /B 1

:env_path_error_ex
echo Unable to find location of %FDOACTENVSTUDY% in the Windows System PATH
SET FDOERROR=1
time /t
exit /B 1

:error
echo There was a build error executing action: %MSACTIONRFP%
time /t
exit /B 1

:custom_error
echo The command is not recognized.
echo Please use the format:
:help_show
echo **************************************************************************
echo build.bat [-h] [-o=OutFolder] [-c=BuildType] [-a=Action] [-d=BuildDocs]
echo *
echo Help:           -h[elp]
echo OutFolder:      -o[utpath]=destination folder for binaries
echo BuildType:      -c[onfig]=release(default), debug
echo Action:         -a[ction]=build(default), buildinstall, install, clean
echo BuildDocs:      -d[ocs]=skip(default), build
echo **************************************************************************
exit /B 0