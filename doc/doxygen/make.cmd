@echo off

echo /////////////////////////////////////////////////////////////////////
echo Making unicomm documentation...
echo.
doxygen Doxyfile
copy /Y ..\sources\template\css\navtree.css ..\manuals\html\
copy /Y ..\sources\template\images\body-bg.png /B ..\manuals\html\

@echo 
