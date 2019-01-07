call del obj\test.obj
call del bin\test.exe
call ml /Fo obj/test.obj /c test.asm
call link /OUT:bin/test.exe obj/test.obj
