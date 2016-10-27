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

#ifndef _TREE_H_ 
#define _TREE_H_

namespace STree
{

#ifndef NULL
#define NULL 0
#endif

const bool SIBLING = true;
const bool CHILD = false;

class Object
{
public:
    Object();
    Object(Object *o);
    virtual Object* newCopy();
    virtual void *getName();
    virtual void setName(void *name);
    virtual void deleteName();
    virtual ~Object();
};

class Name : public Object
{
private:
    char *name;
    char *newName(char *name);
public:
    Name();
    Name(Name *o);
    virtual Name* newCopy();
    Name(char *name);
    virtual void *getName();
    virtual void setName(char *name);
    void deleteName();
    virtual ~Name();
};

class Leaf
{
private:
    void cleanUp();
protected:
    Leaf *next;
    Leaf *previous;
    Leaf *parent;

    Object *data;

    Leaf();
public:
    Leaf* getNext();
    Leaf* getPrevious();
    Leaf* getParent();
    virtual bool isParent();
    virtual Leaf* getFirstChild();
    virtual Leaf* getLastChild();
    virtual void setChild(Leaf *child);
    virtual void ripChild();
    virtual long getChildrenCount();
    virtual void incChildrenCount();
    virtual void decChildrenCount();
    Object *getData();
    void setData(Object *data);
    // rip the leaf from its branch
    void rip();
    // insert this leaf under a branch item as its first child
    Leaf* insert (Leaf *item);
    // prepend this leaf before an other leaf or branch
    Leaf* prepend(Leaf *item);
    // append this leaf after an other leaf or branch
    virtual Leaf* append(Leaf *item);
    // constructors
    Leaf(Object *data, Leaf *item, bool sibling = false);
    Leaf(Object *data);
    //destructor
    virtual ~Leaf();
};

class Branch : public Leaf
{
private:
    void cleanUp();
protected:
    Leaf *child;
    long children_count;
    virtual void setChild(Leaf* child);

public:
    Branch();
    virtual bool isParent();
    virtual Leaf* getFirstChild();
    virtual Leaf* getLastChild();
    virtual long getChildrenCount();
    virtual void incChildrenCount();
    virtual void decChildrenCount();
    virtual void ripChild();
    Branch(Object *data, Leaf *item, bool sibling = false);
    Branch(Object *data);
    virtual ~Branch();
};

class Tree
{
private:
    Leaf *root;
    Leaf *current;
    bool down;

    bool createRoot(Object *data, bool branch = false);
    void createItem(Object *data, Leaf*item, bool sibling, bool branch = false);

public:
    Tree();
    Leaf* getRoot();
    Leaf* getFirst();
    /* begin - compatibility...*/
    Leaf* insertFirst(Object *data, Branch* item, bool branch = false);
    Leaf* insertLast(Object *data, Branch* item, bool branch = false);
    Leaf* prepend(Object *data, Leaf *item = NULL, bool branch = false);
    Leaf* append(Object *data, Leaf *item = NULL, bool branch = false);
    Branch* insertFirstBranch(Object *data, Branch* item = NULL);
    Branch* insertLastBranch(Object *data, Branch* item = NULL);
    Branch* prependBranch(Object *data, Leaf* item = NULL);
    Branch* appendBranch(Object *data, Leaf* item = NULL);
    void ACurr(Branch *item);
    void DeleteCurrent();
    void Done();
    bool First();
    bool isValid();
    void Top();
    Object *getItem();
    Object *getDataOfNext();
    Leaf* Curr();
    Leaf* getNext();
    void Next();
    void Walk();
    void Down();
    /* end - compatibility...*/

    Leaf* CopyBranch(Leaf* item);
    Leaf* TreeWalk(Leaf* from, bool /* in-out */ *down);
    bool isWalkDirectionDown();
    void setWalkDirectionDown();
    void setWalkDirectionUp();
};

} // namespace


#endif // _TREE_H_
