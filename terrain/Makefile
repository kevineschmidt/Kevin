
OBJS= terrain.o


default: terrain

terrain: $(OBJS)
	g++ -o terrain $(OBJS) -L/Library/Frameworks -framework QtGui -framework QtCore -framework QtOpenGL -framework OpenGL -framework GLUT

.cpp.o:
	g++ -g -c -o $*.o $< -I /Library/Frameworks/QtGui.Framework/Headers -I /Library/Frameworks/QtOpenGL.Framework/Headers -I/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/GLUT.framework/Headers