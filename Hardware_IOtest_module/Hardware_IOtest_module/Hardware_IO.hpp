#ifndef __HARDWARE_IO__
#define __HARDWARE_IO__

#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <iostream>
#include "NoteData.hpp"

//SingleTone class�̴�.
class Hardware_IO
{
	//Callback function�� �����ϰ� �ִ� �Լ� ������ �迭
	std::function<void()> callbacks[np_num];
	std::mutex callback_lock;

	// hardware���� �޾ƿ��� input���� ����
	std::queue<NoteData> input_queue;
	// input_queue���� push, pop�� �� ���ڼ��� �������ִ� ����ȭ ��ü
	std::mutex queue_lock;
	
	// thread�� �޴´�.
	std::thread* in_thread;
	std::thread* out_thread;

	static Hardware_IO* instance;

	// queue�� �Է��� �޴� �޼ҵ�
	static void in();
	bool in_flag;

	// queue�� callback�� �����ϴ� �޼ҵ�
	static void out();
	bool out_flag;

public:
	Hardware_IO() { in_flag = false; out_flag = false; }
	~Hardware_IO() {}
	
	// Singletone���� ����� instance�� �����´�.
	// Hardware IO�� !!!�ݵ��!!! �ʱ�ȭ �� ����Ѵ�.
	static Hardware_IO* getInstance()
	{
		return instance;
	}

	// Hardware_IO�� �ʱ�ȭ�Ѵ�. input�� ���� �� �ֵ��� thread�� ű���ش�.
	static bool initialize();
	// Hardware_IO�� �����Ѵ�. thread�� join���ش�.
	static void destroy();
	
	// callback�� ����Ѵ�.
	static bool registCallback(std::function<void()> callback, NoteProtocol np);
};

#endif