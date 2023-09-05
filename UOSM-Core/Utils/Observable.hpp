//
// Created by Jeremy Cote on 2023-08-28.
//

#ifndef UOSM_CORE_OBSERVABLE_HPP
#define UOSM_CORE_OBSERVABLE_HPP

#include "Identifiable.hpp"
#include <functional>

/** @ingroup core-ui-utils
 *  A class that represents a token that can be used to cancel an observation.
 *  It holds a function that can be called to cancel the observation.
 */
class ObserverToken : public Identifiable {
public:
    /** The function that can be called to cancel the observation. */
    std::function<void()> cancel;

    /** Constructs an observer token with a given cancel function.
     *  @param cancel The function that can be called to cancel the observation.
     */
    explicit ObserverToken(std::function<void()> cancel) : cancel(std::move(cancel)) {}
};

template<typename T>
class Observable {
public:
    virtual T& get() const = 0;
    virtual ObserverToken addListener(std::function<void(const T&)> callback) = 0;
};

#endif //UOSM_CORE_OBSERVABLE_HPP
