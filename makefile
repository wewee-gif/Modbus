# gcc -v 添加详细输出 
UV_FLAGS=-I/d/CProject/Clib/libuv-v1.48.0/include -L/d/CProject/Clib/libuv-v1.48.0 -luv
#UV_FLAGS=-ID:\CProject\Clib\libuv-v1.48.0\include -LD:\CProject\Clib\libuv-v1.48.0 -luv
#UV_FLAGS=-I/home/rci/lib/libuv-v1.48.0/include -L/home/rci/lib/libuv-v1.48.0 -luv
objs3 := modbus_client.o hash.o converter.o

modbus_client :$(objs3)
	gcc -o modbus_client $^ $(UV_FLAGS) -ladvapi32 -liphlpapi -lpsapi -lshell32 -luser32 -luserenv -lws2_32 -lole32 -lDbgHelp -lwinmm -Wl,-rpath,'$$ORIGIN'
%.o : %.c
	gcc -c -g $(UV_FLAGS)  $< -o $@ 

clean: 
	rm *.o  modbus_client -f 
	