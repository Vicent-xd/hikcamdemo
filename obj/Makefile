#SDK测试程序

CXX   =  g++
CXXFLAGS = -Wall -Wno-strict-aliasing -Wno-unused-variable

#包含头文件路径
SUBDIR   = $(shell ls ../src -R | grep /)
SUBDIRS  = $(subst :,/,$(SUBDIR))
INCPATHS = $(subst ../,-I../,$(SUBDIRS))
INCPATHS += -I../include/
INCPATHS += -I/usr/include/opencv4/opencv2/ \
	    -I/usr/include/opencv4/

VPATH = $(subst : ,:,$(SUBDIR))./
SOURCE = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*.cpp))
#将cpp的路径名称的.cpp后缀换为.o
OBJS = $(patsubst %.cpp,%.o,$(SOURCE))
#取出.o的文件名，不带路径
OBJFILE  = $(foreach dir,$(OBJS),$(notdir $(dir)))
#.o输出到.OBJ目录下
OBJSPATH = $(addprefix obj/,$(OBJFILE)) 

LIBPATH = ../lib/
LIBS = -Wl,-rpath=./:./HCNetSDKCom:../lib -lhcnetsdk -lPlayCtrl -lpthread `pkg-config --libs opencv4 2> /dev/null || pkg-config --libs opencv`
#LIBS += -lopencv4
#thirdparty_root=/usr/lib/x86_64-linux-gnu/
#LIBS += -L${thirdparty_root} -lopencv_imgcodecs -lopencv_dnn -lopencv_imgproc -lopencv_core
EXE = ../lib/sdkTest

$(EXE):$(OBJFILE)
	$(CXX) -L$(LIBPATH)  -o $(EXE) $(OBJFILE) $(INCPATHS) $(LIBS)
#libhikcam.so:$(OBJFILE)
	$(CXX) -L$(LIBPATH) $(OBJFILE) $(INCPATHS) $(LIBS) -fPIC -o ../lib/libhikcam.so
$(OBJFILE):%.o:%.cpp
	$(CXX)  -c -o $@ $<  $(INCPATHS) -pipe -g -Wall

#依赖文件
DPPS = $(patsubst %.cpp,%.dpp,$(SOURCE))
include $(DPPS)
%.dpp: %.cpp	
	g++ $(INCPATHS) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$ 
	

.PHONY:clean
clean:
	rm -rf $(OBJFILE)
	rm -rf $(DPPS)
	rm -rf $(EXE)

