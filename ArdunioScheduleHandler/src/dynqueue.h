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
    infoType * peek();

    // Get the size of the stack in bytes
    unsigned long getSize();
private:
    node<infoType> *first, *last;
    long size;
};

template<typename infoType>
// infoType * dynqueue<infoType>::pop()
void dynqueue<infoType>::pop()
{
    node<infoType> * popped_node_ptr;
    // infoType * popped_info_ptr;
    switch (size)
    {
        case 0:
            // return nullptr;
            return;
        break;

        case 1:
            popped_node_ptr = first;
            // popped_info_ptr = popped_node_ptr->info;
            first = nullptr;
            last = nullptr;
            // delete popped_node_ptr;
            // size--;
            // return popped_info_ptr;
        break;

        default:
            popped_node_ptr = first;
            first = popped_node_ptr->next;
            if(first == last) last = nullptr;
            
            // popped_info_ptr = popped_node_ptr->info;
            // delete popped_node_ptr;
            // size--;
            // return popped_info_ptr;
        break;
    }
    delete popped_node_ptr;
    size--;
}

template<typename infoType>
infoType * dynqueue<infoType>::peek()
{
    return first->info;
}

template<typename infoType>
void dynqueue<infoType>::push(infoType * new_info)
{
    node<infoType> * new_node = new node<infoType>;
    new_node->info = new_info;
    switch(size)
    {
        // Set the first node
        case 0:
            first = new_node;
            size++;
        break;
        // Link the first node to the new node
        // Set the new node as the last node
        case 1:
            first->next = new_node;
            last = new_node;
            size++;
        break;
        
        // Link the last node to the new node
        // Set the last node as the new node 
        default:
            last->next = new_node;
            last = new_node;
            size++;
        break;
    }
}

template<typename infoType>
unsigned long dynqueue<infoType>::getSize()
{
    return sizeof(infoType) * size;
}