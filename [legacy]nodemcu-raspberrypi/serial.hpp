#ifndef __SERIAL__
#define __SERIAL__

#include <cstdio>
#include <cstdlib>

#include <string.h> //for errno
#include <errno.h> //error output

#include <wiringPi.h>
#include <wiringSerial.h>

#include <thread>

#include "note.h"

extern bool continue_flag;

class Serial_buffer
{
private:
    char* buffer;
    int size;
    int top;

public:
    Serial_buffer(int _size = 500) : size(_size)
    {
        buffer = new char[size];
        this->top = 0;
    }
    ~Serial_buffer()
    {
        if(buffer != NULL)
            delete buffer;
    }
    inline void push(char value)
    {
        if(this->top == size)
        {
            fprintf(stderr, "buffer full!\n");
            return;
        }
        buffer[(this->top)++] = value;
    }
    inline void finish()
    {
        buffer[--(this->top)] = '\0';
    }

    inline int gettop() {return this->top;}
    inline char* data() {return this->buffer;}

    char& operator[](int index)
    {
        return buffer[index];
    }
    char operator[](int index) const
    {
        return buffer[index];
    }
    inline void refresh()
    {
        this->top = 0;
    }
};

class Serial_io
{
    Serial_buffer* buff;
    int serial_fd;

    std::thread pthread;
    //Serial에서 지속적으로 문자열을 받는 Serial을 만든다.
    static void thread_readSerial(int fd);

    public:
    Serial_io()
    {
	    continue_flag = true;
        buff = new Serial_buffer(100);
        //Serial port 연다.
        if ((serial_fd = serialOpen ("/dev/ttyACM0", 115200)) < 0)  // 두번째 인자값이 보레이트 설정
        {
            fprintf (stderr, "Unable to open serial device : %s\n",strerror(errno)) ;                   
            exit(-1);
        }
        if (wiringPiSetup() == -1)
        { 
            fprintf (stdout, "Unable to start wiringPi : %s\n", strerror(errno)) ;
            exit(-1);
        }

        pthread = std::thread(thread_readSerial, serial_fd);
    }
    ~Serial_io()
    {
        pthread.join();
	    serialFlush(serial_fd);
	    serialClose(serial_fd);
        if(buff != NULL)
            delete buff;
    }


    //Serial에서 Protocol을 완성하는 문자열을 받을 떄까지 낱개 문자를 받는다.
    int readSerial();
    //Serial에서 받은 문자열을 parameter로 넘어온 note 정보에 저장한다.
    int setSerial(note* pnote);
    //Serial에서 문자열을 보낸다.
    int writeSerial(char* buf);
    //Serial에서 note를 보낸다.
    int writeNote(note note);
    //Serial에서 받아온 버퍼를 비운다.
    int cleanSerial();
};

#endif
