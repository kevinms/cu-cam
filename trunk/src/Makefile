CC=gcc
CFLAGS=-g -Wall `pkg-config gnutls --cflags`

#-----------------------------------------------------------------------
# Build cu-cam
LDFLAGS=
SERVER_SOURCES=cam-server.c command.c config.c net.c list.c utils.c
SERVER_OBJECTS=$(SERVER_SOURCES:.c=.o)

CLIENT_SOURCES=cam-client.c command.c config.c net.c list.c utils.c
CLIENT_OBJECTS=$(CLIENT_SOURCES:.c=.o)

EXECUTABLE=cam-server cam-client

all: $(EXECUTABLE)
	

cam-server: $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@

cam-client: $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $(CLIENT_OBJECTS) -o $@

#-----------------------------------------------------------------------
# Build tests
T_CONFIG_SOURCES=config-test.c config.c list.c utils.c
T_CONFIG_OBJECTS=$(T_CONFIG_SOURCES:.c=.o)

T_AUTH_LDFLAGS=`pkg-config gnutls --libs`
T_AUTH_SOURCES=auth-test.c auth.c
T_AUTH_OBJECTS=$(T_AUTH_SOURCES:.c=.o)

T_COMMAND_SOURCES=command-test.c command.c list.c utils.c
T_COMMAND_OBJECTS=$(T_COMMAND_SOURCES:.c=.o)

T_SOURCES = $(T_CONFIG_SOURCES) $(T_AUTH_SOURCES)
T_EXECUTABLE=test-config test-auth test-command

build-test: $(T_EXECUTABLE)
	

test-config: $(T_CONFIG_OBJECTS)
	$(CC) $(LDFLAGS) $(T_CONFIG_OBJECTS) -o $@

test-auth: $(T_AUTH_OBJECTS)
	$(CC) $(T_AUTH_LDFLAGS) $(T_AUTH_OBJECTS) -o $@

test-command: $(T_COMMAND_OBJECTS)
	$(CC) $(T_LDFLAGS) $(T_COMMAND_OBJECTS) -o $@
#-----------------------------------------------------------------------

# Implicit rule to compile all .c to .o
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

# Cleanup rules

clean:
	rm -fv *.o $(EXECUTABLE) pbuogt-out pbuogt-event pbuogt-script *~

clean-test:
	rm -fv *.o $(T_EXECUTABLE) *~

#Karl's Shortcuts
#Testing Netbeans abilities
test:
	./test-config
rm:
	clear
	make clean
	make clean-test
	make build-test
	clear
bt:
	make build-test

cl:
	clear
	make clean
	make clean-test
	clear
	@echo All Cleared
