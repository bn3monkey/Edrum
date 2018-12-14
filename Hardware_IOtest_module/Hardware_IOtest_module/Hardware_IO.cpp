#pragma warning(disable : 4996)
#include "Hardware_IO.hpp"
#include <conio.h>

Hardware_IO* Hardware_IO::instance = nullptr;

void Hardware_IO::in()
{
	while (instance->in_flag)
	{
		//1. �ܺ� ���κ��� �Է��� �޾ƿ´�. 
		char buffer = 0;
		if (kbhit())
			buffer = getch();

		if (!buffer) continue;

		//2. �޾ƿ� �Է��� note�� ����� Queue�� �־��ش�.
		NoteData note;
		note.time = 0;
		note.power = 0;
		switch (buffer)
		{
		case 'w': note.power = np_up; break;
		case 's': note.power = np_down; break;
		case 'a': note.power = np_left; break;
		case 'd': note.power = np_right; break;
		case ' ': note.power = np_ok; break;
		case 'q': note.power = np_none; break;
		default: note.power = np_idle; break;
			
		}

		// note�� input_queue�� �ִ´�.
		instance->queue_lock.lock();
		instance->input_queue.push(note);
		instance->queue_lock.unlock();

	}
}

void Hardware_IO::out()
{
	while (instance->out_flag)
	{
		NoteData note;

		instance->queue_lock.lock();
		if (!instance->input_queue.empty())
		{
			// note�� input_queue�� ���´�.
			note = instance->input_queue.front();
			instance->input_queue.pop();
			instance->queue_lock.unlock();

			//callback�� �����Ѵ�.
			instance->callback_lock.lock();
			instance->callbacks[note.power]();
			instance->callback_lock.unlock();
		}
		else
			instance->queue_lock.unlock();
	}
}

static void empty_function()
{
}

// Hardware_IO�� �ʱ�ȭ�Ѵ�. input�� ���� �� �ֵ��� thread�� ű���ش�.
bool Hardware_IO::initialize()
{
	// �̹� �ʱ�ȭ�Ǿ�����, �ʱ�ȭ�� �����.
	if (instance != nullptr)
	{
		std::cout << "Hardware_IO already initialized" << std::endl;
		return false;
	}

	// SingleTone ��ü�� �ʱ�ȭ�Ѵ�.
	instance = new Hardware_IO();
	
	// thread�� �����Ѵ�.
	instance->in_flag = true;
	instance->out_flag = true;
	instance->in_thread = new std::thread(in);
	instance->out_thread = new std::thread(out);

	// ������ ���� ����, �Լ� ���̺��� empty function���� �ʱ�ȭ�Ѵ�.
	for (int i = 0; i < np_num; i++)
		instance->callbacks[i] = std::function<void()>(empty_function);

	return true;
}
// Hardware_IO�� �����Ѵ�. thread�� join���ش�.
void Hardware_IO::destroy()
{
	instance->in_flag = false;
	instance->out_flag = false;
	instance->in_thread->join();
	instance->out_thread->join();

	//queue�� ����.
	while (!instance->input_queue.empty())
		instance->input_queue.pop();

	// SingleTone ��ü�� �Ҵ������Ѵ�.
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}
// callback�� ����Ѵ�.
bool Hardware_IO::registCallback(std::function<void()> callback, NoteProtocol np)
{
	// SingleTone���� �������� �ʾ����� ����Ѵ�.
	if (Hardware_IO::getInstance() == nullptr)
		return false;

	instance->callback_lock.lock();
	instance->callbacks[(int)np] = callback;
	instance->callback_lock.unlock();

	return true;
}