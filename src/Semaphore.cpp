#include "libashe/Semaphore.hpp"


namespace ashe {

LASHE_IMPL_EXCEPTION(SemaphoreException, Exception);

/********************
 * Semaphore
 */
Semaphore &Semaphore::__enter() LASHE_NOEXCEPT
{
    std::unique_lock<std::mutex> l(this->__mtx);

    if (this->__concurrency <= this->__traffic) {
        this->__cv.wait(l);
    }
    this->__traffic += 1;

    return *this;
}

Semaphore &Semaphore::__leave() LASHE_NOEXCEPT
{
    std::unique_lock<std::mutex> l(this->__mtx);

    this->__traffic -= 1;
    this->__cv.notify_one();

    return *this;
}

Semaphore::Semaphore(const uintptr_t concurrency)
    : __traffic(0), __concurrency(concurrency)
{
    if (concurrency == 0) {
        throw SemaphoreException("Concurrency cannot be zero.");
    }
}

Semaphore::~Semaphore() LASHE_NOEXCEPT {}

const char *Semaphore::className() const LASHE_NOEXCEPT { return "Semaphore"; }

uintptr_t Semaphore::concurrency() const LASHE_NOEXCEPT
{
    return this->__concurrency;
}

/********************
 * Semaphore::Ticket
 */
Semaphore::Ticket::Ticket(Semaphore &sem) LASHE_NOEXCEPT : __sem(sem)
{
    this->__sem.__enter();
}

Semaphore::Ticket::~Ticket() LASHE_NOEXCEPT { this->__sem.__leave(); }

const char *Semaphore::Ticket::className() const LASHE_NOEXCEPT
{
    return "Semaphore::Ticket";
}

} // namespace ashe
