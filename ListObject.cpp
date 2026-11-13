#include "ListObject.hpp"
#include "ValueDescriptor.hpp"

//added error handling for lists

ListObject::ListObject(ListElementType initialType) : category{initialType} {}

ListObject::~ListObject() {
    for (ValueDescriptor *element : elements) {
        delete element;
    }
}

void ListObject::retain() {
    ++references;
}

// Decrements the reference count and returns true when it reaches zero.
bool ListObject::release() {
    if (references == 0) {
        throw std::runtime_error("ListObject::release called with zero references");
    }
    --references;
    return references == 0;
}

std::size_t ListObject::referenceCount() const {
    return references;
}

std::size_t ListObject::size() const {
    return elements.size();
}

ListElementType ListObject::elementType() const {
    return category;
}

// Returns a newly allocated descriptor owned by the caller.
ValueDescriptor* ListObject::cloneElementAt(std::size_t index) const {
    if (index >= elements.size()) {
        throw std::runtime_error("list index " + std::to_string(index) + " out of range.");
    }
    return elements[index]->clone();
}

// On success, each of these functions takes ownership of value.
void ListObject::setElement(std::size_t index, ValueDescriptor *value) {
    if (index >= elements.size()) {
        throw std::runtime_error("list index " + std::to_string(index) + " out of range.");
    }

    ListElementType elementType;
    switch (value->type()) {
        case ValueDescriptor::Type::integer:
            elementType = ListElementType::numeric;
            break;
        case ValueDescriptor::Type::doubleValue:
            elementType = ListElementType::numeric;
            break;
        case ValueDescriptor::Type::boolean:
            elementType = ListElementType::boolean;
            break;
        case ValueDescriptor::Type::string:
            elementType = ListElementType::string;
            break;
        case ValueDescriptor::Type::list:
            throw std::runtime_error("lists cannot contain lists");
    }

    

    if (category != ListElementType::unset && elementType != category) {
        throw std::runtime_error("cannot set element");
    }

    delete elements[index];
    elements[index] = value;

    if (category == ListElementType::unset) {
        category = elementType;
    }
}

//ListElementType category{ListElementType::unset};
void ListObject::append(ValueDescriptor *value) {
    ListElementType elementType;
    switch (value->type()) {
        case ValueDescriptor::Type::integer:
            elementType = ListElementType::numeric;
            break;
        case ValueDescriptor::Type::doubleValue:
            elementType = ListElementType::numeric;
            break;
        case ValueDescriptor::Type::boolean:
            elementType = ListElementType::boolean;
            break;
        case ValueDescriptor::Type::string:
            elementType = ListElementType::string;
            break;
        case ValueDescriptor::Type::list:
            throw std::runtime_error("lists cannot contain lists");
    }

    if (category != ListElementType::unset && elementType != category) {
        throw std::runtime_error("cannot append element");
    }

    elements.push_back(value);

    if (category == ListElementType::unset) {
        category = elementType;
    }
}

// Returns the removed descriptor and transfers ownership to the caller.
ValueDescriptor* ListObject::pop() {
    if (elements.empty()) {
        throw std::runtime_error("cannot pop from an empty list");
    }

    ValueDescriptor *element = elements.back();
    elements.pop_back();
    return element;
}