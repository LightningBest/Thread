// 条件变量.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//std::condition_variable  wait()  notify_one()
//条件变量在Cpp中是一个类，使用时实例化一个条件变量对象，要和互斥量配合使用
#include <iostream>
#include <list>
#include <thread>
#include <mutex>

using namespace std;


bool isEmpty(list<int> list)
{
    if (list.empty())
    {
        return false;
    }
    return true;
}


//实现一个简易网络游戏服务器模型
//使用两个线程，一个线程收集玩家数据，并写入队列中
//另一个线程取出队列中数据，执行对应动作 
class receive
{
public:
    void wirteQueue()//此线程用于接收玩家数据
    {
        std::chrono::milliseconds dura(2000);
        std::this_thread::sleep_for(dura);//为了让处理线程先执行

        for (int i = 0; i < 10000; i++)
        {
            std::unique_lock<std::mutex> mylock(my_mutex);
            msg.push_back(i);
            cout << "----" << endl;
            my_cond.notify_one();//唤醒阻塞线程(只能唤醒一个线程)
            //如果有多个线程使用wait，就要使用notify_all唤醒全部wait阻塞的线程
        }
        return;
    }
    void readQueue()//此线程用于处理玩家数据
    {
        while (1)
        {
            std::unique_lock<std::mutex> mylock(my_mutex);

            /*
            * wait的用法：
            * 如第二个参数返回true，那么wait直接返回，线程继续执行
            * 如第二个参数返回false，那么wait会解锁互斥量，并将线程阻塞到本行
            *       直到其他线程调用notify_one()成员函数为止
            * 如何wait没有第二个参数，和第二个参数返回false的情况相同（不安全，可能存在虚假唤醒）
            * 
            * 当某线程调用notify_one()成员函数后
            * wait会不断尝试重新获取互斥量，先获取互斥量
            *   如wait有第二个参数，还会进行条件判断，情况同上
            *   如wait没有第二个参数，wait直接返回，线程继续执行
            */
            my_cond.wait(mylock, [this] {
                if (!msg.empty())
                {
                    return true;
                }
                return false;
            });
            //my_cond.wait(mylock, isEmpty(msg));

            //获取数据
            int command = msg.front();
            msg.pop_front();
            cout << "||||" << command << endl;
            mylock.unlock();
            //对数据进行处理，可能会消耗很长时间，可以提前释放互斥量
            //这样会有一个问题，如下一次接收线程执行notify_one()唤醒处理线程时，
            //处理线程还没有处理完数据，还没有调用wait，那么这次notify_one()就没有效果
        }
    }

private:
    list<int> msg; //用于存储数据
    mutex my_mutex; //互斥量
    std::condition_variable my_cond;//条件变量
};

int main(void)
{
    receive obj;
    std::thread outObj(&receive::readQueue, &obj);
    std::thread inObj(&receive::wirteQueue, &obj);
    std::thread inObj2(&receive::wirteQueue, &obj); //如果有多个线程使用
                                                    //wait，就要使用notify_all
    outObj.join();
    inObj.join();
    inObj2.join();

    cout << "主线程开始执行" << endl;

    return 0;
}


