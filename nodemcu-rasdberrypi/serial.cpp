#include "serial.hpp"
#include <queue>
#include <mutex>
//input thread에서 serial로 받아온 문자들을 저장하는 공간.
std::queue<char> ch_queue;
//queue의 원자성을 보존하기 위한 mutex
std::mutex queue_lock;

void Serial_io::thread_getSerial(int fd)
{
    char ch;
    //serialGetchar blocking 함수.. character 받을 떄까지 대기한다. 
    while(true)
    {
        if(serialDataAvail(fd))
        {
            ch = serialGetchar(fd);
            queue_lock.lock();
            ch_queue.push(ch);
            queue_lock.unlock();
       } 
    }
}

int Serial_io::getSerial()
{
    bool exists;
    char ch = 0;
    buff->refresh();
    while(ch != 10)
    {
	exists = false;
        queue_lock.lock();
        if(!ch_queue.empty())
	{
		exists = true;
		ch = ch_queue.front();
        	ch_queue.pop();
	}
        queue_lock.unlock();
        
	if(exists)
	{
        	buff->push(ch);
         	//printf("%c(%d)\n",ch,ch);
	}
    }
    buff->finish();
    //printf("blocking test : %s\n", buff->data());
    return 1;
}

int Serial_io::setSerial(char* tempbuf)
{

    //printf("blocking test : %s %d\n", buff->data(), buff->gettop());
    memcpy(tempbuf, buff->data(), buff->gettop() + 1);
    return 1;
}
