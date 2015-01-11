#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifndef SDNB_GAP_VECTOR_HH_
#define SDNB_GAP_VECTOR_HH_

#define DEFAULT_VECTOR_LENGTH 128

using namespace std;

namespace SDNB
{
    template <class T>
    class GapVector
    {
        public:
            /* data member(s) */
            size_t size;

            /* constructor(s) &  destructor(s) */
            GapVector(size_t size = DEFAULT_VECTOR_LENGTH) : size(0), \
                                                             __gapBegin(0), \
                                                             __gapEnd(size)
            {
                __data = new vector<T>(size);
            };

            GapVector(const GapVector& arg) : size(arg.size), \
                                              __gapBegin(arg.__gapBegin), \
                                              __gapEnd(arg.__gapEnd)
            {
                __data = new vector<T>(arg.__data);
            };

            /* operators */
            GapVector&
            operator=(GapVector arg)
            {
                if (__data != NULL) {
                    delete __data;
                }
                size = arg.size;
                __data = new vector<T>(arg.__data);
                __gapBegin = arg.__gapBegin;
                __gapEnd = arg.__gapEnd;
                return *this;
            };

            GapVector&
            operator=(const GapVector& arg)
            {
                if (__data != NULL) {
                    delete __data;
                }
                size = arg.size;
                __data = new vector<T>(arg.__data);
                __gapBegin = arg.__gapBegin;
                __gapEnd = arg.__gapEnd;
                return *this;
            };

            T&
            operator[](size_t index)
            {
                index = min(index, size - 1);
                if (index >= __gapBegin && index < __gapEnd) {
                    index += (__gapEnd - __gapBegin);
                }
                return __data->operator[](index);
            };

            const T&
            operator[](size_t index) const
            {
                return const_cast<T&>(*this)[index];
            };

            /* member functions */
            template <class iter>
            void
            insert(iter first, iter last)
            {
                size_t length = last - first;
                if (length > (__gapEnd - __gapBegin)) {
                    __grow(length);
                }
                /* copy */
                copy(first, last, __data->begin() + __gapBegin);
                __gapBegin += last - first;
                size  += length;
            };

            void
            remove(int length)
            {
                if (length < 0) {
                    length = max(length, -(int)(__gapBegin));
                    __gapBegin += length;
                    size += length;
                } else {
                    length = min(length, (int)__gapBegin);
                    __gapEnd += length;
                    size -= length;
                }

                if (__gapEnd - __gapBegin >= size << 1 && size > DEFAULT_VECTOR_LENGTH << 1) {
                    __shrink();
                }
            };

            void
            moveGap(int distance)
            {
                if (distance < 0) {
                    distance = max(distance, -(int)(__gapBegin));
                    copy_backward(  __data->begin() + __gapBegin + distance, \
                                    __data->begin() + __gapBegin, \
                                    __data->begin() + __gapEnd);
                } else if (distance > 0) {
                    distance = min(distance, (int)__gapBegin);
                    copy(   __data->begin() + __gapEnd, \
                            __data->begin() + __gapEnd + distance, \
                            __data->begin() + __gapBegin);
                }

                __gapBegin += distance;
                __gapEnd += distance;
            };

            /* nested classe(s) */
            class iterator;
            friend class iterator;
            class iterator : public std::iterator<random_access_iterator_tag, T>
            {
                public:
                    /* constructor(s) & destructor(s) */
                    iterator(void) : __index(0), __parent(NULL) {};
                    iterator(GapVector<T>* arg) : __index(0), __parent(arg) {};

                    /* operators */
                    iterator&
                    operator=(iterator arg)
                    {
                        __parent = arg.__parent;
                        __index = arg.__index;
                        return *this;
                    };

                    iterator&
                    operator++(void)
                    {
                        __index++;
                        if (__index == __parent->__gapBegin) {
                            __index = __parent->__gapEnd;
                        }
                        return *this;
                    };

                    iterator
                    operator++(int)
                    {
                        iterator tmp(*this);
                        operator++();
                        return tmp;
                    };

                    iterator&
                    operator+=(int offset)
                    {
                        calcOffset(__index + offset);
                        return *this;
                    };

                    friend
                    iterator
                    operator+(iterator it, int offset)
                    {
                        it.calcOffset(it.__index + offset);
                        return it;
                    };

                    friend
                    iterator
                    operator+(int offset, const iterator& it)
                    {
                        it.calcOffset(it.__index + offset);
                        return it;
                    };

                    iterator&
                    operator--(void)
                    {
                        __index--;
                        if (__index == __parent->__gapEnd - 1) {
                            __index = __parent->__gapBegin - 1;
                        }
                        return *this;
                    };

                    iterator
                    operator--(int)
                    {
                        iterator tmp(*this);
                        operator--();
                        return tmp;
                    };

                    iterator&
                    operator-=(int offset)
                    {
                        calcOffset(__index = offset);
                        return *this;
                    };

                    friend
                    iterator
                    operator-(iterator it, int offset)
                    {
                        it.calcOffset(it.__index - offset);
                        return it;
                    };

                    friend
                    iterator
                    operator-(int offset, const iterator& it)
                    {
                        it.calcOffset(it.__index - offset);
                        return it;
                    };

                    friend
                    int
                    operator-(iterator lhs, const iterator& rhs)
                    { 
                        return (int)lhs.__index - (int)rhs.__index;
                    };

                    inline
                    T&
                    operator*(void)
                    {
                        return __parent->__data->operator[](__index);
                    };

                    inline
                    bool
                    operator==(const iterator& rhs)
                    {
                        return __index == rhs.__index;
                    };

                    inline
                    bool
                    operator!=(const iterator& rhs)
                    { 
                        return __index != rhs.__index;
                    };

                    inline
                    bool
                    operator<(const iterator& rhs)
                    {
                        return __index < rhs.__index;
                    };

                    inline
                    bool
                    operator>(const iterator& rhs)
                    {
                        return __index > rhs.__index;
                    };

                    inline
                    bool
                    operator<=(const iterator& rhs)
                    {
                        return __index <= rhs.__index;
                    };

                    inline
                    bool
                    operator>=(const iterator& rhs)
                    {
                        return __index >= rhs.__index;
                    };

                    inline
                    T&
                    operator[](size_t offset)
                    {
                        return __parent->operator[](__index + offset);
                    };

                    inline
                    const T&
                    operator[](size_t offset) const
                    {
                        return const_cast<T&>(*this)[offset];
                    };

                private:
                    /* data members */
                    GapVector<T>* __parent;
                    size_t __index;

                    /* member functions */
                    inline void calcOffset(size_t newIndex)
                    {
                        if (__index < __parent->__gapBegin && \
                            newIndex >= __parent->__gapBegin) {
                            newIndex += (__parent->__gapEnd - __parent->__gapBegin);
                        } else if ( __index >= __parent->__gapEnd && \
                                    newIndex < __parent->__gapEnd) {
                            newIndex -= (__parent->__gapEnd - __parent->__gapBegin);
                        }
                        __index = newIndex;
                    }
            };

            using const_iterator = const iterator;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;
            /* member functions that return nested types */
            iterator
            begin(void)
            {
                return iterator(this);
            };

            reverse_iterator
            rbegin(void)
            {
                return reverse_iterator(this) - (__data->size() - 1);
            };

            iterator
            end(void)
            {
                return iterator(this) + __data->size();
            };

            reverse_iterator
            rend(void)
            {
                return reverse_iterator(this) + 1;
            };

            iterator
            gap(void)
            {
                return iterator(this) + __gapEnd;
            };

            reverse_iterator
            rgap(void)
            {
                return iterator(this) - (__gapBegin - 1);
            };

        private:
            /* data members */
            vector<T>* __data;
            size_t __gapBegin;
            size_t __gapEnd;

            /* member functions */
            void __grow(size_t insertLength)
            {
                size_t frontVectorLength = __data->size() - __gapEnd;
                size_t newGapLength = DEFAULT_VECTOR_LENGTH;
                if (size + insertLength > (DEFAULT_VECTOR_LENGTH << 1)) {
                    newGapLength = (size + insertLength) >> 1;
                }
                __data->resize(size + insertLength + newGapLength);

                copy_backward(  __data->begin() + __gapEnd, \
                                __data->begin() + __gapEnd + frontVectorLength, \
                                __data->end());
                __gapEnd = size + insertLength + newGapLength - frontVectorLength;
            };

            void __shrink(void)
            {
                size_t vectorLength = __data->size();
                size_t frontVectorLength = __data->size() - __gapEnd;
                size_t newGapLength = DEFAULT_VECTOR_LENGTH;
                if (size > (DEFAULT_VECTOR_LENGTH << 1)) {
                    newGapLength = (size >> 1);
                }

                vector<T>* frontVector = new vector<T>(frontVectorLength);
                if (frontVector == NULL) {
                    cerr << "ERROR: new failed" << endl;
                    exit(EXIT_FAILURE);
                }

                copy(   __data->begin(), \
                        __data->begin() + frontVectorLength, \
                        frontVector->begin());
                __data->resize(size + newGapLength);
                __gapEnd = size + newGapLength - frontVectorLength;
            };
    };
}

#endif
