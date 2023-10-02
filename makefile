.SILENT:

EXECUTABLES = CreationBD Client ServeurExec  


LibClientQt = ClientQt
LibSocket = Socket
LibServeur = Serveur
LibCreationBD = BD_Maraicher
LIbOVESP = Serveur/OVESP

COMP = g++ -I $(LibClientQt) -I $(LibSocket) -I$(LibServeur) -I $(LibCreationBD) -I $(LIbOVESP) -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I/usr/include/mysql -lpthread -L/usr/lib64/mysql -lmysqlclient
OBJS = $(LibClientQt)/mainclient.o $(LibClientQt)/windowclient.o $(LibClientQt)/moc_windowclient.o $(LibSocket)/socket.o $(LIbOVESP)/OVESP.o

all : Client ServeurExec CreationBD

Client : $(OBJS)
	echo executable Client
	$(COMP) -o $@ $^ /usr/lib64/libQt5Widgets.so /usr/lib64/libQt5Gui.so /usr/lib64/libQt5Core.so /usr/lib64/libGL.so -lpthread

ServeurExec : $(LibServeur)/Serveur.cpp
	echo executable Serveur
#	g++ Serveur.cpp -o Serveur $(LibSocket)/socket.o -I/usr/include/mysql -lpthread -L/usr/lib64/mysql -lmysqlclient
	$(COMP) $< -o $@ $(LibSocket)/socket.o $(LIbOVESP)/OVESP.o  -I/usr/include/mysql -lpthread -L/usr/lib64/mysql -lmysqlclient

CreationBD : $(LibCreationBD)/CreationBD.cpp
	echo executable CreationBD
	$(COMP) $< -o $@ -I/usr/include/mysql -m64 -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl

$(LibClientQt)/%.o : $(LibClientQt)/%.cpp
	echo .o de ClientQt
	$(COMP) -c $< -o $@ -fPIC

$(LibSocket)/socket.o:   $(LibSocket)/socket.cpp $(LibSocket)/socket.h
	echo socket.o
	$(COMP) -c $< -o $@ -fPIC

$(LIbOVESP)/OVESP.o:	$(LIbOVESP)/OVESP.cpp $(LIbOVESP)/OVESP.h
	echo OVESP.o
	$(COMP) -c $< -o $@ -fPIC


clean:
	echo suppression .o
	rm -f $(OBJS)

clean_executables :
	echo suppression exec
	rm -f $(EXECUTABLES) *o

clean_all: clean clean_executables
	rm -rf build
	rm -f core
	rm -f *~
