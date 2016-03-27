# @author: Daniel Tuchscherer

# //////////////////////////////////////////////////////////////////////////////
# PROJECT SPECIFIC SETTINGS
# //////////////////////////////////////////////////////////////////////////////
## PLEASE SPECIFY THE EXECUTABLE TARGETS
MY_TARGET_SRCS+=

BSW_BASE_DIR=bsw
BSW_BASE_SRC_DIR=$(BSW_BASE_DIR)/src

## BASIC SOFTWARE
BSW_SOURCES=CanSocket.cpp \
			IpAddress.cpp \
			TcpClient.cpp \
			TcpServer.cpp \
			TcpSocket.cpp
			
BSW_INCLUDES=$(BSW_BASE_SRC_DIR) \
				$(BSW_BASE_SRC_DIR)/system \
				$(BSW_BASE_SRC_DIR)/communication

MY_INCLUDES+=$(addprefix -I,$(BSW_INCLUDES))

VPATH+=$(BSW_BASE_SRC_DIR) $(BSW_BASE_SRC_DIR)/communication $(BSW_BASE_SRC_DIR)/system

# //////////////////////////////////////////////////////////////////////////////
include application/URSensorhead/URSensorhead.mk

# //////////////////////////////////////////////////////////////////////////////
include application/URSensorheadSim/URSensorheadSim.mk
	
# //////////////////////////////////////////////////////////////////////////////

## SPECIFY YOUR LIBRARIES YOU WANT TO LINK AGAINST IN THE LINKING PROCESS
MY_LIBS+=-lrt \
		-lpthread \
		-pg

## SPECIFY DEFINES FOR THE COMPILER
MY_DEFINES+=-DDEBUG

## SPECIFY ALL COMPILER WARNINGS
MY_WARNINGS+=-Wall \
			-Wextra \
			-Wpedantic

## SPECIFY YOUR GCC COMPILER FLAGS
MY_CFLAGS+=

## WORK-AROUND IF THE CROSS-COMPILER DOESN'T KNOW
## -std=c++11
ifneq ($(CROSS_COMPILE),)
  export MY_CXXFLAGS=-std=c++1y -fno-exceptions -fno-rtti
endif

## SPECIFY ADDITIONAL G++ COMPILER FLAGS
MY_CXXFLAGS=-std=c++14 -fno-exceptions -fno-rtti -O0 -g -pg

# ------------------------------------------------------------------------------
# GENERIC SETTINGS
# ------------------------------------------------------------------------------
## DO NOT MODIFY!
OBJ_DIR=obj
BIN_DIR=bin
TARGETS=$(MY_TARGET_SRCS:.cpp=)
DEFINES=$(MY_DEFINES)
WARNINGS=$(MY_WARNINGS)
INCLUDES=$(MY_INCLUDES)
LIBS=$(MY_LIBS)

## DO NOT MODIFY THE COMPILER SETTINGS
CC=gcc
CXX=g++
LD=g++
CFLAGS=$(INCLUDES) $(DEFINES) $(MY_CFLAGS) $(WARNINGS)
CXXFLAGS=$(CFLAGS) $(MY_CXXFLAGS)
LDFLAGS=$(LIBS)

# GENERIC CROSS-COMPILATION
ifneq ($(CROSS_COMPILE),)
  export CROSS_CC=$(CROSS_COMPILE)gcc
  export CROSS_CXX=$(CROSS_COMPILE)g++
  export CROSS_LD=$(CROSS_COMPILE)g++
endif

# ------------------------------------------------------------------------------
# TARGETS
# ------------------------------------------------------------------------------
.PHONY: all
all: install $(TARGETS)

install:
	mkdir -p $(BIN_DIR) $(OBJ_DIR)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: copy
copy:
	sshpass -p "hhn" scp -r $(BIN_DIR)/rt_can root@192.168.3.11:/home/.
	sshpass -p "hhn" scp -r $(BIN_DIR)/rt_tcp root@192.168.3.11:/home/.

%.o: %.c
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/$@ -c $<