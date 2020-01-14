#fisier folosit pentru compilarea serverului concurent & clientului TCP

all:
	gcc ./server/servTcpConc.c -o ./server/servTcpConc -lsqlite3
	gcc ./client/cliTcpConc.c -o ./client/cliTcpConc -lsqlite3
clean:
	rm -f *~ ./client/cliTcpConc ./server/servTcpConc