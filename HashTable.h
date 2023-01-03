//
// Created by talro on 21/12/2022.
//

#ifndef WET2_HASHTABLE_H
#define WET2_HASHTABLE_H

#include <iostream>
#include "Node.h"

template <typename K, typename V>
class HashTable {
public:
    explicit HashTable(int size) : size(size), count(0) {
        arr = new Node<K,V>*[size];
        for (int i = 0; i < size; ++i) {
            arr[i] = NULL;
        }
    }

    ~HashTable() {
        for (int i = 0; i < size; ++i) {
            Node<K, V>* curr =  arr[i];
            while (curr != nullptr)
            {
                Node<K, V>* temp = curr->next;
                delete curr->value;
                delete curr;
                curr = temp;
            }
        }
        delete []arr;
    }

    void insert(const K& key, const V& value) {
        if (count >= size / 2) {
            resize(size*2);
        }

        int index = key % size;

        if (arr[index] == NULL) {
            arr[index] = new Node<K,V>{key, value, NULL};
            count++;
            return;
        }

        Node<K,V>* current = arr[index];
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = new Node<K,V>{key, value, NULL};
        count++;
    }

    const V get(const K& key) const {
        int index = key % size;

        Node<K,V>* current = arr[index];
        while (current != NULL) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }

        return NULL;
    }

    const Node<K,V>* getNode(const K& key) const {
        int index = key % size;

        Node<K,V>* current = arr[index];
        while (current != NULL) {
            if (current->key == key) {
                return current;
            }
            current = current->next;
        }

        return NULL;
    }

    void remove(const K& key){
        int index = key % size;

        Node<K,V>* current = arr[index];

        if (current != NULL && current->key == key){
            Node<K,V>* temp = current;
            current = current->next;
            delete temp;
            arr[index] = current;
            return;
        }

        while (current->next != NULL){
            if (current->next->key == key){
                Node<K,V>* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }

    void MakeEmpty(void(*deleteFunc)(V)){
        for (int i = 0; i < size; ++i) {
            Node<K,V>* current = arr[i];
            while (current != NULL) {
                deleteFunc(current->value);
                current = current->next;
            }
        }
    }

private:

    void resize(int new_size) {
        Node<K,V>** new_arr = new Node<K,V>*[new_size];
        for (int i = 0; i < new_size; ++i) {
            new_arr[i] = NULL;
        }
        for (int i = 0; i < size; ++i) {
            Node<K,V>* current = arr[i];
            while (current != NULL) {
                int index = current->key % new_size;
                new_arr[index] = new Node<K,V>{current->key, current->value, new_arr[index]};
                Node<K,V>* next = current->next;
                delete current;
                current = next;
            }
        }

        delete[] arr;
        arr = new_arr;
        size = new_size;
    }

    Node<K,V>** arr;

    int size;

    int count;
};

#endif //WET2_HASHTABLE_H
