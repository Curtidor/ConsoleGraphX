#include <type_traits>
#include <iterator>

/**
 * @brief Trait to check if a type T has .begin() and .end().
 */
template <typename, typename = std::void_t<>>
struct has_begin_end : std::false_type {};

template <typename T>
struct has_begin_end<T, std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end())>> : std::true_type {};

template <typename T>
inline constexpr bool has_begin_end_v = has_begin_end<T>::value;

/**
 * @brief Template generator for iterable types.
 *
 * @tparam T The container type. Must support .begin() and .end().
 */
template <typename T>
class Generator 
{
    static_assert(has_begin_end_v<T>, "Type T must support .begin() and .end()");

    using Iterator = decltype(std::declval<T>().begin());

public:
    explicit Generator(T& container)
        : _m_begin(container.begin()), _m_end(container.end()) {}

    class GeneratorIterator 
    {
    public:
        GeneratorIterator(Iterator current, Iterator end) : _m_current(current), _m_end(end) {}

        bool operator!=(const GeneratorIterator& other) const 
        {
            return _m_current != other._m_current;
        }

        GeneratorIterator& operator++() 
        {
            if (_m_current != _m_end)
            {
                ++_m_current;
            }
            return *this;
        }

        decltype(auto) operator*() const 
        {
            return *_m_current;
        }

   protected:
        Iterator _m_current;
        Iterator _m_end;
    };

    GeneratorIterator begin() const 
    {
        return GeneratorIterator(_m_begin, _m_end);
    }

    GeneratorIterator end() const 
    {
        return GeneratorIterator(_m_end, _m_end);
    }

private:
    Iterator _m_begin;
    Iterator _m_end;
};
