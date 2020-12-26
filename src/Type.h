#ifndef __BISH_TYPE_H__
#define __BISH_TYPE_H__

#include <cassert>

namespace Bish {

class Type {
private:
    typedef enum {
        UndefinedTy, IntegerTy, FractionalTy, StringTy, BooleanTy, ArrayTy
    } InternalType;
    InternalType type;
    Type *element_type;
    Type(InternalType ty) : type(ty), element_type(NULL) {}
    void swap(Type &b) {
        InternalType tmpty = type;
        Type *tmpelt = element_type;
        type = b.type;
        element_type = b.element_type;
        b.type = tmpty;
        b.element_type = tmpelt;
    }
public:
    Type(const Type &ty) : type(ty.type) {
        if (ty.element_type) {
            element_type = new Type(*ty.element_type);
        } else {
            element_type = NULL;
        }
    }
    ~Type() {
        if (element_type) delete element_type;
    }

    static Type Undef() { return Type(UndefinedTy); }
    static Type Integer() { return Type(IntegerTy); }
    static Type Fractional() { return Type(FractionalTy); }
    static Type String() { return Type(StringTy); }
    static Type Boolean() { return Type(BooleanTy); }
    static Type Array(Type ty) {
        Type t(ArrayTy);
        t.element_type = new Type(ty);
        return t;
    }

    bool defined() const { return type != UndefinedTy; }
    bool undef() const { return type == UndefinedTy; }
    bool integer() const { return type == IntegerTy; }
    bool fractional() const { return type == FractionalTy; }
    bool string() const { return type == StringTy; }
    bool boolean() const { return type == BooleanTy; }
    bool array() const { return type == ArrayTy; }
    const Type &element() const {
        assert(array());
        assert(element_type);
        return *element_type;
    }

    std::string str() const {
        switch (type) {
        case UndefinedTy:
            return "undef";
        case IntegerTy:
            return "int";
        case FractionalTy:
            return "frac";
        case StringTy:
            return "string";
        case BooleanTy:
            return "bool";
        case ArrayTy:
            return "array[?]";
        }
        return "";
    }

    Type &operator=(Type b) {
        swap(b);
        return *this;
    }

    bool operator==(const Type &b) const {
        bool result = type == b.type;
        result &= (element_type == NULL) == (b.element_type == NULL);
        if (result && element_type) {
            result &= *element_type == *b.element_type;
        }
        return result;
    }
};

}

#endif
