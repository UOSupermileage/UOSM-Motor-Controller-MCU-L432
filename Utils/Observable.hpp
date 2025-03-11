//
// Created by Jeremy Cote on 2023-08-28.
//

#ifndef UOSM_CORE_OBSERVABLE_HPP
#define UOSM_CORE_OBSERVABLE_HPP

#include "Identifiable.hpp"
#include <functional>

#include <algorithm>

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

/** @ingroup core-ui-utils
 *  A class that represents a listener that can be notified of changes in an observed object.
 *  It holds a function that can be called with the new value of the observed object.
 */
template<typename T>
class Listener : public Identifiable {
public:
    /** The function that can be called with the new value of the observed object. */
    std::function<void(const T&)> onChange;

    /** Constructs a listener with a given onChange function.
     *  @param onChange The function that can be called with the new value of the observed object.
     */
    explicit Listener(std::function<void(const T&)> onChange) {
        this->onChange = onChange;
    }
};

template<typename T>
class Observable {
private:
    /**
     * @brief The list of registered listeners
     */
    std::vector<Listener<T>> listeners;
public:
    /**
     * @brief Add a listener and return a token with a pointer to this object and a cancel lambda.
     *
     * The listener is a function that takes the current value of the object as a parameter and performs some action.
     * The token is an object that allows canceling the listener by calling its cancel() method.
     *
     * @param callback The listener function to be registered.
     * @return ObserverToken The token that allows canceling the listener.
     */
    ObserverToken addListener(std::function<void(const T&)> callback) {
        listeners.emplace_back(Listener<T>(callback));

        uint32_t listenerId = Listener<T>::getNextId();

        std::function<void()> cancel = {[this, listenerId]() {
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [listenerId](Listener<T>& listener) {
                return listener.getId() == listenerId;
            }), listeners.end());
        }};

        return ObserverToken(cancel);
    }

    void notify(T& value) {
        for (const auto& listener: listeners) {
            listener.onChange(value);
        }
    }
};

#endif //UOSM_CORE_OBSERVABLE_HPP
