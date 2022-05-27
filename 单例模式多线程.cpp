// 单例模式多线程.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 单例模式共享数据问题分析、解决  引出std::call_once模板的用法
// 面临的问题：在主线程中创建单例模式是没问题的，但需要在其他线程中创建单例模式（可能不止一个线程）
// 要将GitInstance()函数互斥

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

std::mutex resource_mutex;//定义全局互斥量
std::once_flag flag;//定义全局标记 配合std::call_once使用

class MyCAS //单例类
{
public:
	/*
	* MyCAS* GitInstance()函数在多线程中可能出现的问题
	* 线程1在判断if之后认为m_instance为空，但此时失去CPU 认为现在没有创建m_instance
	* 线程2也判断if后后认为m_instance为空，创建m_instance
	* 线程1得到CPU后再次创建m_instance
	* 导致m_instance被创建两次
	*/
	//static MyCAS* GitInstance()
	//{
	//	if (m_instance == NULL)
	//	{
	//		m_instance = new MyCAS();
	//	}
	//	return m_instance;
	//}

	//解决方法1 使用互斥量可以解决，但是效率太低，
	//只有第一次创建m_instance时给线程加锁才有意义,但每次调用GitInstance()就会加锁
	//static MyCAS* GitInstance()
	//{
	//	std::unique_lock<std::mutex> mymutex(resource_mutex);
	//	if (m_instance == NULL)
	//	{
	//		m_instance = new MyCAS();
	//	}
	//	return m_instance;
	//}

	//解决方法2  解决上一个问题的效率问题
	//static MyCAS* GitInstance()
	//{
	//	if (m_instance == NULL)
	//	{
	//		std::unique_lock<std::mutex> mymutex(resource_mutex);//只会被锁一次
	//		if (m_instance == NULL)
	//		{
	//			m_instance = new MyCAS();
	//		}
	//	}
	//	return m_instance;
	//}

	//解决方法3  使用std::call_once，需要和标记配合使用
	static MyCAS* GitInstance()
	{
		//有一个线程调用CreateInstance()函数时，其他线程会阻塞等待
		//call_once调用的函数再程序运行期间只会被调用一次
		std::call_once(flag, CreateInstance); 
		return m_instance;
	}

	void show()
	{
		cout << "测试" << endl;
	}

private:
	static void CreateInstance()
	{
		std::chrono::milliseconds dura(20000);
		std::this_thread::sleep_for(dura); //模拟运行时长
		m_instance = new MyCAS();
	}

	MyCAS() {}
	static MyCAS* m_instance;
};

MyCAS * MyCAS::m_instance = NULL;

void func()
{
	MyCAS* p = MyCAS::GitInstance(); //可能出现问题
	p->show();
}

int main(void)
{
	std::thread run1(func);
	std::thread run2(func);
	run1.join();
	run2.join();

	return 0;
}

