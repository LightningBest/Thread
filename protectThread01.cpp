#include <iostream>   //分支文件修改
#include <thread>
#include <list>
#include <windows.h>
#include <mutex>

using namespace std;

//实现一个简易网络游戏服务器模型
//使用两个线程，一个线程收集玩家数据，并写入队列中
//另一个线程取出队列中数据，执行对应动作 


class receive
{
public:
    void wirteQueue()//此线程用于接收玩家数据
    {
        for (int i = 0; i < 10000; i++)
        {   
            my_mutex.lock();//对互斥量进行加锁
            msg.push_back(i);
            cout << "----" << endl;
            my_mutex.unlock();//对互斥量进行解锁
        }
        return;
    }
    void readQueue()//此线程用于处理玩家数据
    {
        for (int i = 0; i < 10000; i++)
        {

            if (msg.empty())
            {
                cout << "msg is empty" << endl;
            }
            else
            {
                {
                    unique_lock<mutex> myLock_unique(my_mutex);
                    std::mutex* ptx = myLock_unique.release();//解除绑定
                    int num = msg.front();
                    msg.pop_front();
                    cout << num << endl;
                    ptx->unlock();//解除绑定后要自己解锁

                    //不需要调用unlock，出{}时可提前结束lock_guard，调用析构函数释放锁，提升程序效率
                }
                //处理数据
            }

        }
    }

private:
    list<int> msg; //用于存储数据
    mutex my_mutex;
};


int main(void)
{
    receive obj;
    std::thread outObj(&receive::readQueue,&obj);
    std::thread inObj(&receive::wirteQueue, &obj);

    outObj.join(); //join阻塞等待线程退出
    inObj.join();

    cout << "主线程开始执行" << endl;
    return 0;
}

