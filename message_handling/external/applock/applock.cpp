#include "applock.h"

#ifdef _WIN32
#include <windows.h>

applock::LockResult applock::tryLockApplication(const char* lockName)
{
    auto mutex = CreateMutex(NULL, TRUE, lockName);
    if (mutex == NULL)
        return kError;  // не удалось получить хэндл мьютекса - аномальная ситуация, какая-то системная ошибка.

    auto lastError = GetLastError();
    switch (lastError)  //  DWORD - целое, поэтому можно использовать switch. LastError обновляет свое значение после каждого вызова любой функции WinApi!
    {
        case ERROR_SUCCESS:    // мьютекс захвачен без ошибок.
            return kSuccess;

        case ERROR_ALREADY_EXISTS:  // данный мьютекс уже захвачен другим потоком.
            CloseHandle(mutex);
            return kExists;

        default:  // какая-то другая ошибка.
            CloseHandle(mutex);
            return kError;
    }
}
#endif // _WIN32


#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>

#include <string>

applock::LockResult applock::tryLockApplication(const char* lockName)
{
    const std::string lockFileName("/tmp/" + std::string(lockName));

    const int lockFile = open(lockFileName.c_str(), O_CREAT | O_RDONLY, 0444);  // создаём и(или) открываем файл-блокировку только для чтения.
    if (lockFile == -1)
        return kError;                                                  // не смогли окрыть файл - ошибка.

    const int lock = flock(lockFile, LOCK_EX | LOCK_NB);
    if (lock == -1)                                                     // не смогли установить блокировку на файл - ошибка.
        return (errno == EWOULDBLOCK) ? kExists : kError;               // с чем связана данная ошибка?

    return kSuccess;
}
#endif // __linux__
