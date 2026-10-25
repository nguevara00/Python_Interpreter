#ifndef VALUE_DESCRIPTOR_HPP
#define VALUE_DESCRIPTOR_HPP

#include <cstdint>
#include <string>

//The descriptors do not provide mutators. Once created, a descriptor continues to represent the same value. An assignment changes a variable by replacing the descriptor stored for that variable.


class ValueDescriptor {
public:
    enum class Type {
        integer,
        doubleValue,
        boolean,
        string
    };

    //virtual destructor ensures that deleting a derived descriptor through a ValueDescriptor* invokes the appropriate destructor
    virtual ~ValueDescriptor() = default;

    //Returns a value that the evaluator can use when it checks operand types
    [[nodiscard]] virtual Type type() const = 0;

    //returns a readable data type name for diagnostic messages
    [[nodiscard]] virtual std::string typeName() const = 0;

    //returns the representation used by print and by the the str convention
    [[nodiscard]] virtual std::string toString() const = 0;

    //dynamically allocates and returns a copy of the descriptor. The caller owns the returned pointer
    [[nodiscard]] virtual ValueDescriptor *clone() const = 0;
};

class IntegerValueDescriptor final : public ValueDescriptor {
public:
    explicit IntegerValueDescriptor(std::int64_t value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] std::int64_t value() const;

private:
    std::int64_t integerValue;
};

class DoubleValueDescriptor final : public ValueDescriptor {
public:
    explicit DoubleValueDescriptor(double value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] double value() const;

private:
    double doubleValue;
};

class BooleanValueDescriptor final : public ValueDescriptor {
public:
    explicit BooleanValueDescriptor(bool value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] bool value() const;

private:
    bool booleanValue;
};

class StringValueDescriptor final : public ValueDescriptor {
public:
    explicit StringValueDescriptor(std::string value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] const std::string &value() const;

private:
    std::string stringValue;
};

#endif