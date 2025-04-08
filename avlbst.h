#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here

    //first three are for insertions
    AVLNode<Key, Value>* recurInsert(AVLNode<Key, Value>* node, const Key& key, const Value& value);
    void checkBal(AVLNode<Key, Value>* node);
    void correctImbalance(AVLNode<Key, Value>* node);

    void rebalanceAfterDelete(AVLNode<Key, Value>* node, int diff);
    void adjustAfterDoubleRotate(AVLNode<Key, Value>* node, AVLNode<Key, Value>* child, AVLNode<Key, Value>* grand, int grandBal);
    void rotateRight(AVLNode<Key, Value>* x);
    void rotateLeft(AVLNode<Key, Value>* x);


};

/*
* Insert a key-value pair into the AVL Tree.
* If the key exists, overwrite the value. Rebalance as needed.
*/
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item)
{
    Key key = new_item.first;
    Value value = new_item.second;

    // handle empty tree case
    if (this->root_ == NULL)
    {
        this->root_ = new AVLNode<Key, Value>(key, value, NULL);
        return;
    }

    // start recursive insert
    AVLNode<Key, Value>* inserted = recurInsert(static_cast<AVLNode<Key, Value>*>(this->root_), key, value);
    if (inserted != NULL)
    {
        checkBal(inserted);
    }
}


template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::recurInsert(AVLNode<Key, Value>* node, const Key& key, const Value& value)
{
    if (key == node->getKey())
    {
        node->setValue(value);
        return NULL;
    }
    else if (key < node->getKey())
    {
        if (node->getLeft() == NULL)
        {
            AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(key, value, node);
            node->setLeft(newNode);
            return newNode;
        }
        return recurInsert(static_cast<AVLNode<Key, Value>*>(node->getLeft()), key, value);
    }
    else
    {
        if (node->getRight() == NULL)
        {
            AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(key, value, node);
            node->setRight(newNode);
            return newNode;
        }
        return recurInsert(static_cast<AVLNode<Key, Value>*>(node->getRight()), key, value);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::checkBal(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* parent = node->getParent();
    if (parent == NULL) return;

    if (parent->getLeft() == node)
    {
        parent->updateBalance(-1);
    }
    else
    {
        parent->updateBalance(1);
    }

    int bal = parent->getBalance();

    if (bal == 0) return;
    else if (bal == -1 || bal == 1)
    {
        checkBal(parent);
    }
    else
    {
        correctImbalance(parent);
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::correctImbalance(AVLNode<Key, Value>* node)
{
    if (node->getBalance() == -2)
    {
        AVLNode<Key, Value>* left = node->getLeft();
        if (left->getBalance() <= 0)
        {
            rotateRight(node);
            node->setBalance(0);
            left->setBalance(0);
        }
        else
        {
            AVLNode<Key, Value>* lr = left->getRight();
            rotateLeft(left);
            rotateRight(node);

            if (lr->getBalance() == 0)
            {
                node->setBalance(0);
                left->setBalance(0);
            }
            else if (lr->getBalance() == 1)
            {
                node->setBalance(0);
                left->setBalance(-1);
            }
            else
            {
                node->setBalance(1);
                left->setBalance(0);
            }
            lr->setBalance(0);
        }
    }
    else if (node->getBalance() == 2)
    {
        AVLNode<Key, Value>* right = node->getRight();
        if (right->getBalance() >= 0)
        {
            rotateLeft(node);
            node->setBalance(0);
            right->setBalance(0);
        }
        else
        {
            AVLNode<Key, Value>* rl = right->getLeft();
            rotateRight(right);
            rotateLeft(node);

            if (rl->getBalance() == 0)
            {
                node->setBalance(0);
                right->setBalance(0);
            }
            else if (rl->getBalance() == -1)
            {
                node->setBalance(0);
                right->setBalance(1);
            }
            else
            {
                node->setBalance(-1);
                right->setBalance(0);
            }
            rl->setBalance(0);
        }
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this -> internalFind(key));
    if (node == NULL) return;

    if (node->getLeft() != NULL && node->getRight() != NULL)
    {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this-> predecessor(node));
        nodeSwap(node, pred);
    }

    AVLNode<Key, Value>* parent = node->getParent();
    Node<Key, Value>* subtree = NULL; //start off at null

    if (node->getLeft() != NULL)
    {
        subtree = node->getLeft();
    }
    else if (node->getRight() != NULL)
    {
        subtree = node->getRight();
    }

    if (subtree != NULL)
    {
        subtree -> setParent(parent);
    }

    int diff = 0;
    if (parent == NULL)
    {
        this->root_ = static_cast<AVLNode<Key, Value>*>(subtree);
    }
    else
    {
        if (parent -> getLeft() == node)
        {
            parent -> setLeft(subtree);
            diff = 1;
        }
        else
        {
            parent -> setRight(subtree);
            diff = -1;
        }
    }
    delete node;
    rebalanceAfterDelete(parent, diff);
}



template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceAfterDelete(AVLNode<Key, Value>* node, int diff)
{
    while (node != NULL)
    {
        node->updateBalance(diff);
        int balance = node->getBalance();

        if (balance == 1 || balance == -1)
        {
            break;
        }
        else if (balance == 0)
        {
            AVLNode<Key, Value>* parent = node->getParent();
            if (parent != NULL)
            {
                if (parent->getLeft() == node)
                {
                    diff = 1;
                }
                else
                {
                    diff = -1;
                }
            }
            node = parent;
        }
        else if (balance == 2)
        {
            AVLNode<Key, Value>* right = static_cast<AVLNode<Key, Value>*>(node->getRight());
            int rightBal = 0;
            if (right != NULL)
            {
                rightBal = right->getBalance();
            }

            if (rightBal >= 0)
            {
                rotateLeft(node);
                if (rightBal == 0)
                {
                    node->setBalance(1);
                    right->setBalance(-1);
                    break;
                }
                else
                {
                    node->setBalance(0);
                    right->setBalance(0);
                    AVLNode<Key, Value>* parent = node->getParent();
                    if (parent != NULL)
                    {
                        if (parent->getLeft() == node)
                        {
                            diff = 1;
                        }
                        else
                        {
                            diff = -1;
                        }
                    }
                    node = parent;
                }
            }
            else
            {
                AVLNode<Key, Value>* rl = static_cast<AVLNode<Key, Value>*>(right->getLeft());
                int rlBal = 0;
                if (rl != NULL)
                {
                    rlBal = rl->getBalance();
                }

                rotateRight(right);
                rotateLeft(node);

                adjustAfterDoubleRotate(node, right, rl, rlBal);

                AVLNode<Key, Value>* parent = node->getParent();
                if (parent != NULL)
                {
                    if (parent->getLeft() == node)
                    {
                        diff = 1;
                    }
                    else
                    {
                        diff = -1;
                    }
                }
                node = parent;
            }
        }
        else if (balance == -2)
        {
            AVLNode<Key, Value>* left = static_cast<AVLNode<Key, Value>*>(node->getLeft());
            int leftBal = 0;
            if (left != NULL)
            {
                leftBal = left->getBalance();
            }

            if (leftBal <= 0)
            {
                rotateRight(node);
                if (leftBal == 0)
                {
                    node->setBalance(-1);
                    left->setBalance(1);
                    break;
                }
                else
                {
                    node->setBalance(0);
                    left->setBalance(0);
                    AVLNode<Key, Value>* parent = node->getParent();
                    if (parent != NULL)
                    {
                        if (parent->getLeft() == node)
                        {
                            diff = 1;
                        }
                        else
                        {
                            diff = -1;
                        }
                    }
                    node = parent;
                }
            }
            else
            {
                AVLNode<Key, Value>* lr = static_cast<AVLNode<Key, Value>*>(left->getRight());
                int lrBal = 0;
                if (lr != NULL)
                {
                    lrBal = lr->getBalance();
                }

                rotateLeft(left);
                rotateRight(node);

                adjustAfterDoubleRotate(node, left, lr, lrBal);

                AVLNode<Key, Value>* parent = node->getParent();
                if (parent != NULL)
                {
                    if (parent->getLeft() == node)
                    {
                        diff = 1;
                    }
                    else
                    {
                        diff = -1;
                    }
                }
                node = parent;
            }
        }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::adjustAfterDoubleRotate(AVLNode<Key, Value>* node, AVLNode<Key, Value>* child, AVLNode<Key, Value>* grand,int grandBal)
{
    if (grand == NULL) return;
    if (child == node->getLeft())
    {
        if (grandBal == 0)
        {
            node->setBalance(0);
            child->setBalance(0);
        }
        else if (grandBal == 1)
        {
            node->setBalance(0);
            child->setBalance(-1);
        }
        else 
        {
            node->setBalance(1);
            child->setBalance(0);
        }
    }

    else if (child == node->getRight())
    {
        if (grandBal == 0)
        {
            node->setBalance(0);
            child->setBalance(0);
        }
        else if (grandBal == -1)
        {
            node->setBalance(0);
            child->setBalance(1);
        }
        else // grandBal == 1
        {
            node->setBalance(-1);
            child->setBalance(0);
        }
    }

    grand->setBalance(0); // Always reset grandchild balance
}


// Performs a left rotation around node x
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    // get the right child which will become the new root of the subtree
    AVLNode<Key, Value>* newRoot = x->getRight();

    // shift the left subtree of newRoot to be the right of x
    x->setRight(newRoot->getLeft());
    if (newRoot->getLeft() != NULL)
    {
        newRoot->getLeft()->setParent(x);
    }

    // connect newRoot to x's parent
    newRoot->setParent(x->getParent());
    if (x->getParent() == NULL)
    {
        this->root_ = newRoot; // x was the root, now newRoot is
    }
    else if (x == x->getParent()->getLeft())
    {
        x->getParent()->setLeft(newRoot);
    }
    else
    {
        x->getParent()->setRight(newRoot);
    }

    // complete the rotation
    newRoot->setLeft(x);
    x->setParent(newRoot);
}

// Performs a right rotation around node x
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    // get the left child which will become the new root of the subtree
    AVLNode<Key, Value>* newRoot = x->getLeft();

    // shift the right subtree of newRoot to be the left of x
    x->setLeft(newRoot->getRight());
    if (newRoot->getRight() != NULL)
    {
        newRoot->getRight()->setParent(x);
    }

    // connect newRoot to x's parent
    newRoot->setParent(x->getParent());
    if (x->getParent() == NULL)
    {
        this->root_ = newRoot; // x was the root, now newRoot is
    }
    else if (x == x->getParent()->getLeft())
    {
        x->getParent()->setLeft(newRoot);
    }
    else
    {
        x->getParent()->setRight(newRoot);
    }

    // complete the rotation
    newRoot->setRight(x);
    x->setParent(newRoot);
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
