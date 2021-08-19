#include <iostream>
using namespace std;

enum Color {BLACK, RED};

template <typename DataType>
class RedBlackTree
{
private:
    
    class Node
    {
    public:
        DataType data;
        Node * leftChild;
        Node * rightChild;
        Node * parent;
        enum Color color;

        Node()
        :data(0), leftChild(nullptr), rightChild(nullptr), parent(nullptr), color(RED) //new node is always assumed red.
        {}

        Node(DataType item)
        :data(item), leftChild(nullptr), rightChild(nullptr), parent(nullptr), color(RED) //new node is always assumed red.
        {}
    };
    
    Node* root;

    void leftRotate(Node* node);
    void rightRotate(Node* node);
    void fixDeleteViolations(Node* &x);
    void rbTransplant(Node* &u, Node* &v);
    Node* minimum(Node* node);
    void printHelper(Node* root, string indent, bool last) const;
    Node* searchTreeHelper(Node* node, const DataType & item) const;

public:
    Node* TNULL; //just a reference to an empty node. -- much needed in implementation.
    
    RedBlackTree();

    void insert(const DataType & data);
    void remove(const DataType & item);
    Node* search(const DataType & item) const;
    bool isEmpty() const;
    void prettyPrint() const;
};

template <typename DataType>
RedBlackTree<DataType>::RedBlackTree()
{
    TNULL = new Node;
    TNULL->color = BLACK; //root must be black.
    root = TNULL;
}


template <typename DataType>
void RedBlackTree<DataType>::insert(const DataType &data)
{
    Node* k = new Node(data);
    k->leftChild = TNULL;
    k->rightChild = TNULL;

    Node* y = nullptr;
    Node* x = this->root;

    //traverse and search for the position of insertion.
    while (x != TNULL)
    {
        // y is parent of x
        y = x;
        if (k->data < x->data)
        {
            x = x->leftChild;
        }
        else
        {
            x = x->rightChild;
        }
    }
    
    //joining parent and the new node. -- assign parent and assign child.
    k->parent = y;
    if (y == nullptr)
    {
        root = k;
    }
    else if (k->data < y->data)
    {
        y->leftChild = k;
    }
    else
    {
        y->rightChild = k;
    }

    // if new node is a root node, simply return.
    if (k->parent == nullptr)
    {
        k->color = BLACK;
        return;
    }
    
    // if the grandparent is null, simply return -- no possible violation could occur.
    if (k->parent->parent == nullptr)
    {
        return;
    }
    
    //check for insertion violations
    Node* u;
    while(k->parent->color == RED)
    {
        if(k->parent == k->parent->parent->rightChild) //determine where is the uncle
        {
            u = k->parent->parent->leftChild;  //uncle is left if parent is right
        
            if (u->color == RED)
            {
                //case #1 -- Recolor Parent, GrandParent and Uncle
                u->color = BLACK;
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                //point k to GrandParent to be checked if it is a sub tree or not -- if not color root black.
                k = k->parent->parent;
            }
            else //uncle is black has 2 cases (triangle case #2 and line case #3)
            {
                //case #2 -- Triangle
                if (k == k->parent->leftChild) //if k is left of parent
                {
                    k = k->parent; //send parent as parameter to rotate right
                    rightRotate(k);
                }
                //triangle after rotation becomes line so case #3.
                //continue of case #2 if triangle.
                //case #3 if line.
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                leftRotate(k->parent->parent); //right line rotate GP left
            }
        }
        else //uncle is right
        {
            u = k->parent->parent->rightChild;
            if(u->color == RED)
            {
                //case #1 -- Recolor Parent, GrandParent and Uncle
                u->color  = BLACK;
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                //point k to GrandParent to be checked if it is a sub tree or not -- if not color root black.
                k = k->parent->parent;
            }
            else  //uncle is black has 2 cases (triangle case #2 and line case #3)
            {
                //case #2 -- Triangle
                if (k == k->parent->rightChild)
                {
                    //rotate parent in opposite direction.
                    k = k->parent;
                    leftRotate(k);
                }
                //triangle after rotation becomes line so case #3.
                //continue of case #2 if triangle.
                //case #3 if line.
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                rightRotate(k->parent->parent); //left line rotate GP left
            }
        }
        if (k == root)
        {
            break;
        }
    }
    //Make sure root is black -- to avoid violating black root proberty case #0.
    root->color = BLACK;
}

template <typename DataType>
void RedBlackTree<DataType>::leftRotate(Node* node)
{
    //1. left of node is maintained.
    //2. right of node becomes parent and its right child is maintained.
    //3. left child of new parent becomes right child of node.

    Node* newParent = node->rightChild;
    node->rightChild = newParent->leftChild; //step 3 -- right child of node equals left child of new parent
    
    if (newParent->leftChild != NULL)
    {
        newParent->leftChild->parent = node; //continue step 3 -- left child's parent of new parent equals node.
    }

    newParent->parent = node->parent; //assign new parent to the parent of the node.

    if (node->parent == NULL)
    {
        this->root = newParent; //new parent is the root of the tree.
    }
    else if (node == node->parent->leftChild)
    {
        node->parent->leftChild = newParent;
    }
    else
    {
        node->parent->rightChild = newParent;
    }

    //step 2 join new parent with the node.
    newParent->leftChild = node;
    node->parent = newParent;
}

template <typename DataType>
void RedBlackTree<DataType>::rightRotate(Node* node)
{
    //1. right of node is maintained.
    //2. left of node becomes parent and its left child is maintained.
    //3. right child of new parent becomes left child of node.
    
    Node* newParent = node->leftChild;
    node->leftChild = newParent->rightChild; //step 3 -- left child of new parent becomes left child of node.

    if (newParent->rightChild != NULL)
    {
        newParent->rightChild->parent = node; //continue step 3 -- right child's parent of new parent equals node.
    }

    newParent->parent = node->parent;

    if (node->parent == NULL)
    {
        this->root = newParent;
    }
    else if (node == node->parent->rightChild)
    {
        node->parent->rightChild = newParent;
    } else
    {
        node->parent->leftChild = newParent;
    }

    //step 2 join new parent with the node.
    newParent->rightChild = node;
    node->parent = newParent;
}

template <typename DataType>
void RedBlackTree<DataType>::printHelper(Node* root, string indent, bool last) const
{
    // print the tree structure on the screen recursively.
    if (root != TNULL)
    {
       cout << indent;
        if (last)
        {
            cout<<"R----";
            indent += "     ";
        }
        else
        {
            cout<<"L----";
            indent += "|    ";
        }
            
        string sColor = root->color?"RED":"BLACK";
        cout << root->data << "("<< sColor << ")" << endl;
        
        printHelper(root->leftChild, indent, false);
        printHelper(root->rightChild, indent, true);
    }
}

template <typename DataType>
void RedBlackTree<DataType>::prettyPrint() const
{
    if (root)
    {
        printHelper(this->root, "", true);
    }
}

template <typename DataType>
void RedBlackTree<DataType>::remove(const DataType &item)
{
    Node* node = root;
    Node* x, //pointer to be passed to the fix violation function after deleting node.
    *y; //used in third case mainly to point at right-left-most node.
    
    //1.search for to-be-deleted node and point node to it.
    node = search(item);
    
    //2.Save the color of the node to be deleted.
    Color y_original_color = node->color;
    
    //3. delete the node(node) and send the proper node(x) to fix violations.

    //Case 1: if node leftchild is null, swap rightchild with node even if the rightchild is null.
    if (node->leftChild == TNULL)
    {
        x = node->rightChild;
        rbTransplant(node, x);
    }
    //Case 2: leftchild is not null but the right child is null, swap leftchild with node.
    else if (node->rightChild == TNULL)
    {
        x = node->leftChild;
        rbTransplant(node, x);
    }
    //Case 3: Neither left or right are null. - two childern
    else
    {
        y = minimum(node->rightChild); //get right-left most node.
        y_original_color = y->color;/*update original color with the right-left most node color. (When replacing the node-to-be-deleted with a black node, a depth property violation occurs*/
        x = y->rightChild;
        
        //if y is the right child of the node to be deleted, and it does not have a left child (no left most, only right).
        if (y->parent == node)
        {
            x->parent = y; //complete node child-parent links.
        }
        else /*There exist a left most child*/
        {
            rbTransplant(y, y->rightChild); //swap y(left most node) with its rightchild.
            //right child links.
            y->rightChild = node->rightChild; //join left most node with the node-to-be-deleted's right child. (1st link).
            y->rightChild->parent = y; //complete node child-parent links. (2nd link).
        }
        
        //parent links.
        rbTransplant(node, y); //swap node-to-be-deleted with right-left most node - (two links done).
        
        //left child links.
        y->leftChild = node->leftChild; //join left most node with the node-to-be-deleted's left child. (1st link).
        y->leftChild->parent = y; //complete node child-parent links. (2nd link).
        
        y->color = node->color; //recolor the right-left most node with the node-to-be-deleted's color.
    }
    
    delete node; //delete node-to-be-deleted after replacing it.
    
    //Case 1: If one child or less - if the node-to-be-deleted is red, no violation is made in the depth property, however if the node-to-be-deleted is black then a violation in depth property occurs.
    //Case 2: 2 Childern - if the old color of the right-left-most node is red, no violation is made in the depth property, however if the old color of the right-left-most is black then a violation in depth property occurs.
    if (y_original_color == BLACK)
        fixDeleteViolations(x); //fix the violations.
}

template <typename DataType>
void RedBlackTree<DataType>::fixDeleteViolations(Node* &x)
{
    Node* s; //sibiling ptr
    while (x != root && x->color == BLACK)
    {
        //If x is the left child.
        if (x == x->parent->leftChild)
        {
            s = x->parent->rightChild;
            if (s->color == RED)
            {
                //Case 1: if the sibiling is red.
                s->color = BLACK; //recolor sibling to black.
                x->parent->color = RED; //recolor its parent to red.
                leftRotate(x->parent); //left rotate parent.
                s = x->parent->rightChild; //re-assign sibiling to be the sibiling of x.
            }
            
            if (s->leftChild->color == BLACK && s->rightChild->color == BLACK)
            {
                //Case 2.1: Sibiling is black and its both childern are black. -- this condition will never be true from the start of the loop.
                s->color = RED; //recolor sibiling to red to fix red color violation.
                x = x->parent; //traverse x to its parent to continue fixing if needed.
            }
            else
            {
                //Case 2.2: Sibiling is black and its both childern are not black.
                if (s->rightChild->color == BLACK)
                {
                    //Case 2.2.1: right child of sibiling is black
                    s->leftChild->color = BLACK; //recolor left child of sibiling to black.
                    s->color = RED; //recolor sibiling to red.
                    rightRotate(s);
                    s = x->parent->rightChild; //re-assign sibiling to be the sibiling of x.
                }
                //Case 2.2.2: right child of sibiling is red.
                s->color = x->parent->color; //set the color of sibiling as its parent color.
                x->parent->color = BLACK; //parents color black
                s->rightChild->color = BLACK; //right child of sibiling red.
                leftRotate(x->parent);
                x = root; //end loop - no more violations.
            }
        }
        //If x is the right child.
        else
        {
            s = x->parent->leftChild;
            if (s->color == RED)
            {
                //Case 1: if the sibiling is red.
                s->color = BLACK; //recolor sibling to black.
                x->parent->color = RED; //recolor its parent to red.
                rightRotate(x->parent); //right rotate parent.
                s = x->parent->leftChild; //re-assign sibiling to be the sibiling of x.
            }
            if (s->rightChild->color == BLACK && s->rightChild->color == BLACK)
            {
                //Case 2.2: Sibiling is black and its both childern are not black.
                s->color = RED; //recolor sibiling to red to fix red color violation.
                x = x->parent; //traverse x to its parent to continue fixing if needed.
            }
            else
            {
                //Case 2.2: Sibiling is black and its both childern are not black.
                if (s->leftChild->color == BLACK)
                {
                    //Case 2.2.1: left child of sibiling is black.
                    s->rightChild->color = BLACK; //recolor right child of sibiling to black.
                    s->color = RED; //recolor sibiling to red.
                    leftRotate(s);
                    s = x->parent->leftChild; //re-assign sibiling to be the sibiling of x.
                }
                //Case 2.2.2: left child of sibiling is red.
                s->color = x->parent->color; //set the color of sibiling as its parent color.
                x->parent->color = BLACK; //parents color black
                s->leftChild->color = BLACK; //parents color black
                rightRotate(x->parent);
                x = root; //end loop - no more violations.
            }
        }
    }
    x->color = BLACK; //make sure the root is black to avoid violating the black root property.
}

template <typename DataType>
void RedBlackTree<DataType>::rbTransplant(Node* &u, Node* &v)
{
    /*This functions perform swapping to two different node.*/
    
    //is root?
    if (u->parent == nullptr)
    {
        root = v;
    }
    //is left child?
    else if (u == u->parent->leftChild)
    {
        u->parent->leftChild = v;
    }
    //is right child?
    else
    {
        u->parent->rightChild = v;
    }
    
    //assign parent pointer.
    v->parent = u->parent;
}

template <typename DataType>
typename RedBlackTree<DataType>::Node* RedBlackTree<DataType>::minimum(Node* node)
{
    //get left most node.
    while (node->leftChild != TNULL)
    {
        node = node->leftChild;
    }
    return node;
}

template <typename DataType>
typename RedBlackTree<DataType>::Node* RedBlackTree<DataType>::search(const DataType &item) const
{
    return searchTreeHelper(this->root, item);
}

template <typename DataType>
typename RedBlackTree<DataType>::Node* RedBlackTree<DataType>::searchTreeHelper(Node* node, const DataType & item) const
{
    if (node == TNULL || item == node->data)
    {
        return node;
    }
    
    if (item < node->data)
    {
        return searchTreeHelper(node->leftChild, item);
    }
    return searchTreeHelper(node->rightChild, item);
}
