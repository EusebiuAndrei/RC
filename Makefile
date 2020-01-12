#fisier folosit pentru compilarea serverului concurent & clientului TCP

all:
	gcc servTcpConc.c -o servTcpConc -lsqlite3
	gcc cliTcpConc.c -o cliTcpConc -lsqlite3
clean:
	rm -f *~ cliTcpConc servTcpConc