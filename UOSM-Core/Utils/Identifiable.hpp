//
// Created by Jeremy Cote on 2023-07-29.
//

#ifndef UOSM_CORE_IDENTIFIABLE_HPP
#define UOSM_CORE_IDENTIFIABLE_HPP

#include <cstdint>

/** @ingroup core-ui-utils
 *  A class that represents an identifiable object that has a unique ID.
 *  The ID is assigned automatically when the object is constructed, and can be compared with other objects.
 */
class Identifiable {
private:
    /** The next available ID for the objects. */
    static uint32_t nextId;
    /** The ID of this object. */
    uint32_t id;
public:
    /** Constructs an identifiable object with a unique ID. */
    Identifiable() : id(++nextId) {}

    /** Compares this object with another object by their IDs.
     *  @param other The other object to compare with.
     *  @return True if the IDs are equal, false otherwise.
     */
    virtual bool operator==(const Identifiable& other) {
        return id == other.id;
    }

    /** Returns the ID of this object.
     *  @return The ID of this object.
     */
    uint32_t getId() const {
        return id;
    }

    /** Returns the next available ID for the objects.
     *  @return The next available ID for the objects.
     */
    static uint32_t getNextId() {
        return nextId;
    }
};

#endif //UOSM_CORE_IDENTIFIABLE_HPP