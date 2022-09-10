#ifndef ARRAY_HPP
# define ARRAY_HPP

# include <iterator>
# include <algorithm>
# include <stdexcept>
# include <memory>
# include <limits>
# include <iostream>

namespace ft
{

template<typename T>
class random_iterator : public std::iterator<std::random_access_iterator_tag, T>
{
public:
    typedef std::iterator<std::random_access_iterator_tag, T> base;
    typedef typename base::iterator_category        iterator_category;
    typedef typename base::value_type               value_type;
    typedef typename base::difference_type          difference_type;
    typedef typename base::pointer                  pointer;
    typedef typename base::reference                reference;
public:
    explicit random_iterator(T* ptr = NULL) : current(ptr) {}
    explicit random_iterator(random_iterator const& other) : current(other.current) {}

    template<typename U>
    random_iterator(random_iterator<U> const& right) : current(&*right) {}

    template<typename U>
    random_iterator& operator = (random_iterator<U> const& right) { current = &*right; return *this; }

    random_iterator& operator++() { ++current; return *this; }
    random_iterator  operator++(int) { random_iterator tmp(*this); current++; return tmp; }

    random_iterator& operator--() { --current; return *this; }
    random_iterator  operator--(int) { random_iterator tmp(*this); current--; return tmp; }

    difference_type  operator+(random_iterator n) const { return current + n.current; }
    difference_type  operator-(random_iterator n) const { return current - n.current; }
    random_iterator  operator+(difference_type n) const { return random_iterator(current + n); }
    random_iterator  operator-(difference_type n) const { return random_iterator(current - n); }

    random_iterator& operator+=(difference_type n) const { current += n; return *this; }
    random_iterator& operator-=(difference_type n) const { current -= n; return *this; }

    reference operator*() const { return *current; };
    pointer   operator->() const { return current; };

    reference operator[](difference_type n) { return current[n]; }
    reference const operator[](difference_type n) const { return current[n]; }

    bool operator==(const random_iterator& rhs) const { return this->current == rhs.current; }
    bool operator!=(const random_iterator& rhs) const { return this->current != rhs.current; }
    bool operator< (const random_iterator& rhs) const { return this->current < rhs.current; }
    bool operator<=(const random_iterator& rhs) const { return this->current <= rhs.current; }
    bool operator> (const random_iterator& rhs) const { return this->current > rhs.current; }
    bool operator>=(const random_iterator& rhs) const { return this->current >= rhs.current; }
private:
    pointer current;
};

template<typename T>
typename ft::random_iterator<T>
    operator+(typename ft::random_iterator<T>::difference_type n,
              typename ft::random_iterator<T> current) { return current + n; }

template<typename T>
typename ft::random_iterator<T>
    operator-(typename ft::random_iterator<T>::difference_type n,
              typename ft::random_iterator<T> current) { return current - n; }


template<typename T, size_t Size>
class array
{
public:
    typedef T                                       value_type;
    typedef std::size_t                             size_type;
    typedef std::ptrdiff_t                          difference_type;
    typedef value_type&                             reference;
    typedef const value_type&                       const_reference;
    typedef random_iterator<T>                      iterator;
    typedef random_iterator<const T>                const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

public:
    array()
    {
    }

    array(array const& other)
    {
        for (size_t i = 0; i < Size; i++)
            operator[](i) = other[i];
    }

    ~array()
    {
    }

    array& operator=(array const& other)
    {
        for (size_t i = 0; i < Size; i++)
            operator[](i) = other[i];
    }

    reference operator[](size_type pos) { return _array[pos]; }
    const_reference operator[](size_type pos) const { return _array[pos]; }

    reference at(size_type pos)
    {
        if (pos >= size())
            throw std::out_of_range(""); // TODO normal name
        return _array[pos];
    }

    const_reference at(size_type pos) const
    {
        if (pos >= size())
            throw std::out_of_range(""); // TODO normal name
        return _array[pos];
    }

    reference front() { return _array[0]; }
    const_reference front() const { return _array[0]; }

    reference back() { return _array[Size - 1]; }
    const_reference back() const { return _array[Size - 1]; }

    iterator begin() { return iterator(_array); }
    const_iterator begin() const { return const_iterator(_array); }

    reverse_iterator rbegin() { return reverse_iterator(iterator(end())); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(const_iterator(end())); }

    iterator end() { return iterator(_array + Size); }
    const_iterator end() const { return const_iterator(_array + Size); }

    reverse_iterator rend() { return reverse_iterator(iterator(begin())); }
    const_reverse_iterator rend() const { return const_reverse_iterator(const_iterator(begin())); }

    size_type size() const { return Size; }

    void swap(array<T,Size>& other)
    {
        for (size_t i = 0; i < Size; i++)
            std::swap(operator[](i), other[i]);
    }

    template<typename U, size_t USize>
    friend void swap(array<U,USize>& lhs,
                     array<U,USize>& rhs);

    value_type _array[Size];
};

template<class T, size_t Size>
bool operator==(array<T,Size> const& lhs,
                array<T,Size> const& rhs)
{
    return lhs.size() == rhs.size() && equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class T, size_t Size>
bool operator!=(array<T,Size> const& lhs,
                array<T,Size> const& rhs)
{
    return !(lhs == rhs);
}

template<class T, size_t Size>
bool operator<(array<T,Size> const& lhs,
               array<T,Size> const& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                                    std::less<typename array<T,Size>::value_type>());
}

template<class T, size_t Size>
bool operator<=(array<T,Size> const& lhs,
                array<T,Size> const& rhs)
{
    return !(rhs < lhs);
}

template<class T, size_t Size>
bool operator>(array<T,Size> const& lhs,
               array<T,Size> const& rhs)
{
    return rhs < lhs;
}

template<class T, size_t Size>
bool operator>=(array<T,Size> const& lhs,
                array<T,Size> const& rhs)
{
    return !(lhs < rhs);
}

template<class T, size_t Size>
void swap(array<T,Size>& lhs,
          array<T,Size>& rhs)
{
    lhs.swap(rhs);
}

}

#endif
