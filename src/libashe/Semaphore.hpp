/*
 * Semaphore.h
 *
 * C++11 Semaphore by Ashe
 * On UNIX, link with lpthread
 *
 * @Maintained
 *  Under development since: 2015 Q1
 * @Author
 *  Ashe David Sterkhus
 *  Blame to: ashe.goulding+blame@gmail.com
 * @COLOPHON
 *  This file is part of libashe, Ashe's C++11/98 utility stuff
 */
#ifndef LASHE_SEMAPHORE_HPP_
#define LASHE_SEMAPHORE_HPP_
#include "Exception.hpp"

#include <condition_variable>
#include <mutex>


namespace ashe {

LASHE_DECL_EXCEPTION(SemaphoreException, Exception);

class LASHE_DECL_EXT Semaphore : public Class {
    /* Semaphore implementation, using C++11 features
     *
     * @NOTE
     *  - 'ArithType' could be any type that has following arithmetic
     * operations: +=, -=, =, <=
     *  - None of the feature of this class is thread-safe.
     */
public:
    typedef Class motherClass;
    typedef Semaphore thisClass;

    class LASHE_DECL_EXT Ticket : public Class {
        /* C++11 RAII compliant
         *
         * @NOTE
         *  - Enters the section on construction, leaves it on destruction
         */
    public:
        typedef Class motherClass;
        typedef Ticket thisClass;

    protected:
        Semaphore &__sem;

    public:
        Ticket(Semaphore &sem) LASHE_NOEXCEPT;
        Ticket(const thisClass &) = delete;
        Ticket(thisClass &&) = delete;
        ~Ticket() LASHE_NOEXCEPT;

        thisClass &operator=(const thisClass &src) = delete;
        thisClass &operator=(thisClass &&) = delete;

        const char *className() const LASHE_NOEXCEPT override;
    };

protected:
    std::mutex __mtx;
    std::condition_variable __cv;
    uintptr_t __traffic, // Increasing value
        __concurrency;   // Limit value

    thisClass &__enter() LASHE_NOEXCEPT;
    thisClass &__leave() LASHE_NOEXCEPT;

public:
    Semaphore(const uintptr_t concurrency);
    Semaphore(const thisClass &) = delete;
    Semaphore(thisClass &&) = delete;
    ~Semaphore() LASHE_NOEXCEPT;

    const char *className() const LASHE_NOEXCEPT override;

    thisClass &operator=(const thisClass &src) = delete;
    thisClass &operator=(thisClass &&) = delete;

    uintptr_t concurrency() const LASHE_NOEXCEPT;
};

} /* namespace ashe */

#endif
