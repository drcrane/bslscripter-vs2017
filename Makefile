CXXFLAGS := -std=c++0x -fvisibility=hidden -fvisibility-inlines-hidden -Wall 

DEFINES := -DUNIX

CXXFLAGS += -Os

ifdef DEBUG
CXXFLAGS += -g -O0
else
CXXFLAGS += -Os
DEFINES += -DNDEBUG
endif

export BOOST_DIR
export BIT32
export BIT64
export STATIC
export DEBUG

INCLUDES := \
	-I./ThirdParty/include \
	-I. 
	
LIBS :=

STATIC_LIBS :=

ifdef STATIC
STATIC_LIBS += -lboost_filesystem -lboost_system -lboost_program_options 
else
LIBS += -lboost_filesystem -lboost_system -lboost_program_options
endif

LIBTHIRD := ./ThirdParty/lib64
LIBDIRS := -L$(LIBTHIRD)

PLATFORM := $(shell uname -s)
ifeq ($(PLATFORM),Linux)
	CXX:= g++
	
	ifdef BIT64
	OUTPUT := bsl-scripter-linux-64
	endif
	
	ifdef BIT32
	CXXFLAGS += -m32
	LIBTHIRD = ./ThirdParty/lib
	LIBDIRS = -L$(LIBTHIRD)
	OUTPUT := bsl-scripter-linux-32
	endif

	DEFINES += -DUNIX

	ifdef STATIC
	STATIC_LIBS += -lusb-1.0 
	else
	LIBS += -lusb-1.0 
	endif

	LIBS += -lusb-1.0 -lrt -lpthread -lc

	ifdef BOOST_DIR
	INCLUDES += -I$(BOOST_DIR)
	LIBDIRS += -L$(BOOST_DIR)/stage/lib
	endif
	
	BSTATIC := -Wl,-Bstatic
	BDYNAMIC := -Wl,-Bdynamic

	HIDOBJ := $(LIBTHIRD)/hid-libusb.o
	
else
	CXX:= clang++

	OUTPUT := bsl-scripter-mac

	ifdef STATIC
	STATIC_LIBS += -framework CoreFoundation -framework IOKit -lhidapi -lobjc
	else
	LIBS += -framework CoreFoundation -framework IOKit -lhidapi -lobjc
	endif

	ifdef BOOST_DIR
	INCLUDES += -I$(BOOST_DIR)/include
	LIBDIRS += -L$(BOOST_DIR)/lib
	endif
	
	BSTATIC := 
	BDYNAMIC := 
	
	DEFINES :=

	HIDOBJ := $(LIBTHIRD)/libusb-mac-1.0.a
	
endif

OBJS := BSL_Scripter.o Util.o BslResponseHandler.o BslE4xxResponseHandler.o BslE4xxEthernetResponseHandler.o UsbComm.o UartComm.o UartE4xxComm.o EthernetComm.o UsbDfuComm.o Family16bit.o Family32bit.o Family32bit_E4xx.o Family32bit_E4xxEthernet.o Family32bit_E4xxUsbDfu.o BaseCommands.o Timer.o Interpreter.o ScripterStream.o FactoryTarget.o FactoryCommunication.o 

all:  $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT) $^ $(HIDOBJ) $(LIBDIRS) $(BSTATIC) $(STATIC_LIBS) $(BDYNAMIC) $(LIBS)

%.o: %.cpp 
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(INCLUDES) $(DEFINES)

clean:
	@for i in $(OBJS); do rm -f $$i; done
	@rm -f $(OUTPUT)
