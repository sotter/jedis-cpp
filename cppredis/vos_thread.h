/******************************************************
 *   FileName: vos_thread.h
 *     Author: liubo  2012-7-5 
 *Description:
 *Description: (1)�����ṩ�̷߳���
 *Description: (2)��װwindows���̺߳����ṩ������
 *******************************************************/

#ifndef VOS_THREAD_H_
#define VOS_THREAD_H_

#ifdef __WINDOWS
#include <process.h>
typedef void* THREAD_T;
#else

#include <sys/time.h>
#include <time.h>
#include <pthread.h>
typedef pthread_t THREAD_T;
#endif

#include <vector>
#include "vos_event.h"
namespace vos
{
#define  MAX_THREAD 128

/********************************
 *  ����Linux �� Winodows���ķ�������
 *******************************/
extern THREAD_T make_thread(void *fun, void *arg);
extern int wait_thread(THREAD_T tid);
//static int exit_thread(THREAD_T tid);

class ThreadManage;
class Thread;
class RunThread
{
public:
    virtual ~RunThread(){};
    virtual int run_thread(int type) = 0;
};

class Thread
{
public:
	enum State{init, running, exited, stopped};

    Thread(): _id(0), _tid(0), _state(init), _exit(false){}
    ~Thread(){};

	int start();

	/*********************************
	 * �ڹ涨��ʱ���ڣ��߳����û��ִ���꣬ǿ�н����̡߳�
	 * ******************************/
    int stop(int timeout);

    //�ṩ��Ӧ�ò��ʹ�����õ�
    int isexit(){return _exit;}

private:
	void run()
	{
		_run_thread->run_thread(_type);
	}
    int exit();
    void set_exit(){_exit = true;}
#ifdef __WINDOWS
	static unsigned int WINAPI the_thread(void *param);
	
#else
	static void* the_thread(void *param);
#endif

public:
    vos::Event _stop_event;
    int _type;
    int _id;            //���б�ʶ��
	THREAD_T _tid;
	State _state;
	RunThread *_run_thread;
	bool _exit;
};

class ThreadPool
{
#define max_thread_num 64
public:
    /***
     * ע���̣߳�type_index��ʶ�̵߳����ͣ� �������͵��߳�ע����ٸ���
     */
	int regist(int type_index, RunThread *run,  int num = 1);
    int start();
    /*************
     * �ڷ��ͽ����ź�timeout��ʱ����,�̻߳�û�н����ǿ��ɱ��.
     **********************/
    int stop(int timeout = 0);
private:

	std::vector<Thread*> _vec_thread;
};

} /* namespace vos */
#endif /* VOS_THREAD_H_ */
