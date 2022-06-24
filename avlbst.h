#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
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
    char getBalance () const;
    void setBalance (char balance);
    void updateBalance(char diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    char balance_;
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
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
char AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(char balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(char diff)
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
    void rotateLeft(AVLNode<Key,Value>* node);
    void rotateRight(AVLNode<Key,Value>* node);
    bool isLeftChild(AVLNode<Key,Value>* child);
    bool isRightChild(AVLNode<Key,Value>* child);
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    void removeFix(AVLNode<Key,Value>* node, char diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    
    // Empty tree case
    if (this->empty() == true) {
        AVLNode<Key,Value>* temp = new AVLNode<Key,Value>(new_item.first, new_item.second, NULL);
        temp->setBalance(0);
        this->root_ = temp;
        return;
    }

    // Key already exists
    Node<Key,Value>* exists = this->internalFind(new_item.first);
    if (exists != NULL) {
        exists->setValue(new_item.second);
    }
    // Need to add key
    else {
        AVLNode<Key,Value>* curr = static_cast<AVLNode<Key,Value>*>(this->root_);
        AVLNode<Key,Value>* parent = curr;
        while(curr != NULL) {
            if (new_item.first < curr->getKey()) {
                parent = curr;
                curr = curr->getLeft();
            }
            else if (new_item.first > curr->getKey()) {
                parent = curr;
                curr = curr->getRight();
            }
        }
        // Insert newNode
        AVLNode<Key,Value>* newNode = new AVLNode<Key,Value>(new_item.first, new_item.second, parent);
        newNode->setBalance(0);
        // Update newNode's parent & get diff
        char diff;
        // Set newNode as leftchild
        if (new_item.first < parent->getKey()) {
            parent->setLeft(newNode);
            diff = -1;
        }
        // Set newNode as rightchild
        else if (new_item.first > parent->getKey()) {
            parent->setRight(newNode);
            diff = 1;
        }
        // Check parent's balance to see if we need to call insertFix
        if (parent->getBalance() == 0) {
            parent->updateBalance(diff);
            insertFix(parent, newNode);
        }
        else {
            parent->updateBalance(diff);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO

    AVLNode<Key, Value>* node = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    // Key doesn't exist
    if (node == NULL) {
        return;
    }
    // Node has 2 children
    if (node->getLeft() != NULL && node->getRight() != NULL) {
        nodeSwap(node, static_cast<AVLNode<Key,Value>*>(this->predecessor(node)));
    }
    // Update parent's balance (if parent exists)
    AVLNode<Key, Value>* parent = node->getParent();
    char diff;
    if (parent != NULL) {
        if (isLeftChild(node)) {
            diff = 1;
        }
        else {
            diff = -1;
        }
    }
    // Node has 0 children
    if (node->getLeft() == NULL && node->getRight() == NULL) {
        // Root case
        if (node->getParent() == NULL) {
            delete node;
            this->root_ = NULL;
            return;
        }
        // Node = leftchild
        if (isLeftChild(node)) {
            parent->setLeft(NULL);
        }
        // Node = rightchild
        else {
            parent->setRight(NULL);
        }
        delete node;
    }
    // Node has one child
    else {
        // Root case
        if (parent == NULL) {
            if (node->getLeft() == NULL) {
                this->root_ = node->getRight();
                node->getRight()->setParent(NULL);
                delete node;
            }
            else {
                this->root_ = node->getLeft();
                node->getLeft()->setParent(NULL);
                delete node;
            }
        }
        // Only right child
        else if (node->getLeft() == NULL) {
            if (isLeftChild(node)) {
                parent->setLeft(node->getRight());
            }
            else {
                parent->setRight(node->getRight());
            }
            node->getRight()->setParent(node->getParent());
            delete node;
        }
        // Only left child
        else if (node->getRight() == NULL){
            if (isLeftChild(node)) {
                parent->setLeft(node->getLeft());
            }
            else {
                parent->setRight(node->getLeft());
            }
            node->getLeft()->setParent(node->getParent());
            delete node;
        }
    }
    removeFix(parent, diff);

}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    char tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

// ---- My Helper Functions
template<class Key, class Value>
bool AVLTree<Key, Value>::isLeftChild(AVLNode<Key,Value>* child) {
    return (child == child->getParent()->getLeft());
}

template<class Key, class Value>
bool AVLTree<Key, Value>::isRightChild(AVLNode<Key,Value>* child) {
    return (child == child->getParent()->getRight());
}

template<class Key, class Value>
void AVLTree<Key,Value>::rotateLeft(AVLNode<Key,Value>* node) {
    
    AVLNode<Key,Value>* orphan = NULL;
    // Saving Orphan
    if (node->getRight()->getLeft() != NULL) {
        orphan = node->getRight()->getLeft();
        node->getRight()->setLeft(NULL);
    }
    // Rotating root
    if (node->getParent() == NULL) {
        this->root_ = node->getRight();
        node->getRight()->setParent(NULL);
        node->setParent(node->getRight());
        node->getRight()->setLeft(node);
    }
    else {
        if (isLeftChild(node)) {
            node->getParent()->setLeft(node->getRight());
        }
        else {
            node->getParent()->setRight(node->getRight());
        }
        node->getRight()->setParent(node->getParent());
        node->setParent(node->getRight());
        node->getRight()->setLeft(node);
    }
    // Check for orphan
    if (orphan != NULL) {
        node->setRight(orphan);
        orphan->setParent(node);
    }
    else {
        node->setRight(NULL);
    }
    
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* node) {
    
    AVLNode<Key,Value>* orphan = NULL;
    // Saving Orphan
    if (node->getLeft()->getRight() != NULL) {
        orphan = node->getLeft()->getRight();
        node->getLeft()->setRight(NULL);
    }
    // Rotating root
    if (node->getParent() == NULL) {
        this->root_ = node->getLeft();
        node->getLeft()->setParent(NULL);
        node->setParent(node->getLeft());
        node->getLeft()->setRight(node);
    }
    else {
        if (isLeftChild(node)) {
            node->getParent()->setLeft(node->getLeft());
        }
        else {
            node->getParent()->setRight(node->getLeft());
        }
        node->getLeft()->setParent(node->getParent());
        node->setParent(node->getLeft());
        node->getLeft()->setRight(node);
    }
    // Check for orphan
    if (orphan != NULL) {
        node->setLeft(orphan);
        orphan->setParent(node);
    }
    else {
        node->setLeft(NULL);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node) {
    if (parent == NULL || parent->getParent() == NULL) {
        return;
    }
    AVLNode<Key,Value>* grand = parent->getParent();
    // Update grand's balance for now
    bool leftChild = false;
    if (isLeftChild(parent)) {
        grand->updateBalance(-1);
        leftChild = true;
    }
    else if (isRightChild(parent)) {
        grand->updateBalance(1);
    }
    // Check grand's updated balance
    if (grand->getBalance() == 0) {
        return;
    }
    else if (grand->getBalance() == 1 || grand->getBalance() == -1) {
        insertFix(grand, parent); 
    }
    else if (grand->getBalance() == 2 || grand->getBalance() == -2) {
        // Zig-Zig
        if (isLeftChild(node) == isLeftChild(parent)) {
            // Left-Left
            if (isLeftChild(node)) {
                rotateRight(grand);
            }
            // Right-Right
            else {
                rotateLeft(grand);
            }
            parent->setBalance(0);
            grand->setBalance(0);
        }
        // Zig-Zag
        else {
            // Right-Left
            if (isLeftChild(parent)) {
                rotateLeft(parent);
                rotateRight(grand);
            }
            // Left-Right
            else {
                rotateRight(parent);
                rotateLeft(grand);
            }
            // Check balance of node
            if (leftChild) {
                if (node->getBalance() == -1) {
                    parent->setBalance(0);
                    grand->setBalance(1);
                    node->setBalance(0);
                    }
                else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    grand->setBalance(0);
                    node->setBalance(0);
                }
                else if (node->getBalance() == 1) {
                    parent->setBalance(-1);
                    grand->setBalance(0);
                    node->setBalance(0);
                }
            }
            else {
                if (node->getBalance() == 1) {
                    parent->setBalance(0);
                    grand->setBalance(-1);
                    node->setBalance(0);
                    }
                else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    grand->setBalance(0);
                    node->setBalance(0);
                }
                else if (node->getBalance() == -1) {
                    parent->setBalance(1);
                    grand->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* node, char diff) {
    if (node == NULL) {
        return;
    }
    AVLNode<Key,Value>* parent = node->getParent();
    char ndiff;
    if (parent != NULL) {
        if (isLeftChild(node)) {
            ndiff = 1;
        }
        else {
            ndiff = -1;
        }
    }
    // Diff = -1
    if (diff == -1) {
        if (node->getBalance() + diff == -2) {
            AVLNode<Key,Value>* c = node->getLeft();
            // Zig-Zig
            if (c->getBalance() == -1) {
                rotateRight(node);
                node->setBalance(0);
                c->setBalance(0);
                removeFix(parent, ndiff);
            }
            // Zig-Zig
            else if (c->getBalance() == 0) {
                rotateRight(node);
                node->setBalance(-1);
                c->setBalance(1);
                return;
            }
            // Zig-Zag
            else if (c->getBalance() == 1) {
                AVLNode<Key,Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(node);
                if (g->getBalance() == 1) {
                    node->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0) {
                    node->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == -1) {
                    node->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(parent, ndiff);
            }
        }
        else if (node->getBalance() + diff == -1) {
            node->setBalance(-1);
            return;
        }
        else if (node->getBalance() + diff == 0) {
            node->setBalance(0);
            removeFix(parent, ndiff);
        }
    }
    // Diff = 1
    else if (diff == 1) {
        if (node->getBalance() + diff == 2) {
            AVLNode<Key,Value>* c = node->getRight();
            // Zig-Zig
            if (c->getBalance() == 1) {
                rotateLeft(node);
                node->setBalance(0);
                c->setBalance(0);
                removeFix(parent, ndiff);
            }
            // Zig-Zig
            else if (c->getBalance() == 0) {
                rotateLeft(node);
                node->setBalance(1);
                c->setBalance(-1);
                return;
            }
            // Zig-Zag
            else if (c->getBalance() == -1) {
                AVLNode<Key,Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(node);
                if (g->getBalance() == -1) {
                    node->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0) {
                    node->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 1) {
                    node->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(parent, ndiff);
            }
        }
        else if (node->getBalance() + diff == 1) {
            node->setBalance(1);
            return;
        }
        else if (node->getBalance() + diff == 0) {
            node->setBalance(0);
            removeFix(parent, ndiff);
        }
    }
}

#endif
