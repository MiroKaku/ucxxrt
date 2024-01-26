//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft shared
// source or premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the SOURCE.RTF on your install media or the root of your tools installation.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
/*
 * This module contains the definitions for the inline functions used by the
 * name undecorator.  It is intended that this file should be included
 * somewhere in the source file for the undecorator to maximise the chance
 * that they will be truly inlined.
 */

// The following class heirarchy is a special tree/DAG representation for
// composing strings. The tree representation allows fast appending, and
// allows the tree to contain DNames that are 'filled in' later.  All the
// 'real' string data is in leaf nodes so that those nodes can be shared.


class DNameNode;        // abstract base class
class   charNode;         // leaf node
class   pcharNode;        // leaf node
class   pDNameNode;       // leaf node, points to DName
class   DNameStatusNode;  // leaf node
class   pairNode;         // tree node

//#define UND_STATS
#ifdef UND_STATS
static int charNode_count;
static int pcharNode_count;
static int pcharNode_literal_count;
static int pDNameNode_count;
static int DNameStatusNode_count;
static int DNameStatusNode_get_count;
static int pairNode_count;
static int pairNode_count_avoided;
static int Replicator_count;
#endif

class DNameNode
{
protected:

    DNameNode();

    DNameNode(const DNameNode&);

public:

    virtual int         length() const PURE;
    virtual int         raw_length() const PURE;
    virtual char        getLastChar() const PURE;
    virtual pchar_t     getString(_In_z_ pchar_t, _In_ pchar_t) const PURE;

};


class charNode : public DNameNode
{
private:
    char me;

public:
    charNode(char);

    virtual int         length() const;
    virtual int         raw_length() const;
    virtual char        getLastChar() const;
    virtual pchar_t     getString(_In_z_ pchar_t, _In_ pchar_t) const;

};


class pcharNode : public DNameNode
{
private:
    pchar_t me;
    int     myLen;

public:
    template<StringLife life>
    pcharNode(pcchar_t, int, StringLifeSelector<life>);

    virtual int         length() const;
    virtual int         raw_length() const;
    virtual char        getLastChar() const;
    virtual pchar_t     getString(_In_z_ pchar_t, _In_ pchar_t) const;

};


class pDNameNode : public DNameNode
{
private:
    DName* me;

public:
    pDNameNode(DName*);

    virtual int         length() const;
    virtual int         raw_length() const;
    virtual char        getLastChar() const;
    virtual pchar_t     getString(_In_z_ pchar_t, _In_ pchar_t) const;
};


class DNameStatusNode : public DNameNode
{
private:
#define TruncationMessage       (" ?? ")
#define TruncationMessageLength (4)

    DNameStatus me;
    int         myLen;

    DNameStatusNode(DNameStatus);

public:
    static DNameStatusNode* make(DNameStatus);

    virtual int         length() const;
    virtual int         raw_length() const;
    virtual char        getLastChar() const;
    virtual pchar_t     getString(_In_z_ pchar_t, _In_ pchar_t) const;

};

class pairNode : public DNameNode
{
private:
    const   DNameNode* left;
    const   DNameNode* right;
    mutable int        myLen;

public:
    pairNode(DNameNode const*, DNameNode const*);

    virtual int         length() const;
    virtual int         raw_length() const;
    virtual char        getLastChar() const;
    virtual pchar_t     getString(_In_z_ pchar_t, _In_ pchar_t) const;

};


// Memory allocation functions

inline void* operator new (size_t sz, _HeapManager&, int noBuffer)
{
    if (noBuffer)
        return heap.getMemoryWithoutBuffer(sz);
    else
        return heap.getMemoryWithBuffer(sz);
}

void* _HeapManager::getMemoryWithoutBuffer(size_t sz)
{
    // Align the allocation on an appropriate boundary
    sz = ((sz + PACK_SIZE - 1) & ~(PACK_SIZE - 1));

    return (*pOpNew)(sz);
}

void* _HeapManager::getMemoryWithBuffer(size_t sz)
{
    // Align the allocation on an appropriate boundary
    sz = ((sz + PACK_SIZE - 1) & ~(PACK_SIZE - 1));

    // Handle an invalid request for 0 or negative amount of space.
    DASSERT(sz > 0);
    if (0 >= sz)
        sz = PACK_SIZE;

    if (blockLeft < sz) {
        // Is the request greater than the largest buffer size ?
        if (sz > memBlockSize)
            return nullptr; // If it is, there is nothing we can do

        // Allocate a new block
        Block* pNewBlock = rnew Block;

        // Did the allocation succeed ?  If so connect it up
        if (pNewBlock) {
            // Handle the initial state
            if (tail)
                tail = tail->next = pNewBlock;
            else
                head = tail = pNewBlock;

            // Compute the remaining space
            blockLeft = memBlockSize - sz;

        }
        else {
            return nullptr; // Oh-oh!  Memory allocation failure
        }
    }
    else {
        blockLeft -= sz;    // Deduct the allocated amount
    }

    // And return the buffer address
    return &(tail->memBlock[blockLeft]);
}


inline pchar_t getStringHelper ( _Inout_z_ pchar_t buf, _In_ pchar_t end, _In_count_(len) _Pre_z_ pcchar_t str, int len )
{
    int capacity = (int)(end - buf);
    if (len > capacity)
      len = capacity;
    und_memcpy(buf, str, len);
    return buf + len;
}


// Friend functions of 'DName'

inline DName operator + (char c, const DName& rd)
{
    return DName(c) + rd;
}

inline DName operator + (DNameStatus st, const DName& rd)
{
    return DName(st) + rd;
}

inline DName operator + (const StringLiteral& s, const DName& rd)
{
    return DName(s) + rd;
}


// The 'DName' constructors

inline DName::DName ()
{
    node        = 0;
    stat        = DN_valid;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;
}


#pragma intrinsic(memcpy)  // See below.
inline DName::DName (const DName & rd )
{
#if 1
    // SRA: the compiler generates terrible code for the assignments below,
    // copying each bit one at a time. I have been complaining about this for
    // years.  See VSWhidbey 2889.
    // Using intrinsic memcpy speeds up the whole undecorator by up to 25%
    // ('testundn.exe -fuzz' time reduced from 390s to 289s).
    memcpy(this, &rd, sizeof(*this));
#else
    stat        = rd.stat;
    isIndir     = rd.isIndir;
    isAUDC      = rd.isAUDC;
    isAUDTThunk = rd.isAUDTThunk;
    isArrayType = rd.isArrayType;
    node        = rd.node;
    NoTE        = rd.NoTE;
    pinPtr      = rd.pinPtr;
    comArray    = rd.comArray;
    vcallThunk  = rd.vcallThunk;
#endif
} // End of "DName" CONSTRUCTOR '(const DName&)'


DName::DName ( char c )
{
    stat        = DN_valid;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    node        = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    doPchar(c);

} // End of "DName" CONSTRUCTOR '(char)'


DName::DName(const StringLiteral& s)
{
    stat        = DN_valid;
    node        = 0;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    // If we have a string that needs copying - then copy it.
    if (s.len > 0) {
        doPchar<StringLife::Persistent>(s.str, s.len);
    }
} // End of "DName" CONSTRUCTOR '(pcchar_t)'


template<StringLife life>
DName::DName(pcchar_t name, StringLifeSelector<life>)
{
    stat        = DN_valid;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    node        = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    // Is there a string ?

    if (name) {
        int len = und_strlen(name);
        if (len) {
            doPchar<life>(name, len);
        }
        else {
            stat = DN_truncated;
        }
    }
    else {
        stat = DN_invalid;
    }

} // End of "DName" CONSTRUCTOR '(pcchar_t&,char)'


DName::DName ( pcchar_t & name, char terminator )
{
    stat        = DN_valid;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    node        = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    // Is there a string ?

    if (name)
        if (*name) {
            int len = 0;

            // How long is the string ?

            pcchar_t s;
            for (s = name; *name && (*name != terminator); name++)
                if (isValidIdentChar(*name) || UnDecorator::doNoIdentCharCheck())
                    len++;
                else {
                    stat = DN_invalid;
                    return;

                } // End of IF else

            // Copy the name string fragment

            doPchar<StringLife::Persistent>(s, len);

            // Now gobble the terminator if present, handle error conditions

            if (*name) {
                if (*name++ != terminator) {
                    stat = DN_error;
                    node = 0;

                }
            }
            elif(status() == DN_valid) {
                stat = DN_truncated;
            }
        }
        else {
            stat = DN_truncated;
        }
    else {
        stat = DN_invalid;
    }

} // End of "DName" CONSTRUCTOR '(pcchar_t&,char)'


DName::DName(DNameStatus st)
{
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    stat = (((st == DN_invalid) || (st == DN_error)) ? st : DN_valid);
    node = 0;

    if (st == DN_truncated) {
        node = DNameStatusNode::make(st);

        if (!node)
            stat = DN_error;
    }
} // End of "DName" CONSTRUCTOR '(DNameStatus)'


DName::DName(DName* pd)
{
    if (pd) {
        node = gnew pDNameNode(pd);
        stat = (node ? DN_valid : DN_error);

    } // End of IF else
    else {
        stat = DN_valid;
        node = 0;

    } // End of IF else

    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;
} // End of "DName" CONSTRUCTOR '( DName* )'


DName::DName(unsigned __int64 num)
{
    char  buf[21];
    char* pBuf = buf + 20;

    stat        = DN_valid;
    node        = 0;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    // Essentially, 'ultoa ( num, buf, 10 )' :-

    *pBuf = 0;

    do {
        *(--pBuf) = (char)((num % 10ui64) + '0');
        num /= 10ui64;

    } while (num);

    doPchar<StringLife::Temporary>(pBuf, (20 - (int)(pBuf - buf)));

} // End of "DName" CONSTRUCTOR '(unsigned long)'


DName::DName(__int64 num)
{
    char  buf[22];
    char* pBuf = buf + 21;

    stat        = DN_valid;
    node        = 0;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    // Essentially, 'ltoa ( num, buf, 10 )' :-

    *pBuf = 0;

    bool fSigned = false;
    if (num < 0) {
        fSigned = true;
        num = -num;
    }

    do {
        *(--pBuf) = (char)((num % 10ui64) + '0');
        num /= 10ui64;

    } while (num);

    if (fSigned) {
        *(--pBuf) = '-';
    }

    doPchar<StringLife::Temporary>(pBuf, (21 - (int)(pBuf - buf)));
}


// Now the member functions for 'DName'

inline int DName::isValid() const
{
    // return (( status () == DN_valid ) || ( status () == DN_truncated ));
    // equivalent test:
    typedef bool __cassert[DN_valid == 0 && DN_truncated == 1];
    return stat <= DN_truncated;
}
inline int DName::isEmpty() const
{
    DASSERT(isValid() || node == 0);    // i.e. !isValid() implies node==0
    return (node == 0);
}

inline DNameStatus DName::status()      const { return (DNameStatus)stat; } // The cast is to keep Glockenspiel quiet
inline void        DName::clearStatus()       { stat = DN_valid;          }

inline DName& DName::setPtrRef()            { isIndir = 1; return *this;            }
inline   int  DName::isPtrRef()     const   { return isIndir;                       }
inline   int  DName::isUDC()        const   { return (!isEmpty() && isAUDC);        }
inline   void DName::setIsUDC()             { if (!isEmpty()) isAUDC = TRUE;        }
inline   int  DName::isUDTThunk()   const   { return (!isEmpty() && isAUDTThunk);   }
inline   void DName::setIsUDTThunk()        { if (!isEmpty()) isAUDTThunk = TRUE;   }
inline   void DName::setIsArray()           { isArrayType = 1;                      }
inline   int  DName::isArray()      const   { return isArrayType;                   }
inline   int  DName::isNoTE()       const   { return NoTE;                          }
inline   void DName::setIsNoTE()            { NoTE = TRUE;                          }
inline   int  DName::isPinPtr()     const   { return pinPtr;                        }
inline   void DName::setIsPinPtr()          { pinPtr = TRUE;                        }
inline   int  DName::isComArray()   const   { return comArray;                      }
inline   void DName::setIsComArray()        { comArray = true;                      }
inline   int  DName::isVCallThunk() const   { return vcallThunk;                    }
inline   void DName::setIsVCallThunk()      { vcallThunk = TRUE;                    }

inline int DName::length() const
{
    if (isEmpty())
        return  0;

    return node->length();
}


inline char DName::getLastChar() const
{
    if (isEmpty())
        return  '\0';

    return node->getLastChar();
}


inline pchar_t DName::getString(_Inout_z_cap_(max) pchar_t buf, int max) const
{
    if (!isEmpty()) {
        // Does the caller want a buffer allocated ?

        if (!buf) {
            max = length() + 1;
            buf = gnew char[max]; // Get a buffer big enough

        } // End of IF then

        // If memory allocation failure, then return no buffer

        if (buf) {
            // Now, go through the process of filling the buffer. Leave one
            // character for the NUL terminator.

            pchar_t remaining = getString(buf, buf + max - 1);

            *remaining = 0; // Always NULL terminate the resulting string

        }
    }
    elif(buf)
        * buf = 0;

    // Return the buffer

    return buf;

} // End of "DName" FUNCTION "getString(pchar_t,int)"


inline pchar_t DName::getString(_In_z_ pchar_t buf, _In_ pchar_t end) const
{
    if (isEmpty())
        return  buf;

    return node->getString(buf, end);
}


DName DName::operator + (const StringLiteral& str) const
{
    DName local(*this);

    local += str;

    return local;

} // End of "DName" OPERATOR "+(pcchar_t)"


DName DName::operator + (const DName& rd) const
{
    DName local(*this);

    local += rd;

    return local;

} // End of "DName" OPERATOR "+(const DName&)"


DName DName::operator + (char ch) const
{
    DName local(*this);

    local += ch;

    // And return the newly formed 'DName'
    return local;
}


DName DName::operator + (DName* pd) const
{
    DName local(*this);

    local += pd;

    return local;

} // End of "DName" OPERATOR "+(DName*)"


DName DName::operator + (DNameStatus st) const
{
    DName local(*this);

    local += st;

    return local;

} // End of "DName" OPERATOR "+(DNameStatus)"


template<typename T>
void DName::append(const T* newRight)
{
    if (newRight) {
        node = gnew pairNode(node, newRight);
        if (!node) {
            stat = DN_error;
        }
    }
    else {
        stat = DN_error;
    }
}

DName& DName::operator += (char ch)
{
    if (isValid()) {
        if (ch) {
            if (isEmpty())
                *this = ch;
            else {
                append(gnew charNode(ch));
            }
        }
    }
    // And return self

    return *this;

} // End of "DName" OPERATOR "+=(char)"


DName& DName::operator += (const StringLiteral& str)
{
    if (isValid() && str.len > 0) {
        if (isEmpty())
            *this = str;
        else {
            append(gnew pcharNode(str.str, str.len, StringLifeSelector<StringLife::Persistent>()));
        }
    }
    // And return self

    return *this;

} // End of "DName" OPERATOR "+=(pcchar_t)"


DName& DName::operator += (DName* pd)
{
    if (isValid() && pd) {
        if (isEmpty())
            *this = pd;
        elif((pd->status() == DN_valid) || (pd->status() == DN_truncated))
        {
            append(gnew pDNameNode(pd));
        }
        else
            *this += pd->status();
    }
    // And return self

    return *this;

} // End of "DName" OPERATOR "+=(DName*)"


DName& DName::operator += (DNameStatus st)
{
    if (isValid()) {
        if (isEmpty() || ((st == DN_invalid) || (st == DN_error))) {
            *this = st;
        }
        else if (st == DN_valid) {
        #ifdef UND_STATS
            ++pairNode_count_avoided;
        #endif
            // no point in appending a node that has no error content or output
            // text
        }
        else {
            append(DNameStatusNode::make(st));
        }
    }

    // Return self
    return *this;

} // End of "DName" OPERATOR "+=(DNameStatus)"


DName& DName::operator += (const DName& rd)
{
    if (isValid()) {
        if (rd.isEmpty()) {
            *this += rd.status();
        }
        else {
            if (isEmpty())
                *this = rd;
            else {
                append(rd.node);
            }
        }
    }

    // And return self

    return *this;

} // End of "DName" OPERATOR "+=(const DName&)"



DName& DName::operator |= (const DName& rd)
{
    // Attenuate the error status. Always becomes worse. Don't propogate truncation

    if ((status() != DN_error) && !rd.isValid())
        stat = rd.status();

    // And return self

    return *this;

} // End of "DName" OPERATOR '|=(const DName&)'



DName& DName::operator = (const StringLiteral& str)
{
    stat        = DN_valid;
    node        = 0;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    doPchar<StringLife::Persistent>(str.str, str.len);

    // And return self

    return *this;

} // End of "DName" OPERATOR '=(pcchar_t)'


DName& DName::operator = (const DName& rd)
{
#if 1
    // SRA: see comment in the copy constructor DName::DName(const DName&);
    memcpy(this, &rd, sizeof(*this));
#else
    stat = rd.stat;

    node = rd.node;

    isIndir     = rd.isIndir;
    isAUDC      = rd.isAUDC;
    isAUDTThunk = rd.isAUDTThunk;
    isArrayType = rd.isArrayType;
    NoTE        = rd.NoTE;
    pinPtr      = rd.pinPtr;
    comArray    = rd.comArray;
    vcallThunk  = rd.vcallThunk;
#endif
    // And return self

    return *this;

} // End of "DName" OPERATOR '=(const DName&)'


inline DName& DName::operator = (char ch)
{
    stat        = DN_valid;
    node        = 0;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    doPchar(ch);

    return *this;

} // End of "DName" OPERATOR '=(char)'


DName& DName::operator = (DName* pd)
{
    stat        = DN_valid;
    node        = 0;
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    if (pd) {
        node = gnew pDNameNode(pd);

        if (!node)
            stat = DN_error;

    } // End of IF then
    else
        stat = DN_error;

    // And return self

    return *this;

} // End of "DName" OPERATOR '=(DName*)'


DName& DName::operator = (DNameStatus st)
{
    isIndir     = 0;
    isAUDC      = 0;
    isAUDTThunk = 0;
    isArrayType = 0;
    NoTE        = 0;
    pinPtr      = 0;
    comArray    = 0;
    vcallThunk  = 0;

    stat = st;

    if (st == DN_truncated) {
        node = DNameStatusNode::make(st);

        if (!node)
            stat = DN_error;
    }
    else {
        node = 0;
    }

    // And return self

    return *this;

} // End of "DName" OPERATOR '=(DNameStatus)'



// Private implementation functions for 'DName'

void DName::doPchar(char c)
{
    if(c)
    {
        node = gnew charNode(c);
        if (!node) {
            stat = DN_error;
        }
    }

} // End of "DName" FUNCTION "doPchar(pcchar_t,int)"


template<StringLife life>
void DName::doPchar(pcchar_t str, int len)
{
    if (node)
        *this = DN_error;
    elif(str && len > 0)
    {
        // Allocate as economically as possible

        if (len == 1) {
            node = gnew charNode(*str);
            if (!node) {
                stat = DN_error;
            }
        }
        else {
            node = gnew pcharNode(str, len, StringLifeSelector<life>());
            if (!node) {
                stat = DN_error;
            }
        }
    }
    else {
        stat = DN_invalid;
    }

} // End of "DName" FUNCTION "doPchar(pcchar_t,int)"



// The member functions for the 'Replicator'

inline int Replicator::isFull() const { return (index == 9); }
inline     Replicator::Replicator()
{
    index = -1;
#ifdef UND_STATS
    ++Replicator_count;
#endif
}



Replicator& Replicator::operator += (const DName& rd)
{
    if (!isFull() && !rd.isEmpty()) {
        DName* pNew = gnew DName(rd);

        // Don't update if failed

        if (pNew)
            dNameBuffer[++index] = pNew;

    }

    return *this;

} // End of "Replicator" OPERATOR '+=(const DName&)'


DName Replicator::operator [] (int x) const
{
    if ((x < 0) || (x > 9))
        return DName(DN_error);
    elif((index == -1) || (x > index))
        return DName(DN_invalid);
    else
        return *dNameBuffer[x];

} // End of "Replicator" OPERATOR '[](int)'



// The member functions for the 'DNameNode' classes

DNameNode::DNameNode()
{
}

// The 'charNode' virtual functions

charNode::charNode(char ch)
{
    me = ch;
#ifdef UND_STATS
    ++charNode_count;
#endif
}

inline int  charNode::length()      const { return 1; }
inline int  charNode::raw_length()  const { return length(); }

inline char charNode::getLastChar() const { return me; }

inline pchar_t charNode::getString(_In_z_ pchar_t buf, _In_ pchar_t end) const
{
    if (buf < end)
        *buf++ = me;

    return buf;
}



// The 'pcharNode' virtual functions

inline int  pcharNode::length() const { return myLen; }
inline int  pcharNode::raw_length() const { return length(); }

inline char pcharNode::getLastChar() const { return (myLen ? me[myLen - 1] : '\0'); }

template<StringLife life>
pcharNode::pcharNode(pcchar_t str, int len, StringLifeSelector<life>)
{
    if constexpr (life == StringLife::Persistent) {
        me    = const_cast<pchar_t>(str);
        myLen = len;
    }
    else {

    #ifdef UND_STATS
        ++pcharNode_count;
    #endif

        // Allocate a new string buffer if valid state

        if (len && str) {
        #ifdef UND_STATS
            if (0 == ((((uintptr_t)"string") ^ ((uintptr_t)str)) & (~(uintptr_t)0xFFFF)))
                ++pcharNode_literal_count;
        #endif
            me = gnew char[len];
            myLen = len;

            if (me) {
                und_memcpy(me, str, len);
            }
            else {
                myLen = 0;
            }

        }
        else {
            me = 0;
            myLen = 0;

        }
    }

} // End of "pcharNode" CONSTRUCTOR '(pcchar_t,int)'


inline pchar_t pcharNode::getString(_In_z_ pchar_t buf, _In_ pchar_t end) const
{
    return getStringHelper(buf, end, me, myLen);
}



// The 'pDNameNode' virtual functions

pDNameNode::pDNameNode(DName* pName)
{
    me = ((pName && ((pName->status() == DN_invalid) || (pName->status() == DN_error))) ? 0 : pName);
#ifdef UND_STATS
    ++pDNameNode_count;
#endif
}

inline int pDNameNode::length() const
{
    return me ? me->length() : 0;
}

inline int pDNameNode::raw_length() const
{
    return -1;
}

inline char pDNameNode::getLastChar() const { return (me ? me->getLastChar() : '\0'); }

inline pchar_t pDNameNode::getString(_In_z_ pchar_t buf, _In_ pchar_t end) const
{
    return me ? me->getString(buf, end) : buf;
}



// The 'DNameStatusNode' virtual functions
DNameStatusNode::DNameStatusNode(DNameStatus stat)
{
    me = stat;
    myLen = ((me == DN_truncated) ? TruncationMessageLength : 0);
#ifdef UND_STATS
    ++DNameStatusNode_count;
#endif
}

inline int DNameStatusNode::length()     const { return myLen;    }
inline int DNameStatusNode::raw_length() const { return length(); }

inline char DNameStatusNode::getLastChar() const
{
    return ((me == DN_truncated) ? TruncationMessage[TruncationMessageLength - 1] : '\0');
}

inline pchar_t DNameStatusNode::getString(_In_z_ pchar_t buf, _In_ pchar_t end) const
{
    if (me == DN_truncated) {
        return getStringHelper(buf, end, TruncationMessage, TruncationMessageLength);
    }
    return buf;
}


inline DNameStatusNode* DNameStatusNode::make(DNameStatus stat)
{
    static DNameStatusNode nodes[4] = { DN_valid, DN_truncated, DN_invalid, DN_error };

#ifdef UND_STATS
    ++DNameStatusNode_get_count;
#endif

    if (stat >= 0 && stat < 4)
        return  &nodes[stat];

    return &nodes[DN_error];

}


// The 'pairNode' virtual functions

pairNode::pairNode(DNameNode const* left, DNameNode const* right)
    : left(left)
    , right(right)
    , myLen(-1)
{
#ifdef UND_STATS
    ++pairNode_count;
#endif
}

inline int pairNode::length() const
{
    if (myLen < 0)
        myLen = left->length() + right->length();
    return myLen;
}

inline int pairNode::raw_length() const
{
    return myLen;
}

inline char pairNode::getLastChar() const
{
    char ch = right->getLastChar();
    if (ch == 0)  // i.e. the right half is empty
        ch = left->getLastChar();
    return ch;
}

inline pchar_t pairNode::getString(_In_z_ pchar_t buf, _In_ pchar_t end) const
{
    pchar_t remaining = left->getString(buf, end);
    if (remaining < end)
        return  right->getString(remaining, end);
    else
        return  remaining;
}


static unsigned int und_strlen(pcchar_t str)
{
    unsigned int len;

    for (len = 0; *str; str++)
        len++;

    return len;

} // End of FUNCTION "und_strlen"


static void   und_memcpy(_Out_bytecap_(len) pchar_t dst, pcchar_t src, unsigned int len)
{
    for (unsigned int i = 0; i < len; ++i) {
        dst[i] = src[i];
    }
} // End of FUNCTION "und_memcpy"

static unsigned int und_strncmp(pcchar_t first, pcchar_t last, unsigned int count)
{
    if (!count)
        return(0);

    while (--count && *first && *first == *last) {
        first++;
        last++;
    }

    return(*(unsigned char*)first - *(unsigned char*)last);
}

#ifdef UND_STATS
struct __unDName_atexit
{
    ~__unDName_atexit()
    {
        printf("charNode     = %d\n", charNode_count);
        printf("pcharNode     = %d\n", pcharNode_count);
        printf("    literals = %d\n", pcharNode_literal_count);
        printf("pDNameNode     = %d\n", pDNameNode_count);
        printf("DNameStatusNode    = %d\n", DNameStatusNode_count);
        printf("      get = %d\n", DNameStatusNode_get_count);
        printf("pairNode     = %d\n", pairNode_count);
        printf("     avoided = %d\n", pairNode_count_avoided);
        printf("Replicator     = %d\n", Replicator_count);

        printf("sizeof DName = %u\n", sizeof(DName));
    }
} __unDName_atexit_used;
#endif
