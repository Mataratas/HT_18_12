SRC=main.cpp CSession.cpp CUser.cpp CMessage.cpp sha1.cpp
TARGET=ycpp28chat
SDIR=/usr/local/bin

ycpp28chat: $(SRC) 
	g++ -o $(TARGET) $(SRC) 
clean:
	rm -rf *.o *.a
install:
	install $(TARGET) $(SDIR)
uninstall:
	rm -rf $(SDIR)/$(TARGET)
