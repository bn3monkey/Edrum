#ifndef __NOTE__
#define __NOTE__

#include <arduino.h>

struct note
{
    int drum;
    int power;
    unsigned long time;
};

class note_queue
{
    int led_number;
    note now;
    note next;
    
    public:
    note_queue(int _led_number = 0) : led_number(_led_number)
    {
        next.drum = now.drum = 0x100 * (led_number + 1);
        next.power = now.power = -1;
        next.time = 6000;
        now.time = 3000; //for debugging
        
    }
    ~note_queue()
    {

    }

    // play start 시 note_queue에 있는 정보를 두 개 받는다.
    //trial은 라즈베리파이와의 접속시도다.
    bool download(int trial);
    // playing 시 note_queue에 있는 정보를 하나 받는다.
    // trial은 라즈베리파이와의 접속시도다.
    bool refresh(int trial);

    //해당 LED의 노트를 전부 출력했으면 true, 아니면 false
    bool finish();

    //제대로 받아왔는지 출력.
    bool download_debug();
    bool refresh_debug();

    // 현재 시간과 note에 저장된 시간을 비교하여 얼마나 싱크가 맞는지 결과값을
    // 리턴한다. 
    int note_sync(unsigned long elapsed);

};

#endif
