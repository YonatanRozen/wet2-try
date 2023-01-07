//
// Created by talro on 21/12/2022.
//

#ifndef WET2_HASHTABLE_H
#define WET2_HASHTABLE_H

#include <iostream>
#include "Node.h"

template <typename V>
class HashTable {
public:
    explicit HashTable(int size) : size(size), count(0) {
        arr = new Node<V>*[size];
        for (int i = 0; i < size; ++i) {
            arr[i] = NULL;
        }
    }

    ~HashTable() {
        for (int i = 0; i < size; ++i) {
            Node<V>* curr =  arr[i];
            while (curr != nullptr)
            {
                Node<V>* temp = curr->next;
                delete curr->value;
                delete curr;
                curr = temp;
            }
        }
        delete []arr;
    }

    const Node<V>* getNode(int key) const {
        int index = key % size;

        Node<V>* current = arr[index];
        while (current != NULL) {
            if (current->key == key) {
                return current;
            }
            current = current->next;
        }

        return NULL;
    }

    const V get(int key) const {
        int index = key % size;

        Node<V>* current = arr[index];
        while (current != NULL) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }

        return NULL;
    }

    void remove(int key){
        int index = key % size;

        Node<V>* current = arr[index];

        if (current != NULL && current->key == key){
            Node<V>* temp = current;
            current = current->next;
            delete temp;
            arr[index] = current;
            return;
        }

        while (current->next != NULL){
            if (current->next->key == key){
                Node<V>* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }

    void MakeEmpty(void(*deleteFunc)(V)){
        for (int i = 0; i < size; ++i) {
            Node<V>* current = arr[i];
            while (current != NULL) {
                deleteFunc(current->value);
                current = current->next;
            }
        }
    }

    void insert(int key, const V& value) {
        if (count >= size / 2) {
            resize(size*2);
        }

        int index = key % size;

        if (arr[index] == NULL) {
            arr[index] = new Node<V>{key, value, NULL};
            count++;
            return;
        }

        Node<V>* current = arr[index];
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = new Node<V>{key, value, NULL};
        count++;
    }

private:

    void resize(int new_size) {
        Node<V>** new_arr = new Node<V>*[new_size];
        for (int i = 0; i < new_size; ++i) {
            new_arr[i] = NULL;
        }
        for (int i = 0; i < size; ++i) {
            Node<V>* current = arr[i];
            while (current != NULL) {
                int index = current->key % new_size;
                new_arr[index] = new Node<V>{current->key, current->value, new_arr[index]};
                Node<V>* next = current->next;
                delete current;
                current = next;
            }
        }

        delete[] arr;
        arr = new_arr;
        size = new_size;
    }

    Node<V>** arr;

    int size;

    int count;
};

#endif //WET2_HASHTABLE_H
