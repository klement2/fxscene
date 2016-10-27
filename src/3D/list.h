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
 
#ifndef _LIST_H_
#define _LIST_H_

#ifndef NULL
#define NULL 0
#endif

#define SAFE_GET C==NULL ? NULL :
#define SAFE_MOVE if (C==NULL) return

typedef struct {
    unsigned char my_allocation: 1;
} NodeFlags;

/************** 1 way linked list Node ***************************/
template < class T >
class Node1w
{
protected:
    Node1w *Nxt;
    T *item;
    NodeFlags flags;
public:
    Node1w()
    {
        Nxt = NULL;
        flags.my_allocation = 0;
    }

    Node1w *GetNext()
    {
        return Nxt;
    }

    void SetNext(Node1w *n)
    {
        Nxt = n;
    }

    T *GetItem()
    {
        return item;
    }

    T* NewItem()
    {
        if (flags.my_allocation)
            delete item;
        item = new T;
        flags.my_allocation = 1;
        return item;
    }

    T* NewItem(T *from)
    {
        if (flags.my_allocation)
            delete item;
        item = (T*)(from->newCopy());
        flags.my_allocation = 1;
        return item;
    }

    void SetItem(T *item, int myAllocation = 0)
    {
        if (flags.my_allocation)
            delete this->item;
        this->item = item;
        flags.my_allocation = myAllocation;
    }

    void DeleteItem()
    {
        if (flags.my_allocation)
            delete item;
        item = NULL;
    }

    ~Node1w()
    {
        DeleteItem();
    }
};

/************** 2 way linked list Node ***************************/
template < class T >
class Node2w : public Node1w<T>
{
protected:
    Node2w *Prv;
public:
    Node2w()
    {
        Prv = NULL;
    }

    Node2w *GetPrev()
    {
        return Prv;
    }

    void SetPrev(Node2w *p)
    {
        Prv = p;
    }
};

/************** 1 way linked list ***************************/
template < class T >
class List1w
{
protected:
    Node1w<T> *C, *F, *L;
public:
    void Reset()
    {
        F = NULL;
        C = NULL;
        L = NULL;
    }

    List1w()
    {
        Reset();
    }

    void Copy(List1w <T> *copy)
    {
        F = copy->F;
        C = copy->C;
        L = copy->L;
    }

    ~List1w()
    {
        Node1w<T> *N;
        while (F != NULL) {
            N = F;
            F = N->GetNext();
            N->DeleteItem();
            delete N;
        }
        L = NULL;
        C = NULL;
    }

    Node1w<T>* appendNew()
    {
        append(NULL);
        C->NewItem();
        return C;
    }

    Node1w<T>* appendCopy(T *item)
    {
        append(NULL);
        C->NewItem(item);
        return C;
    }

    Node1w<T>* appendNew(T *item)
    {
        append(NULL);
        C->SetItem(item, 1);
        return C;
    }

    Node1w<T>* append(T *item)
    {
        Node1w<T> *n;
        n = new Node1w<T>;
        n->SetItem(item);
        if (L == NULL) {
            n->SetNext(NULL);
            L = n;
            F = n;
        } else if (C == L) {
            C->SetNext(n);
            n->SetNext(NULL);
            L = n;
        } else {
            n->SetNext(C->GetNext());
            C->SetNext(n);
        }
        C = n;
        return n;
    }

    void deleteCurrent()
    {
    }

    Node1w<T> *First()
    {
        return F;
    }

    Node1w<T> *Last()
    {
        return L;
    }

    Node1w<T> *Curr()
    {
        return C;
    }

    Node1w<T> *Next()
    {
        return SAFE_GET C->GetNext();
    }

    T *getItem()
    {
        return SAFE_GET C->GetItem();
    }

    void Down()
    {
        SAFE_MOVE;
        C = C->GetNext();
    }

    void Top()
    {
        C = F;
    }

    void Bottom()
    {
        C = L;
    }

    bool isTop()
    {
        return C == F ? true : false;
    }

    bool isBottom()
    {
        return C == L ? true : false;
    }

    bool isValid()
    {
        return C == NULL ? false : true;
    }

    void ACurr(Node1w<T> *aC)
    {
        C = aC;
    }
};

/************** 2 way linked list ***************************/
template < class T >
class List2w
{
protected:
    Node2w<T> *C, *F, *L;
public:
    List2w()
    {
        F = NULL;
        C = NULL;
        L = NULL;
    }

    ~List2w();
    void Done();
    Node2w<T>* AddNew();
    Node2w<T>* Add(T *Itm);
    void DeleteCurrent();

    Node2w<T> *First()
    {
        return F;
    }

    Node2w<T> *Last()
    {
        return L;
    }

    Node2w<T> *Curr()
    {
        return C;
    }

    Node2w<T> *Next()
    {
        return SAFE_GET C->GetNext();
    }

    Node2w<T> *Prev()
    {
        return SAFE_GET C->GetPrev();
    }

    T *GetItem()
    {
        return SAFE_GET C->GetItem();
    }

    void Down()
    {
        SAFE_MOVE;
        C = (Node2w<T>*)C->GetNext();
    }

    void Up()
    {
        SAFE_MOVE;
        C = C->GetPrev();
    }

    void Top()
    {
        C = F;
    }

    void Bottom()
    {
        C = L;
    }

    bool isTop()
    {
        return C == F ? true : false;
    }

    bool isBottom()
    {
        return C == L ? true : false;
    }

    bool isValid()
    {
        return C == NULL ? false : true;
    }

    void ACurr(Node2w<T> *Nd)
    {
        C = Nd;
    }
};

template <class T>
void List2w<T> :: Done()
{
    Node2w<T> *N;

    while (F != NULL) {
        N = F;
        F = (Node2w<T>*)N->GetNext();
        N->DeleteItem();
        delete N;
    }

    L = NULL;
    C = NULL;
}

template <class T>
List2w<T> :: ~List2w()
{
    Done();
}

template <class T>
Node2w<T>* List2w<T> :: AddNew()
{
    Add(NULL);
    C->NewItem();
    return C;
}

template <class T>
Node2w<T>* List2w<T> :: Add(T *item)
{
    Node2w<T> *n, *p;
    n = new Node2w<T>;
    n->SetItem(item);
    if (L == NULL) {
        n->SetNext(NULL);
        n->SetPrev(NULL);
        L = n;
        F = n;
    } else if (C == L) {
        p = C;
        p->SetNext(n);
        n->SetNext(NULL);
        n->SetPrev(p);
        L = n;
    } else {
        p = C;
        n->SetNext(p->GetNext());
        ((Node2w<T>*)p->GetNext())->SetPrev(n);
        p->SetNext(n);
        n->SetPrev(p);
    }
    C = n;
    return n;
}

template <class T>
void List2w<T> :: DeleteCurrent()
{
    Node2w<T> *tmp;
    if (C == NULL)
        return;
    tmp = C;
    if (C->GetPrev() != NULL)
        C->GetPrev()->SetNext(C->GetNext());
    if (C->GetNext() != NULL) {
        C->GetNext()->SetPrev(C->GetPrev());
        C = C->GetNext();
    } else {
        C = C->GetPrev();
    }
    if (tmp == L)
        L = C;
    if (tmp == F)
        F = C;
    tmp->DeleteItem();
    delete tmp;
}

#endif // _LIST_H_

