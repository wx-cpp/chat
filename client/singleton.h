#ifndef SINGLETON_H
#define SINGLETON_H

#include"globe.h"

//单例类封装，用作网络请求
template<typename T>
class singleton{
protected:
    singleton()=default;
    singleton(const singleton<T>&)=delete;
    singleton& operator=(const singleton<T>& st)=delete;
    static std::shared_ptr<T> instance;

public:
    static std::shared_ptr<T> get_instance(){
        static std::once_flag s_flag;
        std::call_once(s_flag,[&](){
            instance=std::shared_ptr<T>(new T);
        });

        return instance;
    }

    void print_address(){
        std::cout<<instance.get()<<std::endl;
    }

    ~singleton(){
        std::cout<<"this is singleton destruct"<<std::endl;
    }
};

//实例化
template<typename T>
std::shared_ptr<T> singleton<T>::instance=nullptr;

#endif // SINGLETON_H
