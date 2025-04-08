CFLAGS = --std=c99 -Wall -Werror #-D_GNU_SOURCE
LDFLAGS = -lm -lc
CC = gcc
MONITORFUNCTIONS = monitorMemory monitorCpu monitorCores 
SIGNALLIB = signalhandler.h setsignal.h


.PHONY: all clean

all: $(MONITORFUNCTIONS) myMonitoringTool 



monitorMemory: draw.o graph.o childsignalhandler.o setsignal.o monitorMemory.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);

monitorCpu: draw.o graph.o childsignalhandler.o setsignal.o monitorCpu.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);

monitorCores: draw.o graph.o childsignalhandler.o setsignal.o monitorCores.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);

myMonitoringTool: draw.o getArguments.o parentsignalhandler.o setsignal.o myMonitoringTool.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);



parentsignalhandler.o : parentsignalhandler.c $(SIGNALLIB)
	$(CC) $(CFLAGS) -c $< -o $@;

childsignalhandler.o : childsignalhandler.c $(SIGNALLIB)
	$(CC) $(CFLAGS) -c $< -o $@;

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@;

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@;



clean:
	rm *.o;
	rm -f $(MONITORFUNCTIONS) myMonitoringTool;


#parentsignalhandler.o : parentsignalhandler.c $(SIGNALLIB)
	#$(CC) $(CFLAGS) -c $< -o $@;

#childsignalhandler.o : childsignalhandler.c $(SIGNALLIB)
	#$(CC) $(CFLAGS) -c $< -o $@;

#%signalhandler.o : %signalhandler.c $(SIGNALLIB)
	#$(CC) $(CFLAGS) -c $< -o $@;

#monitorMemory.o : monitorMemory.c graph.h signalhandler.h
	#$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);

#monitorCpu.o : monitorCpu.c graph.h signalhandler.h
	#$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);

#monitorCores.o: monitorCores.c graph.h signalhandler.h
	#$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);

#myMonitoringTool.o: myMonitoringTool.c getArguments.h signalhandler.h
	#$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);