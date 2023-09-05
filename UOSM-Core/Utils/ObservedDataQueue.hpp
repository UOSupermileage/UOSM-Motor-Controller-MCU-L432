//
// Created by Jeremy Cote on 2023-08-21.
//

#ifdef UOSM_OBSERVABLES

#ifndef UOSM_CORE_OBSERVEDDATAQUEUE_HPP
#define UOSM_CORE_OBSERVEDDATAQUEUE_HPP

#include "DataQueue.hpp"
#include "ObservedObject.hpp"

/**
 *
 */
template<typename T>
class ObservedDataQueue: public ObservedObject<DataQueue<T>> {
private:
    DataQueue<T> queue;
public:
    explicit ObservedDataQueue(uint8_t size): queue(size), ObservedObject<DataQueue<T>>(&queue, false) {}

    /**
     * Copy a value into the bar data collection. IMPORTANT: This creates a copy of the passed value.
     * @param value to copy into the collection
     */
    void add(T value) {
        queue.add(value);
        this->publish();
    }

    /**
     * Update the last element of the collection. Useful if the latest value is still changing.
     * If the collection is empty, this acts as a call to add
     * @param value
     */
    void update(T value) {
        queue.update(value);
        this->publish();
    }

    [[nodiscard]] uint8_t getSize() const { return queue.getSize(); }

    /**
     * @return the number of values stored in the collection.
     */
    [[nodiscard]] uint8_t getNumberOfElements() const { return queue.getNumberOfElements(); }

    /**
     * Return the underlying data of the collection.
     * Use to set the data source of a bar chart.
     * @return a pointer to the underlying data source
     */
    [[nodiscard]] T* getValues() const { return queue.getValues(); }

    /**
     * @return a pointer to the newest value added to the collection.
     */
    [[nodiscard]] T& getLatestValue() const noexcept(false) { return queue.getLatestValue(); }

    /**
     * Add a listener that only receives the latest value in the queue
     * @param callback
     * @return
     */
    ObserverToken addListenerForLatest(std::function<void(const T&)> callback) {
        return this->addListener([this, callback](const DataQueue<T>& q) {
            callback(queue.getLatestValue());
        });
    }
};

#endif //UOSM_SENSOR_OBSERVEDDATAQUEUE_HPP
#endif