/*!
    \file
    \brief map-like контейнер, в котором порядок обхода определяется последовательностью добавления элементов. Накидал пока минималку, если что, будем допиливать

    Пары ключ-значение храним в векторе. Индексы элементов для ускорения поиска храним в map/unordered_map. Удаление елементов требует пербора map и обновления индексов там,
    но наш контейнер редко будет использоватся для удаления элементов.

    erase работает очень медленно и через жопу, но это не критично для контейнера, у которого обычно никогда ничего не удаляется.

 */

#pragma once

#include "container_options.h"
#include "exceptions.h"
//

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <initializer_list>
#include <map>
#include <memory>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <utility>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// marty::containers::
namespace marty {
namespace containers {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template< typename Key
        , typename T
        , UpdateStrategy updateStrategy = UpdateStrategy::updateInplace // updateChangeOrder
        , typename ContainerT           = std::vector< std::pair<const Key, T> >
        , typename MapT                 = std::unordered_map< Key, std::size_t >
        >
class insertion_ordered_map
{

public: // types

    using key_type         = Key;
    using mapped_type      = T;

    using container_type   = ContainerT;
    using map_type         = MapT;

    using insertion_ordered_map_type = insertion_ordered_map<key_type, mapped_type, updateStrategy, container_type, map_type>;

    using value_type       = std::pair<const Key, T>;
    using size_type        = std::size_t;
    using index_type       = std::size_t;
    using difference_type  = std::ptrdiff_t;
    using allocator_type   = typename container_type::allocator_type;

    using reference        = value_type&      ;
    using const_reference  = const value_type&;

    using pointer          = typename std::allocator_traits<allocator_type>::pointer      ;
    using const_pointer    = typename std::allocator_traits<allocator_type>::const_pointer;

    using iterator               = typename container_type::iterator              ;
    using const_iterator         = typename container_type::const_iterator        ;
    using reverse_iterator	     = typename container_type::reverse_iterator      ;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;


protected: // member fields

    container_type         m_container;
    map_type               m_map      ;


public: // ctors and assigns

    insertion_ordered_map() = default;
    insertion_ordered_map(const insertion_ordered_map &) = default;
    insertion_ordered_map(insertion_ordered_map &&) = default;

    insertion_ordered_map& operator=(const insertion_ordered_map &other)
    {
        m_container.clear();
        m_container.reserve(other.m_container.size());
        const_iterator it = other.m_container.begin();
        for(; it!=other.m_container.end(); ++it)
        {
            m_container.push_back(*it);
        }

        m_map = other.m_map;

        return *this;
    }

    insertion_ordered_map& operator=(insertion_ordered_map && other)
    {
        m_container = container_type(); // .clear();
        m_map       = map_type      (); // .clear();
        swap(other);
        return *this;
    }

    template< class InputIt >
    insertion_ordered_map( InputIt first, InputIt last )
    {
        insert(first, last);
    }

    insertion_ordered_map(std::initializer_list<value_type> init)
    {
        insert(init.begin(), init.end());
    }

    void swap(insertion_ordered_map &other)
    {
        m_container.swap(other.m_container);
        m_map      .swap(other.m_map      );
    }


protected: // helper classes

    // class UpdateProxy
    // {
    //     const key_type& keyRef;
    //     mapped_type&    valueRef;
    //  
    // public:
    //  
    //     UpdateProxy(const key_type& key, mapped_type &ref) : keyRef(key), valueRef(ref) {}

    class UpdateProxy
    {
        insertion_ordered_map_type  *pMap = 0;
        iterator  updateIt;

        // Позволяет игнорировать стратегию обновления - нужно, чтобы задавать значение, если элемент был добавлен по дефолту
        bool forceAllowUpdate = false; // 


        // mapped_type                 &valueRef;
        // const key_type              *pKey = 0;
        // std::size_t                 idx   = std::size_t(-1);

    public:

        // UpdateProxy(insertion_ordered_map_type *pm, mapped_type &ref, const key_type *pk) : pMap(pm), valueRef(ref), pKey(pk) {}
        // UpdateProxy(insertion_ordered_map_type *pm, mapped_type &ref, std::size_t idx_  ) : pMap(pm), valueRef(ref), idx(idx_) {}

        UpdateProxy(insertion_ordered_map_type *pm, iterator it, bool forceAllowUpdate_=false) : pMap(pm), updateIt(it), forceAllowUpdate(forceAllowUpdate_) {}

        UpdateProxy() = delete;

        UpdateProxy(const UpdateProxy &) = default;
        UpdateProxy(UpdateProxy &&) = default;

        UpdateProxy& operator=(const UpdateProxy &) = delete;
        UpdateProxy& operator=(UpdateProxy &&) = delete;

        // Для простоты не будем поддерживать "цепочки" присваиваний: m["a"] = m["b"] = m["c"] = m["d"];

        // Тут нет проблем - возвращаем константную ссылку
        //const mapped_type& operator mapped_type() const
        operator mapped_type() const &&
        {
            // return valueRef;
            return updateIt->second;
        }

        UpdateProxy& operator=(const mapped_type& newVal) &&
        {
            if (forceAllowUpdate)
            {
                // Если принудительно обновляем, то inplace, без смены положения
                updateIt->second = newVal;
                return *this;
            }

            if constexpr (updateStrategy==UpdateStrategy::updateRestrict)
                throw update_error("marty::containers::insertion_ordered_map::at: updating forbidden due UpdateStrategy::updateRestrict option");
        
            if constexpr (updateStrategy==UpdateStrategy::updateInplace)
            {
                //valueRef = newVal;
                updateIt->second = newVal;
                return *this;
            }

            if constexpr (updateStrategy==UpdateStrategy::updateChangeOrder)
            {
                // Удаляем существующий элемент и помещаем в конец новый 
                // Стратегия обновления UpdateStrategy::updateChangeOrder
                key_type key = updateIt->first;
                pMap->erase(updateIt);
                pMap->insert(value_type{key, newVal});
                return *this;
            }
        }

    }; // class UpdateProxy

    friend UpdateProxy;


protected: // helper methods

    void renumerateIndexes( iterator it, std::size_t idx, std::size_t sz)
    {
        for(; idx!=sz; ++idx, ++it)
        {
            m_map[it->first] = idx;
        }
    }

    static constexpr bool isVector()
    {
        return std::is_same_v< container_type, std::vector<std::pair<const Key, T> > >;
    }

    //! Простое удаление из вектора std:pair, где первый элемент пары - константа - это проблема, потому что копирование (и, возможно, и перемещение) - запрещены.
    // Поэтому делаем не эффективно, через жопу, но оно работает
    template<typename IterType>
    void containerEraseImpl(IterType b, IterType e)
    {
        // if constexpr (!isVector())
        // {
        //     // list, or, may be, deque, or something else
        //     // Но если не будет работать, будем разбираться
        //     m_container.erase(b,e);
        // }
        // else
        {
            auto tailSize = std::size_t(distance(e, m_container.end()));  // Определили размер
            container_type tailVec; tailVec.reserve(tailSize);            // Создали вектор под хвост и зарезервировали под вектор

            auto eraseDistSize     = std::size_t(distance(b, e));         // Сколько удаляем, между итераторами
            //auto eraseBeginIdx     = std::size_t(distance(m_container.begin(), b)); // Индекс первого удаляемого элемента
            auto eraseEndIdx       = std::size_t(distance(m_container.begin(), e)); // Индекс первого оставляемого в хвосте элемента
            auto eraseTotalSize    = eraseDistSize + tailSize;            // Сколько удаляем всего - от итератора b до конца вектора
            auto eraseRestSize     = m_container.size() - eraseTotalSize; // Сколько оставляем в начале исходного контейнера

            // Скопировали (переместили) хвост
            auto tailCopyIt = m_container.begin();
            advance(tailCopyIt, difference_type( eraseEndIdx /* eraseBeginIdx+eraseDistSize */ ));
            for(; tailCopyIt!=m_container.end(); ++tailCopyIt)
                tailVec.emplace_back(*tailCopyIt);

            for(; eraseRestSize!=m_container.size(); )                   // Удалили из контейнера то, что нужно удалить, а также хвост.
                m_container.pop_back();

            // Присовываем в контейнер хвост, который надо присунуть
            auto it = tailVec.begin();
            for(; it!=tailVec.end(); ++it)
               m_container.emplace_back(*it);
        }
    }

    template<typename IterType>
    void containerEraseImpl(IterType it)
    {
        auto itE = it;
        ++itE;
        containerEraseImpl(it, itE);
    }


    template<typename IterType>
    iterator eraseImpl(IterType pos)
    {
        auto mIt = m_map.find(pos->first);
        if (mIt==m_map.end())
            return m_container.end();
        std::size_t idx = mIt->second; // idx элемента, который встанет на место удалённого, будет равен удалённому

        // !!! Надо вычислить distance для pos и сравнить с idx (assert)

        iterator    cIt = m_container.begin();
        advance(cIt, difference_type(idx));
        m_map.erase(mIt);

        containerEraseImpl(cIt);
        cIt = m_container.begin(); // но итератор надо пересчитать
        advance(cIt, difference_type(idx));

        renumerateIndexes(cIt, idx, m_container.size());
        return cIt;
    }

    //template<typename ConvertibleToKey>
    iterator eraseImpl(const key_type &k)
    {
        auto mIt = m_map.find(k);
        if (mIt==m_map.end())
            return m_container.end();
        std::size_t idx = mIt->second; // idx нового элемента, который встанет на место удалённого, будет равен удалённому
        iterator    cIt = m_container.begin();
        advance(cIt, difference_type(idx));
        m_map.erase(mIt);

        containerEraseImpl(cIt);
        cIt = m_container.begin(); // но итератор надо пересчитать
        advance(cIt, difference_type(idx));

        renumerateIndexes(cIt, idx, m_container.size());
        return cIt;
    }


public:

    std::pair<iterator, bool> insert(const value_type& value)
    {
        auto mIt = m_map.find(value.first);
        if (mIt!=m_map.end())
        {
            // Обновление существующего элемента
            if constexpr (updateStrategy==UpdateStrategy::updateRestrict)
                throw update_error("marty::containers::insertion_ordered_map::insert: updating forbidden due UpdateStrategy::updateRestrict option");

            else
            {
                auto cIt = m_container.begin();
                advance(cIt, difference_type(mIt->second));
    
                if constexpr (updateStrategy==UpdateStrategy::updateInplace)
                {
                    cIt->second = value.second;
                    return std::make_pair(cIt, false);
                }
    
                if constexpr (updateStrategy==UpdateStrategy::updateChangeOrder)
                {
                    erase(cIt);
                    return insert(value); // Повторяем попытку вставки после удаления
                }
            }

        }
        else
        {
            // Добавление нового элемента
            std::size_t idx = m_container.size();
            m_map[value.first] = idx;
            m_container.emplace_back(value);
            auto cIt = m_container.begin();
            advance(cIt, difference_type(idx));
            return std::make_pair(cIt, true);
        }
    }

    std::pair<iterator, bool> insert(value_type&& value)
    {
        auto mIt = m_map.find(value.first);
        if (mIt!=m_map.end())
        {
            // Обновление существующего элемента
            if constexpr (updateStrategy==UpdateStrategy::updateRestrict)
                throw update_error("marty::containers::insertion_ordered_map::insert: updating forbidden due UpdateStrategy::updateRestrict option");

            else
            {
                auto cIt = m_container.begin();
                advance(cIt, difference_type(mIt->second));
    
                if constexpr (updateStrategy==UpdateStrategy::updateInplace)
                {
                    cIt->second = value.second;
                    return std::make_pair(cIt, false);
                }
    
                if constexpr (updateStrategy==UpdateStrategy::updateChangeOrder)
                {
                    erase(cIt);
                    return insert(std::move(value)); // Повторяем попытку вставки после удаления
                }
            }

        }
        else
        {
            // Добавление нового элемента
            std::size_t idx = m_container.size();
            m_map[value.first] = idx;
            m_container.emplace_back(value);
            auto cIt = m_container.begin();
            advance(cIt, difference_type(idx));
            return std::make_pair(cIt, true);
        }
    }

    // template< class P > std::pair<iterator, bool> insert( P&& value );

    template< class InputIt >
    void insert( InputIt first, InputIt last )
    {
        for(; first!=last; ++first)
            insert(*first);
    }

    void insert( std::initializer_list<value_type> ilist )
    {
        insert(ilist.begin(), ilist.end());
    }

    std::pair<iterator, bool> emplace(const key_type& key, T&& t)
    {
        return insert(value_type(key, t));
    }

    std::pair<iterator, bool> emplace(const value_type& value)
    {
        return insert(value);
    }

    std::pair<iterator, bool> emplace(value_type&& value)
    {
        return insert(std::forward(value));
    }

    // try_emplace


    iterator erase( iterator pos )
    {
        return eraseImpl(pos);
    }

    iterator erase(const_iterator pos)
    {
        return eraseImpl(pos);
    }

    iterator erase( const_iterator first, const_iterator last )
    {
        std::size_t firstIdx = distance(m_container.begin(), first);

        // Удалям ключи
        for(auto it=first; it!=last; ++it)
        {
            auto mIt = m_map.find(it->first);
            if (mIt!=m_map.end())
                m_map.erase(mIt);
        }

        containerEraseImpl(first, last);
        auto cIt = m_container.begin();
        advance(cIt, difference_type(firstIdx));
        renumerateIndexes(cIt, firstIdx, m_container.size());
        return cIt;
    }

    template<typename ConvertibleToKey>
    auto erase(const ConvertibleToKey &k) -> std::enable_if_t< std::is_convertible_v< ConvertibleToKey, key_type >, iterator >
    {
        return eraseImpl(key_type(k));
    }

    template<typename ConvertibleToKey>
    auto erase(ConvertibleToKey &&k) -> std::enable_if_t< std::is_convertible_v< ConvertibleToKey, key_type >, iterator >
    {
        return eraseImpl(key_type(std::forward<ConvertibleToKey>(k)));
    }


    size_type size() const { return m_container.size (); }
    bool     empty() const { return m_container.empty(); }
    void     clear()       { m_container.clear(); m_map.clear(); }

    //------------------------------
    // Доступ по индексу
    // Работает, как для вектора - если индекс за пределами - исключение, или быть беде
    //------------------------------
    const mapped_type& at(size_type idx) const
    {
        if (idx>=size())
            throw std::out_of_range("marty::containers::insertion_ordered_map::at: index is out of range");

        auto cIt = m_container.begin();
        advance(cIt, difference_type(idx));

        // Тут у нас нет проблем, так как возвращается константная ссылка
        return m_container[idx].second;
    }

    const mapped_type& operator[](size_type idx) const
    {
        // Тут у нас нет проблем, так как возвращается константная ссылка
        return m_container[idx].second;
    }

    // Неконстантные по индексу не добавляют ничего
    // at - кидает исключение
    // operator[] - молча делает бяку
    // Позволяют обновлять значение, в зависимости от заданной стратегии
    UpdateProxy at(size_type idx)
    {
        if (idx>=size())
            throw std::out_of_range("marty::containers::insertion_ordered_map::at: index is out of range");

        auto cIt = m_container.begin();
        advance(cIt, difference_type(idx));
        return UpdateProxy(this, cIt);
    }

    UpdateProxy operator[](size_type idx)
    {
        auto cIt = m_container.begin();
        advance(cIt, difference_type(idx));
        return UpdateProxy(this, cIt);
    }

    //------------------------------
    // Доступ по ключу
    // Работает, как для std::*map
    //------------------------------
    const mapped_type& at(const key_type &k) const
    {
        auto mIt = m_map.find(k);
        if (mIt==m_map.end())
            throw std::out_of_range("marty::containers::insertion_ordered_map::at: key not found");
    
        auto cIt = m_container.begin();
        advance(cIt, difference_type(mIt->second));
        // Тут у нас нет проблем, так как возвращается константная ссылка
        return cIt->second;
    }

    // Константная версия operator[] не предусмотрена интерфейсом map
    // const mapped_type& operator[](const key_type& k) const
    // {
    //     auto mIt = m_map.find(k);
    //     if (mIt!=m_map.end())
    //     {
    //         auto cIt = m_container.begin();
    //         advance(cIt, difference_type(mIt->second));
    //         // Тут у нас нет проблем, так как возвращается константная ссылка
    //         return cIt->second;
    //     }
    //  
    //     throw std::out_of_range("marty::containers::insertion_ordered_map::operator[]: can't add value to const container");
    //     //return at(k);
    // }

// std::pair<iterator, bool> insert(const value_type& value)
// insertion_ordered_map_type


    UpdateProxy at(const key_type &k)
    {
        auto mIt = m_map.find(k);
        if (mIt==m_map.end())
            throw std::out_of_range("marty::containers::insertion_ordered_map::at: key not found");
    
        auto cIt = m_container.begin();
        advance(cIt, difference_type(mIt->second));
        return UpdateProxy(this, cIt);
    }

    UpdateProxy operator[](const key_type& k)
    {
        auto mIt = m_map.find(k);
        if (mIt!=m_map.end())
        {
            auto idx = mIt->second;
            auto cIt = m_container.begin();
            advance(cIt, difference_type(idx));
            return UpdateProxy(this, cIt); // Используется стратегия обновления
        }

        auto ibPair = insert(value_type{k, mapped_type{}}); // Вставляем значение по умолчанию для типа mapped_type
        return UpdateProxy(this, ibPair.first, true); // дефолтное вставленное значение можно обновить вне зависимости от стратегии обновления
    }
    //------------------------------


    bool contains( const key_type& k ) const
    {
        return m_map.find(k)!=m_map.end();
    }

    iterator find(const Key& key)
    {
        auto mIt = m_map.find(key);
        if (mIt==m_map.end())
            return m_container.end();

        auto cIt = m_container.begin();
        advance(cIt, difference_type(mIt->second));
        return cIt;
    }

    const_iterator find(const Key& key) const
    {
        auto mIt = m_map.find(key);
        if (mIt==m_map.end())
            return m_container.end();

        auto cIt = m_container.begin();
        advance(cIt, difference_type(mIt->second));
        return cIt;
    }

    iterator        begin()       { return m_container.begin (); }
    iterator        end  ()       { return m_container.end   (); }
    const_iterator  begin() const { return m_container.begin (); }
    const_iterator  end  () const { return m_container.end   (); }
    const_iterator cbegin() const { return m_container.cbegin(); }
    const_iterator cend  () const { return m_container.cend  (); }

    reverse_iterator        rbegin()       { return m_container.rbegin (); }
    reverse_iterator        rend  ()       { return m_container.rend   (); }
    const_reverse_iterator  rbegin() const { return m_container.rbegin (); }
    const_reverse_iterator  rend  () const { return m_container.rend   (); }
    const_reverse_iterator crbegin() const { return m_container.crbegin(); }
    const_reverse_iterator crend  () const { return m_container.crend  (); }


}; // class insertion_ordered_map

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

} // namespace contyainers
} // namespace marty


