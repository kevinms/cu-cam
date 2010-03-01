CC=gcc
CFLAGS=-g -Wall `pkg-config gnutls --cflags`

# Build cu-cam
LDFLAGS=
SOURCES=config.c plugin.c list.c utils.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=cu-cam

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# Build tests
T_CONFIG_SOURCES=config-test.c config.c list.c utils.c
T_CONFIG_OBJECTS=$(T_CONFIG_SOURCES:.c=.o)

T_AUTH_LDFLAGS=`pkg-config gnutls --libs`
T_AUTH_SOURCES=auth-test.c auth.c
T_AUTH_OBJECTS=$(T_AUTH_SOURCES:.c=.o)

T_SOURCES = $(T_CONFIG_SOURCES) $(T_AUTH_SOURCES)
T_EXECUTABLE=test-config test-auth

build-test: $(T_EXECUTABLE)
	

test-config: $(T_CONFIG_OBJECTS)
	$(CC) $(LDFLAGS) $(T_CONFIG_OBJECTS) -o $@

test-auth: $(T_AUTH_OBJECTS)
	$(CC) $(T_AUTH_LDFLAGS) $(T_AUTH_OBJECTS) -o $@


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
