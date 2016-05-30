@echo off

if not exist "C:\Program Files\Mercurial\" goto INSTALL_MERCURIAL
if not exist SDL "C:\Program Files\Mercurial\hg" clone http://hg.libsdl.org/SDL

cd SDL
"C:\Program Files\Mercurial\hg" update
cd ..

goto ENDE

:INSTALL_MERCURIAL
echo "Please install Mercurial x64"
pause

:ENDE
echo "finish."
pause
