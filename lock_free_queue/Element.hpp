//
// Created by François Costa on 18.04.2024.
//

#ifndef ELEMENT_H
#define ELEMENT_H

template <typename T>
class Element {

public:
    Element(Element *previous_element, T value) {
        this->_previous_element = previous_element;
        this->value = value;
    }

    [[nodiscard]] Element<T> *& previous_element() {
        return _previous_element;
    }

    T value;
private:
    Element *_previous_element;
};


#endif //ELEMENT_H
