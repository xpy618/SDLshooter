#ifndef ObJECT_POOL_H
#define OBJECT_POOL_H

#include "Object.h"
#include <vector>

//模板类,只能在.h文件中

template <typename T>  //声明一个通用类，T是占位符
class ObjectPool{
private:
    size_t poolSize;  //size_t,最适合存储内存池大小（非负 + 内存字节数 + 跨平台兼容 + 标准库配套）
    std::vector<T> pool;
    T* firstAvailable = nullptr;

public:
    // 构造函数（决定池子大小，并创建连续对象）
    ObjectPool(T Prototype, size_t pool_size = 20):poolSize(pool_size){     //只有构造函数能初始化列表，直接给成员变量赋值，可有多个
        pool.resize(poolSize, Prototype);                                   //预留空间+拷贝
        firstAvailable = &pool[0];                                          //指针只能接受地址，不能接受对象本身
        for (size_t i = 0; i < poolSize - 1; i++){
            pool[i].next = &pool[i + 1];
        }
        pool[poolSize - 1].next = nullptr;
    }

	// 获取（指针指向可用对象，然后链表删除节点）
    T* create(){
        if (firstAvailable == nullptr){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No more objects in pool");
            return nullptr;
        }
        T* result = firstAvailable;
        firstAvailable = firstAvailable->next;
        return result;
    }

    // 放回（链表插入节点）
    void release(T* object){
        object->next = firstAvailable;  //先让归还对象指向第一个对象
        firstAvailable = object;  //归还对象赋为第一个对象
    }
};

#endif