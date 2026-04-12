#include<iostream>
#include<thread>
#include<mutex>
#include<queue>
using namespace std;

template<class Type>
class ThreadSafeQueue
{
    private:
        queue<Type> q;
        mutex mtx;
    public:

        void push(Type e)
        {
           {
            lock_guard<mutex> lock(this->mtx);
            q.push(e);
            cout<<"push success!"<<endl;
           }
        }

        void pop()
        {
            {
                lock_guard<mutex> lock(this->mtx);
                q.pop();
                cout<<"pop success!"<<endl;
            }
        }
};

queue<int> normal;            //普通容器
ThreadSafeQueue<int> safe;    //线程安全的容器

mutex add_mtx;
void NormalAdd(int a,int b)
{
    {
        lock_guard<mutex> lock(add_mtx);
        normal.pop();
        cout<<"pop success!"<<endl;
        int c=a+b;
        normal.push(c);
        cout<<"push success!"<<endl;
    }
}

void SafeAdd(int a,int b)
{
    safe.pop();
    int c=a+b;
    safe.push(c);
}

int main()
{
    return 0;
}