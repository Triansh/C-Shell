#make file - this is a comment section

all:    #target name
	gcc main.c stringTokenize.c commandList.c executeCommand.c cd.c echo.c envVariables.c pwd.c ls.c prompt.c killProcess.c pinfo.c history.c jobs.c  changeProcessGroup.c otherCommands.c -o main