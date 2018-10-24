
namespace sstd{
    template< typename T >
class alignas(T *) ArrayView {
    T * mmm_Data;
    std::size_t mmm_Size;
public:

    inline constexpr ArrayView(T * d, std::size_t n) noexcept :mmm_Data(d), mmm_Size(n) {
    }

    inline constexpr ArrayView(const ArrayView &) noexcept = default;
    inline constexpr ArrayView(ArrayView &&) noexcept = default;
    inline constexpr ArrayView&operator=(const ArrayView &) noexcept = default;
    inline constexpr ArrayView&operator=(ArrayView &&) noexcept = default;

    using element_type = T;
    using value_type = std::remove_cv_t<element_type>;
    using index_type = std::ptrdiff_t;
    using difference_type = index_type;
    using pointer = T * ;
    using reference = T & ;
    using const_reference = const T &;
    using iterator = T * ;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    inline constexpr std::size_t size() const noexcept {
        return mmm_Size;
    }

    inline constexpr pointer data() const noexcept {
        return mmm_Data;
    }

    inline constexpr iterator begin() const noexcept {
        return data();
    }

    inline constexpr iterator end() const noexcept {
        return begin() + mmm_Size;
    }

    inline constexpr reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }

    inline constexpr reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }

    inline constexpr const_iterator cbegin() const noexcept {
        return data();
    }

    inline constexpr const_iterator cend() const noexcept {
        return begin() + mmm_Size;
    }

    inline constexpr const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    inline constexpr const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    inline constexpr reference operator[](std::size_t N) noexcept {
        return data()[N];
    }

    inline constexpr const_reference operator[](std::size_t N) const noexcept {
        return data()[N];
    }

};
}/*namespace sstd*/

