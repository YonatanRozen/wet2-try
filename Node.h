//
// Created by yonat on 25/12/2022.
//

#ifndef WET2_NODE_H
#define WET2_NODE_H

template<typename V>
struct Node {
    int key;
    V value;
    Node* next;
};

#endif //WET2_NODE_H
