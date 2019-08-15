
include ./makefile.rule

INC_PATH += -I./inc
INC_PATH += -I../common
INC_PATH += -I./
INC_PATH += -I../../common/include/dms_api_include



CFLAGS += -DHI_ACODEC_TYPE_NVP6124 -DHI_ACODEC_TYPE_HDMI 
CFLAGS +=   $(INC_PATH) 
CFLAGS +=  $(INC_FLAGS)


LIBFILE += -lpthread -lm -lstdc++

LIBDIR=$(PROJECT_DIR)/common/lib

SRCS    :=  $(wildcard *.cpp)
OBJS    :=  $(SRCS:%.cpp=%.o)

SRCEXE :=   main.c upload.c   

MY_OBJS := libupload.o

STATICLIB := libupload.a

TARGET :=  libupload.so
.PHONY : clean all

$(TARGET):$(OBJS)
ifeq ($(BUILD_STATIC_LIB), n)	
	$(CC)  -fPIC -shared  $(CFLAGS)  -o $@ $^ 
else
	@rm -rf $(STATICLIB) 
	$(AR) -r $(STATICLIB) $(OBJS)

endif

%.o:%.c 
	$(CC)   $(CFLAGS)   -c $< -o $@ 	

%.o:%.cpp
	$(CC)   $(CFLAGS)  -c $< -o $@

all :
	rm -f upload_test
	$(CC)  $(SRCEXE) -o upload_test $(LIBFILE) $(CFLAGS) $(INCLUDE) $(LIBS) $(MPI_LIBS) -L./ -lupload  -Os -rdynamic  -ldl 


	

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJS)
	@rm -rf $(STATICLIB) 

