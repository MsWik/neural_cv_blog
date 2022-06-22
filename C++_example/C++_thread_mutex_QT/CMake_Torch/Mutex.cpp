#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>



#include <thread>
#include <mutex>
#include <iostream>

#include <mutex>
#include <condition_variable>
#include <iostream>




class Restaurant {
    enum class Status { idle, newOrder, ready };
    Status orderStatus = Status::idle;
    std::mutex order;
    std::condition_variable orderBell;
    int counter = 10;


public:

    void chef() {
        while (counter>1){
 

            std::unique_lock<std::mutex> ul(order);
            orderBell.wait(ul, [=]() { return orderStatus == Status::newOrder; });
            //������������� ���� �� ������
            std::cout << "chef make Order!" << counter << "\n";
            orderStatus = Status::ready;
            orderBell.notify_one();
            counter--;
            order.lock();
        }
    }

    void waiter() {

        while (counter>0) {

        

            {
                std::lock_guard<std::mutex> lg(order);
                orderStatus = Status::newOrder;
                orderBell.notify_one();
            } // lg ��� ������� ��������� = order.unlock()

            std::unique_lock<std::mutex> ul(order);
            orderBell.wait(ul, [=]() { return orderStatus == Status::ready; });
            orderStatus = Status::idle;
            ul.unlock();
            //�������� �����
            std::cout << "waiter go castpmer! " << counter  <<"\n";
            counter--;
        
            
        }

    }

};


int main() {
    Restaurant restaurant;
    std::thread chef(&Restaurant::chef, std::ref(restaurant));
    std::thread waiter(&Restaurant::waiter, std::ref(restaurant));
    chef.join();
    waiter.join();
    return 0;
}

