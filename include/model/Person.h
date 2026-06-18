#pragma once
#include <string>
#include <iostream>

// ============================================================
//  Person — 抽象基底類別，展示 C++ 純虛函數與多態
// ============================================================
class Person {
protected:
    std::string id;
    std::string name;

public:
    Person(const std::string& id, const std::string& name)
        : id(id), name(name) {}

    virtual ~Person() = default;

    // Pure virtual functions — subclasses must implement
    virtual void        display()  const = 0;
    virtual std::string getType()  const = 0;

    // Concrete accessors
    std::string getId()   const { return id; }
    std::string getName() const { return name; }
};
