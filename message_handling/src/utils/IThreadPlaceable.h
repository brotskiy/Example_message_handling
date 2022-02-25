#ifndef IWORKER_H
#define IWORKER_H

#include <QObject>
#include <QThread>

#include <functional>
#include <type_traits>

#include <QDebug>

namespace utils
{
namespace heavy
{

class IThreadPlaceable
{
public:
    static constexpr QObject *const NO_PARENT = nullptr;    // константа, означающая, что у экземпляра дочернего класса-работника нет родителя!

    virtual ~IThreadPlaceable() = 0;    // абстрактный класс!
    virtual void onThreadStop();
};

template<class Derived_t>
Derived_t* create(QObject* parent, std::function<Derived_t*()> factory);

template<class Derived_t>
void destroy(Derived_t* me);

}
}



inline utils::heavy::IThreadPlaceable::~IThreadPlaceable()    // хоть и абстрактный класс, но деструктор всё равно должен быть определен.
{
}

inline void utils::heavy::IThreadPlaceable::onThreadStop()
{
    qDebug() << "IThreadPlaceable is stopping!";
}

template<class Derived_t>
Derived_t* utils::heavy::create(QObject* workerThreadParent, std::function<Derived_t*()> createWorker)
{
    static_assert(std::is_base_of<QObject, Derived_t>::value,          "only QObject subclass can be placed inside a thread!");
    static_assert(std::is_base_of<IThreadPlaceable, Derived_t>::value, "only IThreadPlaceable subclass can be created!");

    Derived_t* worker = createWorker();

    if (worker)
    {
        QThread* workerThread = new QThread(workerThreadParent);
        worker->moveToThread(workerThread);

        auto onStop =[worker]() -> void
        {
            worker->onThreadStop();
        };
        QObject::connect(workerThread, &QThread::finished, static_cast<QObject*>(worker), onStop);


        workerThread->start();
    }

    return worker;
}

template<class Derived_t>
void utils::heavy::destroy(Derived_t* me)
{
    static_assert(std::is_base_of<QObject, Derived_t>::value,          "only QObject subclass can be placed inside a thread!");
    static_assert(std::is_base_of<IThreadPlaceable, Derived_t>::value, "only IThreadPlaceable subclass can be destroyed!");

    if (me)
    {
        QThread* myThread = me->thread();
        myThread->quit();
        myThread->wait();

        delete me;

        if (myThread->parent() == nullptr)    // если потоку не был передан родитель, то мы сами отвечаем за его удаление.
        {
            delete myThread;

            qDebug() << "thread is destroyed manually!";
        }
    }
}

#endif // IWORKER_H
