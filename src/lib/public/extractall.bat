@ECHO OFF
SETLOCAL EnableDelayedExpansion

:Loop
IF "%1"=="" GOTO Continue

FOR %%F in (%1) DO (

    SET LIBFILE=%%F
    SHIFT

    @ECHO !LIBFILE!

    FOR /F %%O IN ('lib /LIST !LIBFILE! /NOLOGO') DO (
        @SET OBJFILE=%%O
        @ECHO !OBJFILE!
        
        SET OBJPATH=%%~dO%%~pO
        SET OBJNAME=%%~nO

        IF NOT EXIST "!OBJPATH!" md !OBJPATH!

        IF EXIST "!OBJFILE!" ECHO !OBJFILE! exists, skipping...
        IF NOT EXIST "!OBJFILE!" lib /NOLOGO !LIBFILE! "/EXTRACT:!OBJFILE!" "/OUT:!OBJFILE!"
    )
)
GOTO Loop
:Continue