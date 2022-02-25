#ifndef APPLOCK_H
#define APPLOCK_H

namespace applock
{
    enum LockResult: int
    {
        kSuccess = 0x0,  // экземпляр приложения успешно установил блокировку.
        kExists  = 0x1,  // уже существует блокировка, занятая другим экземпляром приложения.
        kError   = 0x2   // при установке блокировки произошел сбой, не связанный с тем, что блокировка уже занята.
    };

    LockResult tryLockApplication(const char* lockName);
}

#endif // APPLOCK_H
