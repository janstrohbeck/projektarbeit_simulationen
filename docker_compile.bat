@ECHO OFF
IF "%1"=="" GOTO Error
docker run -it -v %cd%:/data jstroh/latex_sphinx_plantuml make -C %*
GOTO End

:Error
echo "You need to specify the subfolder!"
:End
