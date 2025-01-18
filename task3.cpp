#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

mutex muterOut;

struct Philosopher
{
    unsigned int id;
    mutex& leftFork;
    mutex& rightFork;

    Philosopher(unsigned int num, mutex& lF, mutex& rF) : id(num), leftFork(lF), rightFork(rF) {}

    void doing() // у каждого философа есть два состояния: есть и думать, они чередуются
    {
        while (true)
        {
            think();
            eat();
        }
    }

    void think()
    {
        {
            lock_guard<mutex> lockOut(muterOut);
            cout << "Philosopher " << id << " is thinking" << endl;
        }
        this_thread::sleep_for(chrono::seconds(2)); // создаем видимость работы
    }

    void eat()
    {
        lock(leftFork, rightFork); // чтобы другой поток не успел забрать одну из вилок
        lock_guard<mutex> lockLeft(leftFork, adopt_lock); // adopt_lock говорит о том, что мьютекс уже заблокирован
        lock_guard<mutex> lockRight(rightFork, adopt_lock);

        {
            lock_guard<mutex> lockOut(muterOut);
            cout << "Philosopher " << id << " is eating" << endl;
        }
        this_thread::sleep_for(chrono::seconds(1)); // создаем видимость работы
    }

};

int main() 
{
    vector<mutex> forks(5);
    vector<thread> philosophers;

    for (unsigned int i = 0; i < 5; ++i)
    {
        // добавляем поток в вектор, запуская метод
        philosophers.emplace_back(&Philosopher::doing, Philosopher(i + 1, forks[i], forks[(i + 1) % 5]));
    }

    for (auto& i : philosophers)
    {
        i.join();
    }
}