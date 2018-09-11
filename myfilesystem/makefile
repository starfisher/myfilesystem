obj = main.o filesocket.o filerw.o fileblockrw.o tools.o command.o explain.o fileuserrw.o

filesystem : $(obj)
	cc -o filesystem $(obj)

$(obj) : filesym.h

.PHONY : clean
clean :
	-rm filesystem $(obj)
