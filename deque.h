#ifndef __G_QUEUE__
#define __G_QUEUE__

#include <deque>
using namespace std;


template <class A>
class G_Queue
{
	public:		
		G_Queue(int uQueSize = 0);	
		~G_Queue();
		bool push(const A &item);    //为生产者线程调用
		bool pop(A &item);           //为消费者线程调用
		int get_size();
		int quit_wait();
	private:		
		deque<A> m_que;    ///STL 
		pthread_mutex_t mutex;
		pthread_cond_t cond;	
		int m_uQueSize;   
		int m_bQuitFlag;
};


template <class A>
G_Queue<A>::G_Queue(int uQueSize)
{
	m_uQueSize =  uQueSize;
	m_bQuitFlag = false;
	pthread_mutex_init(&mutex,NULL);	
	pthread_cond_init(&cond,NULL);	//被用来初始化一个条件变量
}

template <class A>
G_Queue<A>::~G_Queue()
{
	pthread_cond_destroy(&cond);	
	pthread_mutex_destroy(&mutex);
	m_que.clear();
}

template <class A>
int G_Queue<A>::get_size()
{
	pthread_mutex_lock(&mutex);	
	int size = m_que.size();
	pthread_mutex_unlock(&mutex);
	return size;
}


template <class A>
bool G_Queue<A>::push(const A &item)
{
	pthread_mutex_lock(&mutex);	
	if(m_uQueSize && (m_que.size() == m_uQueSize))  
	{	
//		printf("FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
		pthread_mutex_unlock(&mutex);
		return false;
	}	
	m_que.push_back(item);	

	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);    //唤醒一个线程
	return true;
}

template <class A>
bool G_Queue<A>::pop(A &item)
{	
	pthread_mutex_lock(&mutex);	
	while (m_que.empty()) 
	{		
		if(m_bQuitFlag){
			//printf("~~~queue qiut wait!!!!");
//			printf("FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
			pthread_mutex_unlock(&mutex);
			return false; 
		}		
		pthread_cond_wait(&cond,&mutex);  //线程睡眠，等待唤醒
	}		
	if(m_que.size() == 0)
	{	
//		Log(ERROR,"FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
		pthread_mutex_unlock(&mutex);
		return false;
	}
	item = m_que.front();
	m_que.pop_front();	
	pthread_mutex_unlock(&mutex);
	return true;
}
//强制线程从POP的阻塞中退出
template <class A>
int  G_Queue<A>::quit_wait()
{

	m_bQuitFlag = true;
	pthread_cond_signal(&cond);    //唤醒一个线程
	return 0;
}


#endif
