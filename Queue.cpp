/*
 * =====================================================================================
 *
 *       Filename:  Queue.cpp
 *
 *    Description:  The defeinition for the Queue class
 *
 *        Version:  1.0
 *        Created:  03/19/2017 08:19:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
 *        Company:  FH Südwestfalen, Iserlohn
 *
 * =====================================================================================
 */

#include "Queue.h"

template <class T>
Queue<T>::Queue(){
    frontPtr = NULL;
    backPtr = NULL;
    count = 0;
}

template <class T>
bool Queue<T>::isEmpty()
{
    return (count == 0);
}

template <class T>
bool Queue<T>::enqueue(T data)
{
    if (count < MAXQUEUESIZE) {
        Node* newOne = new Node;
        newOne->date = data;
        newOne->next = NULL;
        if (isEmpty()) {
            frontPtr = newOne;
        }
        else {
            backPtr->next = newOne;
        }
        backPtr = newOne;
        count++;
        return true;
    }
    return false;
}

template <class T>
T Queue<T>::dequeue()
{
    if (isEmpty()) {
        return 0;
    }
    else {
        T data;
        Node* temp = frontPtr;
        if (frontPtr == backPtr) {
            frontPtr = NULL;
            backPtr = NULL;
        }
        else {
            frontPtr = frontPtr->next;
        }
        data = temp->date;
        delete temp;
        count--;
        return data;
    }
}
