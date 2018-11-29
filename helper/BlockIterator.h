#ifndef MEDIENINFO_BLOCKITERATOR_H
#define MEDIENINFO_BLOCKITERATOR_H

#include <cassert>      // assert
#include <cstddef>      // ptrdiff_t
#include <iterator>     // iterator
#include <type_traits>  // remove_cv
#include <utility>      // swap

template <class Type, class UnqualifiedType = std::remove_cv_t<Type>>
class BlockIterator : public std::iterator<std::forward_iterator_tag, UnqualifiedType, std::ptrdiff_t, Type*, Type&>
{
private:
    Type* itr;
    const unsigned int xsize;
    const unsigned int blocksize = 8;
    unsigned int xjumps;

public:
    BlockIterator& operator++ () { // Pre-increment
        assert(itr != nullptr && "Out-of-bounds iterator increment!");
        ++xjumps;
        if(xjumps >= blocksize) {
            xjumps = 0;
            itr += (xsize - blocksize);
        }
        else {
            ++itr;
        }
        return *this;
    }

    Type& operator* () const {
        assert(itr != nullptr && "Invalid iterator dereference!");
        return &(*itr);
    }

    BlockIterator(Type* offset, unsigned int xsize, const Type* startoffset)
        : itr(startoffset), xjumps(0), xsize(xsize) {

    }

    //
    // default methods
    //

    void swap(BlockIterator& other) noexcept {
        using std::swap;
        swap(itr, other.iter);
    }

    BlockIterator operator++ (int) { // Post-increment
        assert(itr != nullptr && "Out-of-bounds iterator increment!");
        BlockIterator tmp(*this);
        this->operator++();
        return tmp;
    }

    // two-way comparison: v.begin() == v.cbegin() and vice versa
    template<class OtherType>
    bool operator == (const BlockIterator<OtherType>& rhs) const {
        return itr == rhs.itr;
    }

    template<class OtherType>
    bool operator != (const BlockIterator<OtherType>& rhs) const {
        return itr != rhs.itr;
    }

    Type& operator-> () const {
        assert(itr != nullptr && "Invalid iterator dereference!");
        return this->operator*();
    }

    // One way conversion: iterator -> const_iterator
    operator BlockIterator<const Type>() const {
        return BlockIterator<const Type>(itr);
    }
};

// `iterator` and `const_iterator` used by your class:
//typedef BlockIterator<T> iterator;
//typedef BlockIterator<const T> const_iterator;

#endif //MEDIENINFO_BLOCKITERATOR_H
