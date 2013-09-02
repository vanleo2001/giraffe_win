CXX=g++

CXXFLAGS=-g 

INC=-I/usr/include/libxml2  

LIBS=-lpcap -lpthread -llog -lzmq -lluajit-5.1 -ldl -lxml2

OBJS= monitor.o utils.o log.o capture_net_packet.o handle_net_packet.o parse.o luaroutine.o configparser.o DIDTemplateToLuaStruct.o bitstream.o crc16.o mword.o extract_dc.o crc16.o DidUncompress.o dzh_time_t.o generalcps.o l2compress.o pub_pfloat.o public.o MSXML_DOMDocument.o

TARGET=monitor

all:$(TARGET) 

$(TARGET):$(OBJS) 
	$(CXX) -o $@ $^ $(LIBS) 

monitor.o:monitor.cc
	$(CXX) $(CXXFLAGS) $(INC) -c $<

clean:
	rm -f *.o 
