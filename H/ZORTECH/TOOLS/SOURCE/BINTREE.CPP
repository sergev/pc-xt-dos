#include <assert.h>
#include <bintree.hpp>

// The constructor for the zBinTree class requires
// at least the pointer to the required comparison
// function as an argument. The what to do function
// pointer can be provided later.
zBinTree::zBinTree(int (* compare)(void*, void*),
          int purge, void (* what_to_do)(void*))
{
    root = NULL;
    cf = compare;
    visit = what_to_do;
    purging = purge;
}

int zBinTree::insert(void* p)
{
    zBinTreeNode *last = 0, *current = root;
    int cmp;

    if (!p) {
        err = BINTREE_BADARG;
        return err;
    }
    while (current) {
        if ((cmp = (*cf)(p, current->body)) == 0)
            break;
        else {
            last = current;
            if (cmp < 0)
                current = current->left;
            else
                current = current->right;
        }
    }
    if (!current) {
        zBinTreeNode *nn = new zBinTreeNode;
        if (!nn) {
            err = ENOMEM;
            return err;
        }
        nn->body = p;
        nn->left = nn->right = 0;
        if (!last)
            root = nn;
        else if (cmp < 0)
            last->left = nn;
        else
            last->right = nn;
        err = 0;
        return 0;
    } else {
        err = BINTREE_DUP;
        return err;
    }
}

void *zBinTree::remove(void* p)
{
    zBinTreeNode *last = 0, *current = root, *marked, *successor;
    int branch,cmp = 0;

    if (!p) {
        err = BINTREE_BADARG;
        return 0;
    }
    while (current) {
        branch = cmp;   // remember which way we moved last
        if ((cmp = (*cf)(p, current->body)) == 0)
            break;
        else {
            last = current;
            if (cmp < 0)
                current = current->left;
            else
                current = current->right;
        }
    }
    if (current) {
        marked = current;
        if (!(current->left || current->right))
            successor = 0;                // no substitute required
        else if (!current->left)
            successor = current->right;   // right sub-tree only
        else if (!current->right)
            successor = current->left;    // left sub-tree only
        else {
            successor = current = current->right;
            while (current->left)   // seek to lowest value in right sub-tree
                current = current->left;
            current->left = marked->left;   // hang left sub-tree on there
        }
        if (last) {
            if (branch < 0)
                last->left = successor;
            else
                last->right = successor;
        } else
            root = successor;
        void *p = marked->body;
        delete marked;
        err = BINTREE_OK;
        return p;
    } else {
        err = BINTREE_NOSUCH;
        return 0;
    }
}

void* zBinTree::seek(void* p)
{
    zBinTreeNode *last = 0, *current = root;
    int cmp;

    while (current) {
        if ((cmp = (*cf)(p, current->body)) == 0) {
            err = 0;
            return(current->body);
        } else {
            last = current;
            if (cmp < 0)
                current = current->left;
            else
                current = current->right;
        }
    }
    err = BINTREE_NOSUCH;
    return 0;
}

void zBinTree::inorder(zBinTreeNode* n, int first)
{
    zBinTreeNode *current;

    if (first)
        current = root;
    else
        current = n;
    if (current) {
        inorder(current->left,0);
        (* visit)(current->body);
        inorder(current->right,0);
    }
}

void zBinTree::cleanup(zBinTreeNode* n, int first)
{
    zBinTreeNode *current;
    if (first) {
        current = root;
    } else
        current = n;
    if (current) {
        cleanup(current->left,0);
        cleanup(current->right,0);
        if (purging)
            delete current->body;
        delete current;
    }
}
