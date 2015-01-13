#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifndef SDNB_GAP_VECTOR_HH_
#define SDNB_GAP_VECTOR_HH_

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
            GapVector(size_t size = 128) : size(0), \
                                                             __gapBegin(0), \
                                                             __gapEnd(size)
            {
                __data = new vector<T>(size);
            };

            GapVector(const GapVector& arg) : size(arg.size), \
                                              __gapBegin(arg.__gapBegin), \
                                              __gapEnd(arg.__gapEnd)
            {
                __data = new vector<T>(*(arg.__data));
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
                    index += (__gapSize());
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
                if (length > (__gapSize())) {
                    __grow(length);
                }
                /* copy */
                copy(first, last, __data->begin() + __gapBegin);
                __gapBegin += last - first;
                size  += length;
            };

            int
            remove(int length)
            {
                if (length < 0) {
                    length = max(length, -(int)(__gapBegin));
                    __gapBegin += length;
                    size += length;
                } else {
                    length = min(length, (int)__data->size() - (int)__gapEnd);
                    __gapEnd += length;
                    size -= length;
                }
                __shrink();
                return length;
            };

            int
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
                return distance;
            };

            /* nested classe(s) */
            class iterator;
            friend class iterator;
            class iterator : public std::iterator<random_access_iterator_tag, T>
            {
                public:
                    /* constructor(s) & destructor(s) */
                    iterator(void) : __index(0), __parent(NULL) {};
                    iterator(GapVector<T>* arg) : __parent(arg)
                    {
                        if (__parent->__gapBegin == 0) {
                            __index = __parent->__gapEnd;
                        } else {
                            __index = 0;
                        }
                    };

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
                        __index = getNewIndex(__index + offset);
                        return *this;
                    };

                    friend
                    iterator
                    operator+(iterator it, int offset)
                    {
                        it.__index = it.getNewIndex(it.__index + offset);
                        return it;
                    };

                    friend
                    iterator
                    operator+(int offset, const iterator& it)
                    {
                        size_t index = it.getNewIndex(it.__index + offset);
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
                        __index = getNewIndex(__index = offset);
                        return *this;
                    };

                    friend
                    iterator
                    operator-(iterator it, int offset)
                    {
                        it.__index = it.getNewIndex(it.__index - offset);
                        return it;
                    };

                    friend
                    iterator
                    operator-(int offset, const iterator& it)
                    {
                        size_t index = it.getNewIndex(it.__index - offset);
                        iterator diffIt(it);
                        diffIt.__index = index;
                        return diffIt;
                    };

                    friend
                    int
                    operator-(iterator lhs, const iterator& rhs)
                    {
                        size_t leftIndex = lhs.outputIndex();
                        size_t rightIndex = rhs.outputIndex();
                        return (int)leftIndex - (int)rightIndex;
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
                        __index = getNewIndex(__index + offset);
                        return __parent->operator[](__index);
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
                    inline
                    size_t
                    getNewIndex(size_t newIndex) const
                    {
                        if (__index < __parent->__gapBegin && \
                            newIndex >= __parent->__gapBegin) {
                            newIndex += (__parent->__gapSize());
                        } else if ( __index >= __parent->__gapEnd && \
                                    newIndex < __parent->__gapEnd) {
                            newIndex -= (__parent->__gapSize());
                        }
                        return newIndex;
                    }

                    inline
                    size_t
                    outputIndex(void) const
                    {
                        size_t newIndex = __index;
                        if (__index > __parent->__gapBegin) {
                            newIndex -= (__parent->__gapSize());
                        }
                        return newIndex;
                    };
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
                return reverse_iterator(this) - (size - 1);
            };

            iterator
            end(void)
            {
                return iterator(this) + size;
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

        protected:
            const size_t defaultGapSize = 128;

        private:
            /* data members */
            vector<T>* __data;
            size_t __gapBegin;
            size_t __gapEnd;

            /* member functions */
            inline
            size_t
            __gapSize(void)
            {
                return __gapEnd - __gapBegin;
            }

            void
            __grow(size_t insertSize)
            {
                size_t frontVectorSize = __data->size() - __gapEnd;
                size_t newGapSize = defaultGapSize;
                if (size + insertSize > (defaultGapSize << 1)) {
                    newGapSize = (size + insertSize) >> 1;
                }
                __data->resize(size + insertSize + newGapSize);

                copy_backward(  __data->begin() + __gapEnd, \
                                __data->begin() + __gapEnd + frontVectorSize, \
                                __data->end());
                __gapEnd = size + insertSize + newGapSize - frontVectorSize;
            };

            void
            __shrink(void)
            {
                if (__gapSize() > (size / 2) && size > 2 * defaultGapSize) {
                    size_t vectorSize = __data->size();
                    size_t frontVectorSize = __data->size() - __gapEnd;
                    size_t newGapSize = defaultGapSize;
                    vector<T>* frontVector = new vector<T>(frontVectorSize);
                    if (frontVector == NULL) {
                        cerr << "ERROR: new failed" << endl;
                        exit(EXIT_FAILURE);
                    }

                    copy(   __data->begin(), \
                            __data->begin() + frontVectorSize, \
                            frontVector->begin());
                    __data->resize(size + newGapSize);
                    __gapEnd = size + newGapSize - frontVectorSize;
                }
            };
    };
}

#endif
