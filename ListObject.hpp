#ifndef LISTOBJECT_HPP
#define LISTOBJECT_HPP

#include "ValueDescriptor.hpp"
#include <cstddef>
#include <vector>

enum class ListElementType {
    unset,
    numeric,
    boolean,
    string
};

class ListObject {
public:
    explicit ListObject(
        ListElementType initialType = ListElementType::unset);
    ~ListObject();

    ListObject(const ListObject &) = delete;
    ListObject &operator=(const ListObject &) = delete;

    void retain();

    // Decrements the reference count and returns true when it reaches zero.
    bool release();

    std::size_t referenceCount() const;
    std::size_t size() const;
    ListElementType elementType() const;

    // Returns a newly allocated descriptor owned by the caller.
    ValueDescriptor* cloneElementAt(std::size_t index) const;

    // On success, each of these functions takes ownership of value.
    void setElement(std::size_t index, ValueDescriptor *value);
    void append(ValueDescriptor* value);

    // Returns the removed descriptor and transfers ownership to the caller.
    ValueDescriptor* pop();

private:
    std::size_t references{0};
    ListElementType category{ListElementType::unset};
    std::vector<ValueDescriptor *> elements;
};



#endif //LISTOBJECT_HPP