#pragma execution_character_set("utf-8")

#include <functional>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <windows.h>
using namespace std;

class A
{
public:
    A() = default;
    ~A()
    {
        cout << __PRETTY_FUNCTION__ << endl;
    }

    void function1(){}

    virtual void v1funciton(){cout << __PRETTY_FUNCTION__ << endl;}
    virtual void v2funciton(){cout << __PRETTY_FUNCTION__ << endl;}

public:
    int i = 0;
};

class B : public A
{
public:
    B() = default;
    ~B()
    {
        cout << __PRETTY_FUNCTION__ << endl;
    }
};

//删除父类指针，先delete 子类.不借助析构函数是虚函数的方法实现
// MySharedV2<A> (new B);
template<typename T>
class MySharedV2
{
public:
    template<typename T2>
    MySharedV2(T2* p)
    {
        data_ = p;
        deleter_ = [p](){delete p;};
    }

    ~MySharedV2()
    {
        deleter_();
    }

    T* operator->()
    {
        return data_;
    }
private:
    std::function<void()> deleter_;
    T* data_ = nullptr;
};

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    A a;
    union{
        void *pv;
        void(A::*func)();
    } u;
    u.func = &A::function1;
    cout << u.pv << endl;

    cout << "--------------------------------" << endl;

    int* vPtr = (int*)&a;                           //虚表指针
    int vPtrAddress = *vPtr;                        //虚表地址
    //int vFunc1Address = *(int*)vPtrAddress;
    int vFunc1Address = *(int*)(*(int*)&a);         //第一个虚函数地址
    //int vFunc2Address = *(int*)(vPtrAddress + 1);
    int vFunc2Address = *((int*)(*(int*)&a) + 1);   //第二个虚函数地址

    cout << "vPtr=" << vPtr << endl;
    cout << "&a.i=" << &a.i << endl;
    cout << "vPtrAddress=" << vPtrAddress << endl;
    cout << "vFunc1Address=" << (int*)(*(int*)&a) << endl;      //vFunc1Address == (int*)(*(int*)&a)
    cout << "vFunc2Address=" << ((int*)(*(int*)&a) + 1) << endl;//vFunc2Address == *((int*)(*(int*)&a) + 1)



    cout << "--------------------------------" << endl;

    cout << "-------------start call func-------------------" << endl;
    //(int*)&a 是虚表指针。 *(int*)&a 是虚表地址。 将虚表地址转成(*int)表示取从虚表地址开始的前四个字节；
    //因为虚函数表其实就一个数组，数组存的是虚函数的地址，所以 (*int)*(int*)&a 表示数组的第一个元素，对其解引用
    //就能获取到第一个虚函数的地址 *(*int)*(int*)&a
    ((void(*)(void))(*(int*)*(int*)&a))();
    ((void(*)(void))vFunc2Address)();

    cout << "-------------end call func-------------------" << endl;

    //*(int*)&a 是虚函数数组的首地址，数组存的是int*，所以 (int*)*(int*)&a 表示一个int*的数组
    int* vFuncArray = (int*)*(int*)&a;  //虚函数表数组
    int* vFunc1Arress_ = vFuncArray;    //第一个数组元素
    int* vFunc2Arress_ = vFuncArray + 1;//第二个数组元素

    ((void(*)(void))(*vFunc1Arress_))();
    ((void(*)(void))(*vFunc2Arress_))();

    return 0;
}
