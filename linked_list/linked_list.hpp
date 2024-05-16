#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <vector>
#include <mutex>
#include <optional>
#include <array>
#include <assert.h>
#include <thread>
#include <set>

template<typename T>
class Node {
public:
    Node(T item, Node *next) : item{item}, next{next} {}
    Node(T item) : Node(item, nullptr) {}

    void setNext(Node *next) {
        this->next = next;
    }

    T item;
    Node *next;
};

template<typename T>
class SetOracle {
public:
    bool Add(T value) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        if(s.find(value) != s.end()) {
            return false;
        }
        s.insert(value);
        return true;
    }

    bool Remove(T value) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        if(s.find(value) == s.end()) {
            return false;
        }
        s.erase(s.find(value));
        return true;
    }

    bool Contains(T value) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        return s.find(value) != s.end();
    }

    std::set<T> s;
    std::mutex mutex;
};

template<typename T>
class CoarseLinkedList {
public:
    CoarseLinkedList() : head(new Node(std::numeric_limits<T>::min())), tail(new Node(std::numeric_limits<T>::max())){
        head->setNext(this->tail);
    }

    bool Add(T value) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        Node<T> *previous = head;
        Node<T> *current = head->next;
        while(current->item < value) {
            previous = current;
            current = current->next;
        }

        if(current->item == value) {
            return false;
        } else {
            Node<T> *node = new Node<T>(value, current);
            previous->next = node;
            return true;
        }
    }

    bool Remove(T value) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        Node<T> *previous = head;
        Node<T> *current = head->next;
        while(current->item < value) {
            previous = current;
            current = current->next;
        }

        if(current->item != value) {
            return false;
        } else {
            previous->next = current->next;
            delete current;
            return true;
        }
    }

    bool Contains(T value) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        Node<T> *current = head->next;
        while(current->item < value) {
            current = current->next;
        }

        return current->item == value;
    }

private:
    Node<T> *head;
    Node<T> *tail;
    std::mutex mutex;
};

template<typename T>
class LockNode {
public:
    LockNode(T item, LockNode *next) : item{item}, next{next} {}
    LockNode(T item) : LockNode(item, nullptr) {}

    void setNext(LockNode *next) {
        this->next = next;
    }

    void lock() {
        mutex.lock();
    }

    void unlock() {
        mutex.unlock();
    }

    T item;
    LockNode *next;
    std::mutex mutex;
};


template<typename T>
class FineLinkedList {
public:
    FineLinkedList() : head(new LockNode(std::numeric_limits<T>::min())), tail(new LockNode(std::numeric_limits<T>::max())){
        head->setNext(this->tail);
    }

    bool Add(T value) {
        LockNode<T> *previous = head;
        previous->lock();

        LockNode<T> *current = head->next;
        current->lock();

        while(current->item < value) {
            previous->unlock();
            previous = current;
            current = current->next;
            current->lock();
        }

        if(current->item == value) {
            previous->unlock();
            current->unlock();
            return false;
        } else {
            LockNode<T> *node = new LockNode<T>(value, current);
            previous->next = node;
            previous->unlock();
            current->unlock();
            return true;
        }
    }

    bool Remove(T value) {
        LockNode<T> *previous = head;
        previous->lock();

        LockNode<T> *current = head->next;
        current->lock();

        while(current->item < value) {
            previous->unlock();
            previous = current;
            current = current->next;
            current->lock();
        }

        if(current->item != value) {
            previous->unlock();
            current->unlock();
            return false;
        } else {
            previous->next = current->next;
            previous->unlock();
            current->unlock();
            delete current;
            return true;
        }
    }

    bool Contains(T value) {
        LockNode<T> *previous = head;
        previous->lock();

        LockNode<T> *current = head->next;
        current->lock();

        while(current->item < value) {
            previous->unlock();
            previous = current;
            current = current->next;
            current->lock();
        }

        bool return_value = current->item == value;

        previous->unlock();
        current->unlock();

        return return_value;
    }

private:
    LockNode<T> *head;
    LockNode<T> *tail;
};



#endif