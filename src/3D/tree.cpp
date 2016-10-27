/*****************************************************************************
 * 
 * Copyright (C) 1997-2016 Karol Gajdos <klement2@azet.sk>
 *
 * This file is part of the FXScene, a software for 3D modeling and
 * raytracing.     
 *
 * FXScene is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FXScene is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FXScene.  If not, see <http://www.gnu.org/licenses/>.
 *   
 *****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "tree.h"

using namespace STree;


// This tree model has the following attributes (I have observed):
// 1, A leaf can be conected with another leaf or with a brach
// 2, A brach can be connected with another leaf or with a branch
// 3, Only branch can have children => only branch can be parent
// 4, Leaves have direct connection to their parents (branches)
// 5, The previous item of the first item on a branch is NULL
// 6, The next item of the last item on a branch is NULL

/************************ Object implementation ******************************/
Object::Object()
{}
Object::Object(Object *o)
{
    *this = *o;
}
Object* Object::newCopy()
{
    Object *o = new Object(this);
    return o;
}
void * Object::getName()
{
    return NULL;
}
void Object::setName(void *name)
{}
void Object::deleteName()
{}
Object::~Object()
{}

/************************ Name implementation ******************************/
char * Name::newName(char *name)
{
    unsigned int ln;
    char *newname;
    newname = NULL;
    if (name != NULL) {
        ln = strlen(name);
        newname = (char*)malloc(ln + 1);
        if (newname != NULL) {
            strncpy(newname, name, ln);
            newname[ln] = '\0';
        }
    }
    return newname;
}

Name::Name()
{
    name = NULL;
}
Name::Name(Name *o)
{
    *this = *o;
}
Name* Name::newCopy()
{
    Name *o = new Name(this);
    o->name = newName(this->name);
    return o;
}
Name::Name(char *name)
{
    this->name = newName(name);
}
void *Name::getName()
{
    return name;
}
void Name::setName(char *name)
{
    unsigned int ln;
    if (name == NULL) {
        deleteName();
        return;
    }
    if (this->name == NULL) {
        this->name = newName(name);
    }
    ln = strlen(name);
    if (strlen(this->name) >= ln) {
        strncpy(this->name, name, ln);
        this->name[ln] = '\0';
    } else {
        deleteName();
        this->name = newName(name);
    }
}
void Name::deleteName()
{
    if (name != NULL)
        free(name);
    name = NULL;
}
Name::~Name()
{
    deleteName();
}


/************************ Leaf implementation ******************************/
void Leaf::cleanUp()
{
    next = NULL;
    previous = NULL;
    parent = NULL;
}

Leaf::Leaf()
{
    cleanUp();
}

Leaf* Leaf::getNext()
{
    return next;
}
Leaf* Leaf::getPrevious()
{
    return previous;
}
Leaf* Leaf::getParent()
{
    return parent;
}
bool Leaf::isParent()
{
    return false;
}
Leaf* Leaf::getFirstChild()
{
    return NULL;
}
Leaf* Leaf::getLastChild()
{
    return NULL;
}
void Leaf::setChild(Leaf *child)
{
    throw 1;  //error
}
void Leaf::ripChild()
{
    throw 1;  //error
}
long Leaf::getChildrenCount()
{
    return 0;
}
void Leaf::incChildrenCount()
{
    throw 1;  //error
}
void Leaf::decChildrenCount()
{
    throw 1;  //error
}
Object* Leaf::getData()
{
    return data;
}
void Leaf::setData(Object *data)
{
    this->data = data;
}
// rip the leaf from its branch
void Leaf::rip()
{
    if (next != NULL)
        next->previous = previous;
    if (previous != NULL)
        previous->next = next;
    if (parent != NULL) {
        if (parent->getFirstChild() == this)
            parent->ripChild();
        else
            parent->decChildrenCount();
    }
    cleanUp();
}
// insert this leaf under a branch item as its first child
Leaf* Leaf::insert (Leaf *item)
{
    if (next != NULL || previous != NULL || parent != NULL) {
        return NULL;
    }
    if (item != NULL && item->isParent()) {
        if (item->getFirstChild() != NULL) {
            return prepend(item->getFirstChild());
        } else
            item->setChild(this);
        parent = item; // this should be really after setChild
    } else
        throw;
    return this;
}
// prepend this leaf before an other leaf or branch
Leaf* Leaf::prepend(Leaf *item)
{
    if (next != NULL || previous != NULL || parent != NULL) {
        return NULL;
    }
    if (item == NULL) {
        cleanUp();
        return this;
    }
    parent = item->parent;
    if (parent != NULL) {
        if (parent->getFirstChild() == item) {
            parent->setChild(this);
        } else
            parent->incChildrenCount();
    }
    previous = item->previous;
    if (previous != NULL)
        previous->next = this;
    next = item;
    item->previous = this;
    return this;

}
// append this leaf after an other leaf or branch
Leaf* Leaf::append(Leaf *item)
{
    if (next != NULL || previous != NULL || parent != NULL) {
        return NULL;
    }
    if (item == NULL) {
        cleanUp();
        return this;
    }
    parent = item->parent;
    if (parent != NULL)
        parent->incChildrenCount();
    next = item->next;
    if (next != NULL)
        next->previous = this;
    previous = item;
    item->next = this;
    return this;
}
// constructors
Leaf::Leaf(Object *data, Leaf *item, bool sibling)
{
    cleanUp();
    setData(data);
    if (sibling)
        append(item);
    else
        insert(item);
}
Leaf::Leaf(Object *data)
{
    cleanUp();
    setData(data);
}
//destructor
Leaf::~Leaf()
{
    delete data;
    // rip();
}

/************************ Branch implementation ******************************/
void Branch::cleanUp()
{
    child = NULL;
    children_count = 0;
}

void Branch::setChild(Leaf* child)
{
    this->child = child;
    incChildrenCount();
}

Branch::Branch()
{
    cleanUp();
}
bool Branch::isParent()
{
    return true;
}
Leaf* Branch::getFirstChild()
{
    return child;
}
Leaf* Branch::getLastChild()
{
    Leaf *c = child, *p = c;
    while (c != NULL) {
        p = c;
        c = c->getNext();
    }
    return p;
}
long Branch::getChildrenCount()
{
    return children_count;
}
void Branch::incChildrenCount()
{
    children_count++;
}
void Branch::decChildrenCount()
{
    children_count--;
}
void Branch::ripChild()
{
    if (child == NULL)
        return;

    decChildrenCount();
    child = child->getNext();
}

Branch::Branch(Object *data, Leaf *item, bool sibling) : Leaf(data)
{
    cleanUp();
    if ((sibling) || (item != NULL && (!item->isParent())))
        append(item);
    else
        insert(item);
}
Branch::Branch(Object *data) : Leaf(data)
{
    cleanUp();
}
Branch::~Branch()
{
    if (child != NULL) {
        next = child->getNext();
        delete child;
        while (next != NULL) { // && (next->getParent()==this))
            previous = ((Branch*)next)->next;
            delete next;
            next = previous;
        }
    }
}

/************************* Tree implementation *******************************/
bool Tree::createRoot(Object *data, bool branch)
{
    bool rc = (root == NULL);
    if (rc) {
        root = new Branch(NULL);
        createItem(data, root, CHILD, branch);
    }
    return rc;
}
void Tree::createItem(Object *data, Leaf*item, bool sibling, bool branch)
{
    if (branch)
        current = new Branch(data, item, sibling);
    else
        current = new Leaf(data, item, sibling);
}

Tree::Tree()
{
    root = NULL;
    current = NULL;
    down = true;
}
Leaf* Tree::getRoot()
{
    return root;
}
Leaf* Tree::getFirst()
{
    if (root != NULL)
        return root->getFirstChild();
    else
        return NULL;
}

/* begin - compatibility...*/
Leaf* Tree::insertFirst(Object *data, Branch* item, bool branch)
{
    if (createRoot(data, branch))
        return current;
    else {
        if (item == NULL)
            item = (Branch*) root;
        createItem(data, item, CHILD, branch);
    }
    return current;
}
Leaf* Tree::insertLast(Object *data, Branch* item, bool branch)
{
    if (createRoot(data, branch))
        return current;
    else {
        if (item == NULL)
            item = (Branch*)root;
        current = item->getLastChild();
        if (current != NULL)
            createItem(data, current, SIBLING, branch);
        else
            createItem(data, item, CHILD, branch);
    }
    return current;
}
Leaf* Tree::prepend(Object *data, Leaf *item, bool branch)
{
    if (createRoot(data, branch))
        return current;
    else {
        if (item == NULL)
            item = current;
        if (item == NULL)
            throw;
        if (branch)
            current = new Branch(data);
        else
            current = new Leaf(data);
        current->prepend(item);
    }
    return current;
}
Leaf* Tree::append(Object *data, Leaf *item, bool branch)
{
    if (createRoot(data, branch))
        return current;
    else {
        if (item == NULL)
            item = current;
        if (item == NULL)
            throw;
        createItem(data, item, SIBLING, branch);
    }
    return current;
}
Branch* Tree::insertFirstBranch(Object *data, Branch* item)
{
    return (Branch*)insertFirst(data, item, true);
}
Branch* Tree::insertLastBranch(Object *data, Branch* item)
{
    return (Branch*)insertLast(data, item, true);
}
Branch* Tree::prependBranch(Object *data, Leaf* item)
{
    return (Branch*)prepend(data, item, true);
}
Branch* Tree::appendBranch(Object *data, Leaf* item)
{
    return (Branch*)append(data, item, true);
}
void Tree::ACurr(Branch *item)
{
    current = item;
}
void Tree::DeleteCurrent()
{
    delete current;
    current = NULL;
}
void Tree::Done()
{
    delete root;
    root = NULL;
    current = NULL;
}
bool Tree::First()
{
    return getFirst() != NULL;
}
bool Tree::isValid()
{
    return current != NULL;
}
void Tree::Top()
{
    current = getFirst();
    down = true;
}

Object* Tree::getItem()
{
    return (current == NULL) ?  NULL : current->getData();
}

Object* Tree::getDataOfNext()
{
    return (current == NULL || current->getNext() == NULL) ? NULL : current->getNext()->getData();
}

Leaf* Tree::Curr()
{
    return current;
}

Leaf* Tree::getNext()
{
    if (current != NULL)
        return current->getNext();
    else
        return NULL;
}
void Tree::Next()
{
    if (current != NULL)
        current = current->getNext();
}

void Tree::Walk()
{
    current = TreeWalk(current, &down);
}

void Tree::Down()
{
    do {
        current = TreeWalk(current, &down);
    } while (current != NULL && !down);
}

/* end - compatibility...*/

Leaf* Tree::CopyBranch(Leaf* item)
{
    Leaf *c, *pc, *n, *r = NULL, *nr = NULL;
    Object *data = NULL;
    bool down = true;
    if (item == NULL)
        return NULL;
    c = item;
    if (item->getNext() == NULL) {
        item = item->getParent();
    } else
        item = item->getNext();
    pc = c;
    do {
        data = c->getData();
        if (data != NULL)
            data = data->newCopy();
        if (c->isParent())
            n = new Branch(data);
        else
            n = new Leaf(data);

        if (r == NULL) {
            r = n;
            nr = r;
        } else if (down) {
            if (c->getParent() == pc) {
                r = n->insert(r);
            } else {
                r = n->append(r);
            }
        } else {
            r = r->getParent();
        }
        pc = c;
        c = TreeWalk(c, &down);
    } while(c != NULL && c != item);
    return nr;
}

Leaf* Tree::TreeWalk(Leaf* from, bool *down)
{
    Leaf *b1 = from;
    if (*down) {
        if (b1 != NULL) {
            b1 = b1->getFirstChild();
            if (b1 == NULL) {
                b1 = from->getNext();
                if (b1 == NULL) {
                    b1 = from->getParent();
                    if (b1 == root)
                        b1 = NULL;
                    *down = false;
                }
            }
        }
    } else {
        if (b1 != NULL) {
            b1 = b1->getNext();
            if (b1 == NULL) {
                b1 = from->getParent();
                if (b1 == root)
                    b1 = NULL;
            } else {
                *down = true;
            }
        }
    }
    return b1;
}

bool Tree::isWalkDirectionDown()
{
    return down;
}

void Tree::setWalkDirectionDown()
{
    down = true;
}

void Tree::setWalkDirectionUp()
{
    down = false;
}

