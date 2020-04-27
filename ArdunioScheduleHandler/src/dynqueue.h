#pragma once
#include "node.h"

template<typename infoType>
class dynqueue
{
public:
    // Push an infoType onto the queue
    // Creates a node
    void push(infoType *);

    // Pop the next item from the queue
    // infoType * pop();
    void pop();
    
    // Get the next item from the queue without
    // removing it from the stack.
    inline infoType * peek();

    // Get the size of the stack in bytes
    inline unsigned long getSize();

private:
    node<infoType> *first, *last;
    unsigned long length;
};

template<typename infoType>
void dynqueue<infoType>::pop()
{
    node<infoType> * popped_node_ptr;
    switch (length)
    {
        case 0:
            return;
        break;

        case 1:
            popped_node_ptr = first;
            first = nullptr;
            last = nullptr;
        break;

        default:
            popped_node_ptr = first;
            first = popped_node_ptr->next;
            if(first == last) last = nullptr;
        break;
    }
    delete popped_node_ptr;
    length--;
}

template<typename infoType>
inline infoType * dynqueue<infoType>::peek()
{
    return first->info;
}

template<typename infoType>
void dynqueue<infoType>::push(infoType * new_info)
{
    node<infoType> * new_node = new node<infoType>;
    new_node->info = new_info;
    switch(length)
    {
        // Set the first node
        case 0:
            first = new_node;
        break;
        // Link the first node to the new node
        // Set the new node as the last node
        case 1:
            first->next = new_node;
            last = new_node;
        break;
        
        // Link the last node to the new node
        // Set the last node as the new node 
        default:
            last->next = new_node;
            last = new_node;
        break;
    }
    length++;
}

template<typename infoType>
inline unsigned long dynqueue<infoType>::getSize()
{
    return sizeof(infoType) * length;
}