/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2014-2026 Alexander Martynov
    \brief Trie (префиксное дерево) implementation. Compatible with std::map in general cases

    Repository: https://github.com/al-martyn1/marty_containers
*/

#pragma once

#ifndef MARTY_TRIE_H
#define MARTY_TRIE_H

/* add this lines to your scr
#ifndef MARTY_TRIE_H
    #include <marty/trie.h>
#endif
*/

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4100) // warning C4100: 'pt': unreferenced formal parameter
    #pragma warning(disable:4189) // warning C4189: 'lastNodeIdx': local variable is initialized but not referenced
    #pragma warning(disable:4365) // warning C4365: 'argument': conversion from 'unsigned __int64' to 'const __int64', signed/unsigned mismatch
#endif


#ifndef MARTY_ADT_TRIE_IMPL_ASSERT
    #ifdef CLIASSERT
        #define MARTY_ADT_TRIE_IMPL_ASSERT(expr)    CLIASSERT(expr)
    #endif
#endif


#if !defined(_VECTOR_) && !defined(_STLP_VECTOR) && !defined(__STD_VECTOR__) && !defined(_CPP_VECTOR) && !defined(_GLIBCXX_VECTOR)
    #include <vector>
#endif

#if !defined(_STRING_) && !defined(_STLP_STRING) && !defined(__STD_STRING) && !defined(_CPP_STRING) && !defined(_GLIBCXX_STRING)
    #include <string>
#endif

#if !defined(_ALGORITHM_) && !defined(_STLP_ALGORITHM) && !defined(__STD_ALGORITHM) && !defined(_CPP_ALGORITHM) && !defined(_GLIBCXX_ALGORITHM)
    #include <algorithm>
#endif

#if !defined(_UTILITY_) && !defined(_STLP_UTILITY) && !defined(__STD_UTILITY__) && !defined(_CPP_UTILITY) && !defined(_GLIBCXX_UTILITY)
    #include <utility>
#endif


#ifndef MARTY_ADT_TRIE_IMPL_ASSERT
    #ifdef BOOST_ASSERT
        #define MARTY_ADT_TRIE_IMPL_ASSERT(expr)    BOOST_ASSERT(expr)
    #endif
#endif

#ifndef MARTY_ADT_TRIE_IMPL_ASSERT
    #define MARTY_ADT_TRIE_IMPL_ASSERT(expr)    assert(expr)
#endif



#define MARTY_ADT_TRIE_ITERATOR_RESERVE_MAGIC_NUMBER 256



namespace marty
{
// Abstract data types
namespace containers
{


template<typename F, typename S>
struct ref_pair
{   
    F& first; S& second;
    ref_pair( F& f, S& s) : first(f), second(s) { }
    ref_pair( const ref_pair &rp) : first(rp.first), second(rp.second) { }

    template<typename F2, typename S2>
    ref_pair( const ref_pair<F2,S2> &rp ) : first(rp.first), second(rp.second) { }

    operator std::pair<F,S>( ) const
    { return std::pair<F,S>(first,second); }

    void swap( ref_pair<F,S> &rp ) { std::swap(first,rp.first); std::swap(second,rp.second); }
};

template<class T>
struct boxed_ptr
{
  mutable T data;
  boxed_ptr(const T & d) : data(d) {}
  boxed_ptr(const T * pd) : data(*pd) {}
  boxed_ptr( const boxed_ptr &b) : data(b.data) {}
  T& operator * () const { return  data; }
  T* operator ->() const { return &data; }
};

template<class F, class S>
struct ptr_pair
{
  F* pf; S* ps;
  ptr_pair(F* pf, S* ps) : pf(pf), ps(ps) {}
  boxed_ptr< ref_pair<F,S> > operator -> () const { return boxed_ptr< ref_pair<F,S> >( ref_pair<F,S>(*pf,*ps) ); }
  ref_pair<F,S>              operator *  () const { return ref_pair<F,S>(*pf,*ps); }
};



template < typename TrieType >
class trie_const_iterator_impl;

template < typename TrieType >
class trie_iterator_impl;

template < typename TrieType
         , typename TrieKeyTypeContainer
         >
class trie_map_iterator_base_impl;


template < typename TrieType
         , typename TrieKeyTypeContainer
         >
class trie_map_const_iterator_impl;

template < typename TrieType
         , typename TrieKeyTypeContainer
         >
class trie_map_iterator_impl;



template < typename TrieType
         , typename T
         >
class trie_iterator_base_impl;

template < typename KeyType
         , typename ValueType
         , typename Traits
         >
class trie_map;


template<typename T>
class trie_inspector;



template < typename KeyType
         , typename ValueType
         , typename Traits = std::less< KeyType >
         >
class trie
{

public:

    // Type Declarations 

    typedef KeyType       key_type;
    typedef ValueType     mapped_type;
    typedef Traits        key_compare;

    typedef std::size_t   size_type;

    typedef KeyType                                             value_type;
    typedef std::ptrdiff_t                                      difference_type;

    friend class trie_const_iterator_impl< trie<key_type,mapped_type,key_compare> >;
    friend class trie_iterator_impl< trie<key_type,mapped_type,key_compare> >;

    //template<class T> friend class trie_map_iterator_impl< trie, T >;
    template < typename TrieType, typename T> // !!!
    friend class trie_iterator_base_impl;

    template < typename TrieType, typename TrieKeyTypeContainer >
    friend class trie_map_iterator_base_impl;

    template < typename MapKeyType
             , typename MapValueType
             , typename MapTraits
             >
    friend class trie_map; // !!!

    template<typename U>
    friend class trie_inspector;

    typedef trie_const_iterator_impl< trie<key_type,mapped_type,key_compare> >   const_iterator;
    typedef trie_iterator_impl< trie<key_type,mapped_type,key_compare> >         iterator;

    typedef std::reverse_iterator<iterator>                     reverse_iterator;
    typedef std::reverse_iterator<const_iterator>               const_reverse_iterator;

    struct  trie_node_data_item;
    struct  trie_node;

    friend struct trie_node;

    typedef std::vector< trie_node_data_item >             trie_node_data_item_holder;
    typedef typename trie_node_data_item_holder::size_type trie_node_data_item_index;
    const static trie_node_data_item_index                 trie_node_data_item_index_npos = static_cast<trie_node_data_item_index>(-1);
    
    typedef std::vector< trie_node >                     trie_nodes_holder;
    typedef typename trie_nodes_holder::size_type        trie_node_index;
    const static trie_node_index                         trie_node_index_npos  = static_cast<trie_node_index>(-1);

    typedef std::vector< mapped_type >                   values_holder;
    
    typedef typename values_holder::size_type            value_index;
    const static value_index                             value_index_npos      = static_cast<value_index>(-1);

    //typedef std::stack< value_index    , std::vector<value_index> >     value_free_index_holder;
    //typedef std::stack< trie_node_index, std::vector<trie_node_index> > trie_node_free_index_holder;
    typedef std::vector< value_index     >     value_free_index_holder;
    typedef std::vector< trie_node_index > trie_node_free_index_holder;



    struct trie_node_data_item
    {
        key_type               key;
        trie_node_index        child_idx;
        value_index            value_idx;
        trie_node_data_item(const key_type &k = key_type()
                      , trie_node_index chidx = trie_node_index_npos
                      , value_index vidx = value_index_npos)
            : key(k), child_idx(chidx), value_idx(vidx)
            {}
    };

    struct trie_node_data_item_comparator
    {
         key_compare comparator;
         trie_node_data_item_comparator(key_compare c) : comparator(c) {}
         bool operator()( const trie_node_data_item &l, const trie_node_data_item& r) const
             { return comparator(l.key,r.key); }
         bool operator()( const key_type &l, const trie_node_data_item& r) const
             { return comparator(l,r.key); }
         bool operator()( const trie_node_data_item &l, const key_type& r) const
             { return comparator(l.key,r); }
    };

    struct trie_position
    {
         trie_node_index           node_idx;
         trie_node_data_item_index item_idx;
         trie_position( trie_node_index n = trie_node_index_npos
                , trie_node_data_item_index i = trie_node_data_item_index_npos )
                : node_idx(n), item_idx(i) {}
         bool operator==( const trie_position &tp) const
             { return node_idx==tp.node_idx && item_idx==tp.item_idx; }
         bool operator!=( const trie_position &tp) const
             { return node_idx!=tp.node_idx || item_idx!=tp.item_idx; }
    }; // struct trie_position

    struct trie_node
    {
        typedef class trie<KeyType,ValueType,Traits> trie_type;

        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        trie_node_data_item_index    first_item;
        trie_node_data_item_index    size;
        //trie_node_index              parent_idx;
        trie_node( trie_node_data_item_index fi = trie_node_index_npos, trie_node_data_item_index s = 0) 
           : first_item(fi), size(s) /* , parent_idx(pi) */  {}
        trie_node( trie_node_data_item_index fi, trie_node_data_item_index s, trie_node_index pi) 
           : first_item(fi), size(s) /* , parent_idx(pi) */  {}
        #else
        trie_node_data_item_holder    data_items;
        trie_node() : data_items() { }
        #endif

        void reserve( size_t s )
            {
             #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
             data_items.reserve(s);
             #endif
            }

        void clear()
           {
            trie_node_data_item_holder tmp;
            data_items.swap(tmp);
           }

        trie_node_data_item_index keys_size() const
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            return size;
            #else
            return data_items.size();
            #endif
           }

        trie_node_data_item& get_data_item( trie_type *pt, trie_node_data_item_index idx )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            MARTY_ADT_TRIE_IMPL_ASSERT( idx<size && "node data index (idx) out of range" );
            idx += first_item;
            MARTY_ADT_TRIE_IMPL_ASSERT( idx<trie_node_data_items.size() && "node data index (first_item) out of range" );
            return pt->trie_node_data_items[idx];
            #else
            MARTY_ADT_TRIE_IMPL_ASSERT( idx<data_items.size() && "node data index (idx) out of range" );
            return data_items[idx];
            #endif
           }

        const trie_node_data_item& get_data_item( const trie_type *pt, trie_node_data_item_index idx ) const
           {
            return const_cast<trie_node*>(this)->get_data_item( const_cast<trie_type*>(pt), idx );
           }

        typename trie_node_data_item_holder::iterator find_key_impl( trie_type *pt, const key_type &k, bool &bFound /* else return insert pos */ )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            MARTY_ADT_TRIE_IMPL_ASSERT( (first_item)<pt->trie_node_data_items.size() && "node data index (first_item) out of range" );
            MARTY_ADT_TRIE_IMPL_ASSERT( (first_item+size)<pt->trie_node_data_items.size() && "node data index (size) out of range" );

            typename trie_node_data_item_holder::iterator rangeBegin
                     = pt->trie_node_data_items.begin() + first_item;
            typename trie_node_data_item_holder::iterator rangeEnd
                     = rangeBegin + size;

            bFound = false;

            typename trie_node_data_item_holder::iterator foundIt = 
                   ::std::lower_bound( rangeBegin, rangeEnd
                                     , k, trie_node_data_item_comparator( pt->comparator )
                                     );
            if (foundIt!=rangeEnd)
               {
                MARTY_ADT_TRIE_IMPL_ASSERT(!(pt->comparator(k,foundIt->key) && pt->comparator(foundIt->key,k)) && "invalid order relation");
                if (pt->comparator(k,foundIt->key) == pt->comparator(foundIt->key,k))
                    bFound = true;
               }
            return foundIt;
            #else
            bFound = false;

            typename trie_node_data_item_holder::iterator foundIt = 
                   ::std::lower_bound( data_items.begin(), data_items.end()
                                     , k, trie_node_data_item_comparator( pt->comparator )
                                     );
            if (foundIt!=data_items.end())
               {
                MARTY_ADT_TRIE_IMPL_ASSERT(!(pt->comparator(k,foundIt->key) && pt->comparator(foundIt->key,k)) && "invalid order relation");
                if (pt->comparator(k,foundIt->key) == pt->comparator(foundIt->key,k))
                    bFound = true;
               }
            return foundIt;
            #endif
           }

        typename trie_node_data_item_holder::iterator find_key( trie_type *pt, const key_type &k, bool &bFound /* else return insert pos */ )
           {
            return find_key_impl(pt, k, bFound);
           }


        typename trie_node_data_item_holder::const_iterator find_key( const trie_type *pt, const key_type &k, bool &bFound /* else return insert pos */ ) const
           {
            return const_cast<trie_node*>(this)->find_key_impl( const_cast<trie_type*>(pt), k, bFound );
           }

        trie_node_data_item_index find_key_idx( const trie_type *pt, const key_type &k ) const
           {
            bool bFound = false;
            typename trie_node_data_item_holder::const_iterator fit = find_key( pt, k, bFound );
            if (bFound)
               {
                #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
                typename trie_node_data_item_holder::const_iterator rangeBegin
                         = pt->trie_node_data_items.begin() + first_item;
                return (trie_node_data_item_index)(fit - rangeBegin);
                #else
                return (trie_node_data_item_index)(fit - data_items.begin());
                #endif
               }

            #if 0
            MARTY_ADT_TRIE_IMPL_ASSERT( (first_item)<pt->trie_node_data_items.size() && "node data index (first_item) out of range" );
            MARTY_ADT_TRIE_IMPL_ASSERT( (first_item+size)<pt->trie_node_data_items.size() && "node data index (size) out of range" );

            typename trie_node_data_item_holder::const_iterator rangeBegin
                     = pt->trie_node_data_items.begin() + first_item;
            typename trie_node_data_item_holder::const_iterator rangeEnd
                     = rangeBegin + size;

            typename trie_node_data_item_holder::const_iterator foundIt = 
                   ::std::lower_bound( rangeBegin, rangeEnd
                                     , k, trie_node_data_item_comparator( pt->comparator )
                                     );
            if (foundIt!=rangeEnd)
               {
                MARTY_ADT_TRIE_IMPL_ASSERT(!(pt->comparator(k,foundIt->key) && pt->comparator(foundIt->key,k)) && "invalid order relation");
                if (pt->comparator(k,foundIt->key) == pt->comparator(foundIt->key,k))
                   return (trie_node_data_item_index)(foundIt - rangeBegin);
               }
            #endif

            return trie_node_data_item_index_npos;
           }

        trie_node_data_item_index nodeDataIteratorToGlobalIndex( trie_type *pt, typename trie_node_data_item_holder::iterator it)
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            return (trie_node_data_item_index)(it - pt->trie_node_data_items.begin());
            #else
            MARTY_ADT_TRIE_IMPL_ASSERT(0 && "nodeDataIteratorToGlobalIndex not implemented");
            #endif
           }

        trie_node_data_item_index nodeDataIteratorToLocalIndex( trie_type *pt, typename trie_node_data_item_holder::iterator it)
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            return (trie_node_data_item_index)(it - pt->trie_node_data_items.begin() - first_item);
            #else
            return (trie_node_data_item_index)(it - data_items.begin());
            #endif
           }

        trie_node_data_item_index nodeDataIteratorToGlobalIndex( const trie_type *pt, typename trie_node_data_item_holder::const_iterator it) const
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            return (trie_node_data_item_index)(it - pt->trie_node_data_items.begin());
            #else
            MARTY_ADT_TRIE_IMPL_ASSERT(0 && "nodeDataIteratorToGlobalIndex not implemented");
            #endif
           }

        trie_node_data_item_index nodeDataIteratorToLocalIndex( const trie_type *pt, typename trie_node_data_item_holder::const_iterator it) const
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            return (trie_node_data_item_index)(it - pt->trie_node_data_items.begin() - first_item);
            #else
            return (trie_node_data_item_index)(it - data_items.begin());
            #endif
           }

        void insert_data_item( trie_type *pt, typename trie_node_data_item_holder::iterator pos, const trie_node_data_item &i)
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            pt->trie_node_data_items.insert( pos, i );
            ++size;
            #else
            //if (data_items.capacity()<4) data_items.reserve(4);
            data_items.insert( pos, i );
            #endif
           }

        void insert_data_item( trie_type *pt, typename trie_node_data_item_holder::iterator pos, const key_type &k
                           , trie_node_index chidx = trie_node_index_npos
                           , value_index vidx = value_index_npos
                           )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            pt->trie_node_data_items.insert( pos, trie_node_data_item( k, chidx, vidx ) );
            ++size;
            #else
            //if (data_items.capacity()<4) data_items.reserve(4);
            data_items.insert( pos, trie_node_data_item( k, chidx, vidx ) );
            #endif
           }

        void insert_data_item( trie_type *pt, const key_type &k )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            if (first_item==trie_node_index_npos)
               {
                first_item = pt->trie_node_data_items.size();
                pt->trie_node_data_items.push_back( trie_node_data_item( k ) );
                size = 1;
               }
            else
               {
                bool bFound = false;
                typename trie_node_data_item_holder::iterator it = find_key( pt, k, bFound );
                if (!bFound) insert_data_item( pt, it, trie_node_data_item(k));
               }
            #else
            if (data_items.capacity()<pt->reserve_trie_node_data_items) data_items.reserve(pt->reserve_trie_node_data_items);
            if (data_items.empty())
                data_items.push_back( trie_node_data_item( k ) );
            else
               {
                bool bFound = false;
                typename trie_node_data_item_holder::iterator it = find_key( pt, k, bFound );
                if (!bFound) insert_data_item( pt, it, trie_node_data_item(k));
               }
            #endif
           }

        //trie_node_index_npos

        void remove_item_value( trie_type *pt, typename trie_node_data_item_holder::iterator pos )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            #else
            if (pos->value_idx!=value_index_npos)
               pt->remove_value_impl( pos->value_idx );
            pos->value_idx = value_index_npos;
            #endif
           }
           
        void remove_item_value( trie_type *pt, trie_node_data_item_index itemIdx )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<size && "node item index out of range" );
            MARTY_ADT_TRIE_IMPL_ASSERT( (itemIdx+first_item)<pt->trie_node_data_items.size() && "node data index (size) out of range" );
            itemIdx += first_item;
            if (pt->trie_node_data_items[itemIdx].value_idx!=value_index_npos)
               pt->remove_value_impl( pt->trie_node_data_items[itemIdx].value_idx );
            pt->trie_node_data_items[itemIdx].value_idx = value_index_npos;
            #else
            MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<data_items.size() && "node item index out of range" );
            if (data_items[itemIdx].value_idx!=value_index_npos)
               pt->remove_value_impl( data_items[itemIdx].value_idx );
            data_items[itemIdx].value_idx = value_index_npos;
            #endif
           }

        mapped_type &set_item_value( trie_type *pt, trie_node_data_item_index itemIdx, const mapped_type &val )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<size && "node item index out of range" );
            MARTY_ADT_TRIE_IMPL_ASSERT( (itemIdx+first_item)<pt->trie_node_data_items.size() && "node data index (size) out of range" );
            itemIdx += first_item;

            if (pt->trie_node_data_items[itemIdx].value_idx==value_index_npos)
               pt->trie_node_data_items[itemIdx].value_idx = pt->add_value_impl( val );
            else
               pt->values[pt->trie_node_data_items[itemIdx].value_idx] = val;
            return pt->values[pt->trie_node_data_items[itemIdx].value_idx];
            #else
            MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<data_items.size() && "node item index out of range" );
            if (data_items[itemIdx].value_idx==value_index_npos)
               data_items[itemIdx].value_idx = pt->add_value_impl( val );
            else
               pt->values[data_items[itemIdx].value_idx] = val;
            return pt->values[data_items[itemIdx].value_idx];
            #endif
           }

        void erase_key( trie_type *pt, typename trie_node_data_item_holder::iterator pos )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            #else
            if (pos==data_items.end()) return;
            remove_item_value( pt, pos );
            data_items.erase(pos);
            #endif
           }

        void erase_key( trie_type *pt, const key_type &k )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            #else
            bool bFound = false;
            typename trie_node_data_item_holder::iterator fit = find_key( pt, k, bFound );
            erase_key(pt,fit);
            #endif
           }
        
        void erase_key_by_index( trie_type *pt, trie_node_data_item_index itemIdx )
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            #else
            MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<data_items.size() && "node item index out of range" );
            erase_key(pt,data_items.begin()+itemIdx);
            #endif
           }
     
        bool is_key_payloaded( const trie_type *pt, typename trie_node_data_item_holder::const_iterator pos ) const
           {
            return pos->value_idx!=value_index_npos;
           }

        bool is_key_payloaded( const trie_type *pt, trie_node_data_item_index itemIdx ) const
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<size && "node item index out of range" );
            MARTY_ADT_TRIE_IMPL_ASSERT( (itemIdx+first_item)<pt->trie_node_data_items.size() && "node data index (size) out of range" );
            itemIdx += first_item;
            return pt->trie_node_data_items[itemIdx].value_idx!=value_index_npos;
            #else
            is_payloaded( pt, data_items.begin() + itemIdx );
            #endif
           }

        bool key_has_child( const trie_type *pt, typename trie_node_data_item_holder::const_iterator pos ) const
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            #else
            return pos->child_idx!=trie_node_index_npos;
            #endif
           }

        trie_node_index get_child_id( const trie_type *pt, trie_node_data_item_index itemIdx ) const
           {
            #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<size && "node item index out of range" );
            MARTY_ADT_TRIE_IMPL_ASSERT( (itemIdx+first_item)<pt->trie_node_data_items.size() && "node data index (size) out of range" );
            itemIdx += first_item;
            return pt->trie_node_data_items[itemIdx].child_idx;
            #else
            return data_items[itemIdx].child_idx;
            #endif
           }

        bool key_has_child( const trie_type *pt, trie_node_data_item_index itemIdx ) const
           {
            return get_child_id( pt, itemIdx )!=trie_node_index_npos;
           }

        bool is_keys_payloaded( const trie_type *pt ) const
           {
            trie_node_data_item_index idx = 0, s = keys_size();
            for(; idx!=s; ++idx)
               {
                if (!key_has_child(pt, idx)) continue;
                if (is_key_payloaded(pt, idx)) return true;
                pt->trie_nodes[get_child_id(idx)].is_keys_payloaded(pt) return true;
               }
            return false;
           }

    }; // struct trie_node



#ifndef TRIE_IMPL_DEBUG
protected:
#endif

    key_compare                   comparator;
    values_holder                 values;
    value_free_index_holder       value_free_indexes;
    trie_node_free_index_holder   trie_node_free_indexes;
    #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
    trie_node_data_item_holder    trie_node_data_items;
    #endif
    trie_nodes_holder             trie_nodes;
    size_type                     reserve_trie_node_data_items;


    // Public utility functions

    value_index add_value_impl( const mapped_type &v)
    {
        if (value_free_indexes.empty())
           {
            value_index res = values.size();
            values.push_back(v);
            return res;
           }
        value_index res = value_free_indexes.back();
        MARTY_ADT_TRIE_IMPL_ASSERT( res<values.size() && "value index out of range" );
        value_free_indexes.pop_back();
        values[res] = v;
        return res;
    }

    void remove_value_impl( value_index i)
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( i<values.size() && "value index out of range" );
        if (i==(values.size()-1))
           { // last value
            values.erase( values.begin()+i );
           }
        else
           {
            values[i] = mapped_type(); // set to default value
            value_free_indexes.push_back(i);
           }
    }

    // End of Public utility functions

    trie_node_index add_trie_node_impl( const trie_node &n)
    {
        if (trie_node_free_indexes.empty())
           {
            trie_node_index res = trie_nodes.size();
            trie_nodes.push_back(n);
            trie_nodes[res].reserve(reserve_trie_node_data_items);
            return res;
           }
        trie_node_index res = trie_node_free_indexes.back();
        MARTY_ADT_TRIE_IMPL_ASSERT( res<trie_nodes.size() && "node index out of range" );
        trie_node_free_indexes.pop_back();
        trie_nodes[res] = n;
        trie_nodes[res].reserve(reserve_trie_node_data_items);
        return res;
    }

    // simple remove node, node data and values keep untouched
    void remove_node_impl( trie_node_index n )
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( n<trie_nodes.size() && "node index out of range" );
        MARTY_ADT_TRIE_IMPL_ASSERT( trie_nodes[n].keys_size()!=0 && "node allready removed" );
        if (n==(trie_nodes.size()-1))
           {
            trie_nodes.erase(trie_nodes.begin()+n);
           }
        else
           {
            trie_nodes[n].clear(); // = trie_node( ) ; // 0, 0 pos and size == 0 marks node as removed
            trie_node_free_indexes.push_back(n);
           }
    }

    void remove_node_value( trie_node_index n, trie_node_data_item_index itemIdx )
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( n<trie_nodes.size() && "node index out of range" );
        MARTY_ADT_TRIE_IMPL_ASSERT( trie_nodes[n].size!=0 && "node allready removed" );
        trie_nodes[n].remove_item_value( this, itemIdx );
    }

    mapped_type &set_node_value( trie_node_index n, trie_node_data_item_index itemIdx, const mapped_type &val )
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( n<trie_nodes.size() && "node index out of range" );
        MARTY_ADT_TRIE_IMPL_ASSERT( trie_nodes[n].keys_size()!=0 && "node allready removed" );
        return trie_nodes[n].set_item_value( this, itemIdx, val );
    }

    void remove_data_item( trie_node_data_item_index dataItemIdx, trie_node_index nodeFromIdx /* node from wich remove */)
    {
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        // remove item value
        if (trie_node_data_items[dataItemIdx].value_idx!=value_index_npos)
           remove_value_impl( trie_node_data_items[dataItemIdx].value_idx );
        // remove item
        trie_node_data_items.erase( trie_node_data_items.begin() + dataItemIdx );
        // adjust node indexes - first_item
        typename trie_nodes_holder::iterator tnIt = trie_nodes.begin();
        for(; tnIt != trie_nodes.end(); ++tnIt)
           {
            if (!tnIt->size) continue;
            if ((tnIt - trie_nodes.begin())==nodeFromIdx) continue;
            if (tnIt->first_item >= dataItemIdx) tnIt->first_item--;
           }
        #else
        MARTY_ADT_TRIE_IMPL_ASSERT( nodeFromIdx<trie_nodes.size() && "node index out of range" );
        //trie_nodes[nodeFromIdx].remove_item_value( pt, dataItemIdx );
        trie_nodes[nodeFromIdx].erase_key_by_index( pt, dataItemIdx );
        #endif
    }

    bool is_node_item_or_childs_payloaded( trie_node_index n, trie_node_data_item_index itemIdx ) const
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( n<trie_nodes.size() && "node index out of range" );

        if (trie_nodes[n].is_key_payloaded( this, itemIdx )) return true;
        if (!trie_nodes[n].key_has_child( this, itemIdx ))   return false;

        trie_node_index childId = trie_nodes[n].get_child_id(this, itemIdx);
        MARTY_ADT_TRIE_IMPL_ASSERT( childId<trie_nodes.size() && "node index out of range" );
        return trie_nodes[childId].is_keys_payloaded( this );

        #if 0
        MARTY_ADT_TRIE_IMPL_ASSERT( n<trie_nodes.size() && "node index out of range" );
        MARTY_ADT_TRIE_IMPL_ASSERT( trie_nodes[n].size!=0 && "node allready removed" );
        MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<trie_nodes[n].size && "node item index out of range" );

        trie_node_data_item_index dataItemIdx = trie_nodes[n].first_item + itemIdx;
        if (trie_node_data_items[dataItemIdx].value_idx!=value_index_npos)
           return true;

        trie_node_index childIdx = trie_node_data_items[dataItemIdx].child_idx;

        trie_node_data_item_index chDataIdx = 0;
        for(; chDataIdx!=trie_nodes[childIdx].size; ++chDataIdx)
           {
            if (is_node_item_or_childs_payloaded( childIdx, chDataIdx ))
               return true;
           }
        return false;
        #endif
    }

    void remove_node_item( trie_node_index n, trie_node_data_item_index itemIdx )
    {
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        MARTY_ADT_TRIE_IMPL_ASSERT( n<trie_nodes.size() && "node index out of range" );
        MARTY_ADT_TRIE_IMPL_ASSERT( trie_nodes[n].size!=0 && "node allready removed" );
        MARTY_ADT_TRIE_IMPL_ASSERT( itemIdx<trie_nodes[n].size && "node item index out of range" );

        trie_node_data_item_index dataItemIdx = trie_nodes[n].first_item + itemIdx;
        trie_node_index childIdx = trie_node_data_items[dataItemIdx].child_idx;
        if (childIdx!=trie_node_index_npos) // has child?
           { 
            // remove child items
            while( trie_nodes[childIdx].size )
               {
                remove_node_item( childIdx, 0 );
               }
            // remove child itself
            trie_nodes[n].clear(); //  = trie_node( 0, 0 );
            trie_node_free_indexes.push(n);
           }

        remove_data_item( trie_nodes[n].first_item + itemIdx, n );
        #else
        MARTY_ADT_TRIE_IMPL_ASSERT( n<trie_nodes.size() && "node index out of range" );
        if (trie_nodes[n].key_has_child( this, itemIdx ))
           {
            trie_node_index childId = trie_nodes[n].get_child_id(this, itemIdx);
            MARTY_ADT_TRIE_IMPL_ASSERT( childId<trie_nodes.size() && "node index out of range" );
            while( trie_nodes[childId].keys_size() )
               {
                remove_node_item( childId, 0 );
               }
            // remove child itself
            trie_nodes[n].clear();// = trie_node( );
            trie_node_free_indexes.push(n);
           }
        trie_nodes[n].erase_key_by_index(this, itemIdx);
        #endif
    }

public:

    key_compare key_comp( ) const
    {
        return comparator;
    }

    trie()
        : comparator()
        , values()
        , value_free_indexes()
        , trie_node_free_indexes()
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        , trie_node_data_items()
        #endif
        , trie_nodes()
        , reserve_trie_node_data_items(1)
        {}

    trie(const Traits &t)
        : comparator(t)
        , values()
        , value_free_indexes()
        , trie_node_free_indexes()
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        , trie_node_data_items()
        #endif
        , trie_nodes()
        , reserve_trie_node_data_items(1)
        {}

    trie( const trie &t)
        : comparator(t.comparator)
        , values(t.values)
        , value_free_indexes(t.value_free_indexes)
        , trie_node_free_indexes(t.trie_node_free_indexes)
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        , trie_node_data_items(t.trie_node_data_items)
        #endif
        , trie_nodes(t.trie_nodes)
        , reserve_trie_node_data_items(t.reserve_trie_node_data_items)
        {}

    const_iterator begin() const;
    iterator begin();

    const_iterator end() const;
    iterator end();

    reverse_iterator rbegin()             { return (reverse_iterator(end())); }
    const_reverse_iterator rbegin() const { return (const_reverse_iterator(end())); }
    reverse_iterator rend()               { return (reverse_iterator(begin())); }
    const_reverse_iterator rend() const   { return (const_reverse_iterator(begin())); }

    void clear() { clear_impl(); }
    bool empty() const
    {
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
            return (trie_nodes.empty() || trie_nodes[0].size==0);
        #else
            return (trie_nodes.empty());
        #endif
    }

    void swap( trie &t)
    {
        std::swap(comparator, t.comparator);
        values                 .swap(t.values                );
        value_free_indexes     .swap(t.value_free_indexes    );
        trie_node_free_indexes .swap(t.trie_node_free_indexes);
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        trie_node_data_items   .swap(t.trie_node_data_items  );
        #endif
        trie_nodes             .swap(t.trie_nodes            );
    }

    // reserve mem for s values
    void reserve( size_type s, size_type ri = 1)
    {
        values              .reserve(   s );
        trie_nodes          .reserve( 2*s );
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        trie_node_data_items.reserve( 8*s );
        #endif
        reserve_trie_node_data_items = ri;
    }

    bool next( const_iterator &it, const key_type &k ) const;
    bool next( iterator &it, const key_type &k ) const;


    template<typename KeyIter>  const_iterator find(                          const KeyIter &b, const KeyIter &e ) const;
    template<typename KeyIter>  const_iterator find( const_iterator findFrom, const KeyIter &b, const KeyIter &e ) const;
    template<typename KeyIter>  iterator       find(                          const KeyIter &b, const KeyIter &e );
    template<typename KeyIter>  iterator       find(       iterator findFrom, const KeyIter &b, const KeyIter &e );


    const_iterator find(                          key_type k ) const;
    const_iterator find( const_iterator findFrom, key_type k ) const;
    iterator       find(                          key_type k );
    iterator       find(       iterator findFrom, key_type k );


    iterator insert( iterator where, const key_type &k );
    iterator insert( iterator where, const key_type &k, const mapped_type &v);

    template<typename KeyIter>
    iterator insert( const KeyIter &b, const KeyIter &e );

    template<typename KeyIter>
    iterator insert( iterator where, const KeyIter &b, const KeyIter &e );

    template<typename KeyIter>
    iterator insert( const KeyIter &b, const KeyIter &e, const mapped_type &v );

    template<typename KeyIter>
    iterator insert( iterator where, const KeyIter &b, const KeyIter &e, const mapped_type &v );

    iterator erase( iterator what );
    //iterator erase( iterator where, const key_type &k );

    //template<typename KeyIter>
    //void erase( const KeyIter &b, const KeyIter &e );

    bool is_payloaded( const const_iterator &i ) const;
    bool is_payloaded( const iterator &i );

    mapped_type& payload( iterator i, const mapped_type &v ); //!< Добавляем нагрузку
    mapped_type& payload( iterator i ); //!< Получаем ссылку на нагрузку
    const mapped_type& payload( const_iterator i ) const; //!< Получаем const ссылку на нагрузку
    void remove_payload( iterator i ); //!< Удаляем нагрузку

    size_type all_values_size() const { return values.size(); }
    mapped_type& value_at( size_type i) { return values[i]; }
    const mapped_type& value_at( size_type i) const { return values[i]; }

    const value_free_index_holder& get_value_free_indexes() const { return value_free_indexes; }

    size_type values_size() const
    {
        return values.size() - value_free_indexes.size();
    }

    size_type get_used_mem() const
    {
     #if !defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
     size_type nodesDataSize = 0;
     typename trie_nodes_holder::const_iterator tnIt = trie_nodes.begin();
     for(;tnIt != trie_nodes.end(); ++tnIt)
        {
         //if (tnIt->data_items.em)
         nodesDataSize += sizeof(tnIt->data_items) + tnIt->data_items.capacity() *sizeof(trie_node_data_item);
        }
     #endif

     return sizeof(values_holder)                + values.capacity()*sizeof(mapped_type)
          + sizeof(value_free_index_holder)      + value_free_indexes.capacity()*sizeof(value_index)
          + sizeof(trie_node_free_index_holder)  + trie_node_free_indexes.capacity()*sizeof(trie_node_index)
          #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
          + sizeof(trie_node_data_item_holder)   + trie_node_data_items.capacity()*sizeof(trie_node_data_item)
          #else
          + nodesDataSize
          #endif
          + sizeof(trie_nodes_holder)            + trie_nodes.capacity()*sizeof(trie_node)
          ;
    }




#ifndef TRIE_IMPL_DEBUG
protected:
#endif

    iterator non_const_iter_end() const;

    template<typename Iter>
    void construct_last( Iter &b, trie_node_index nodeIdx = trie_node_index_npos) const;

    trie_node_data_item_index find_or_insert_key( const key_type &k, trie_node_index nodeIdx )
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( nodeIdx!=trie_node_index_npos && "invalid node index" );
        MARTY_ADT_TRIE_IMPL_ASSERT( nodeIdx<trie_nodes.size() && trie_nodes[nodeIdx].keys_size()!=0 && "node index out of range" );
        trie_node &node = trie_nodes[nodeIdx];

        bool bFound = false;
        trie_node_data_item_holder::iterator foundIt = node.find_key( this, k, bFound );
        trie_node_data_item_index new_pos_index = node.nodeDataIteratorToLocalIndex(this,foundIt);
        if (bFound) 
           return new_pos_index;

        node.insert_data_item(this, foundIt, k );

        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        // adjust node indexes - first_item
        typename trie_nodes_holder::iterator tnIt = trie_nodes.begin();
        for(; tnIt != trie_nodes.end(); ++tnIt)
           {
            if (!tnIt->size) continue;
            if ((tnIt - trie_nodes.begin())==nodeIdx) continue;
            if (tnIt->first_item >= new_pos_index) tnIt->first_item++;
           }
        //node.size++;
        #endif
        return new_pos_index;
    }


    template<typename KeyIterator, typename TrieIterator>
    TrieIterator find_impl( KeyIterator keyBegin, KeyIterator keyEnd, TrieIterator where ) const
    {
        if (keyBegin==keyEnd)
           return non_const_iter_end();

        if (trie_nodes.empty() || !trie_nodes[0].keys_size())
           return non_const_iter_end();

        if (where.is_end_iter()) // find starts on trie root
           {
            bool bFound = false;
            trie_node_data_item_holder::const_iterator foundIt = trie_nodes[0].find_key( this, *keyBegin++, bFound );
            if (!bFound)
               return non_const_iter_end();

            where.push_pos( 0 , trie_nodes[0].nodeDataIteratorToLocalIndex(this,foundIt) );
           }
        for(; keyBegin!=keyEnd; ++keyBegin)
           {
            trie_node_index nextNodeIdx = where.get_node_data_item().child_idx;
            if (nextNodeIdx==trie_node_index_npos) // last pos points to the item without child
               return non_const_iter_end();

            bool bFound = false;
            trie_node_data_item_holder::const_iterator foundIt = trie_nodes[nextNodeIdx].find_key( this, *keyBegin, bFound );
            if (!bFound)
               return non_const_iter_end();
    
            trie_node_data_item_index nextItemIdx = trie_nodes[nextNodeIdx].nodeDataIteratorToLocalIndex(this,foundIt);

            where.push_pos( nextNodeIdx , nextItemIdx );
           }
        return where;
    }


    template<typename TrieIterator>
    TrieIterator find_impl( key_type keyVal, TrieIterator where ) const
    {
        if (trie_nodes.empty() || !trie_nodes[0].keys_size())
           return non_const_iter_end();

        if (where.is_end_iter()) // find starts on trie root
           {
            bool bFound = false;
            trie_node_data_item_holder::const_iterator foundIt = trie_nodes[0].find_key( this, keyVal, bFound );
            if (!bFound)
               return non_const_iter_end();

            where.push_pos( 0 , trie_nodes[0].nodeDataIteratorToLocalIndex(this,foundIt) );
           }
        else
           {
            trie_node_index nextNodeIdx = where.get_node_data_item().child_idx;
            if (nextNodeIdx==trie_node_index_npos) // last pos points to the item without child
               return non_const_iter_end();
            bool bFound = false;
            trie_node_data_item_holder::const_iterator foundIt = trie_nodes[nextNodeIdx].find_key( this, keyVal, bFound );
            if (!bFound)
               return non_const_iter_end();

            trie_node_data_item_index nextItemIdx = trie_nodes[nextNodeIdx].nodeDataIteratorToLocalIndex(this,foundIt);
            where.push_pos( nextNodeIdx , nextItemIdx );
           }

        return where;
    }


    void clear_impl()
    {
        values.clear();
        value_free_indexes.clear();
        trie_node_free_indexes.clear();
        #if defined(USE_MARTY_ADT_TRIE_SINGLE_DATA_ARRAY)
        trie_node_data_items.clear();
        #endif
        trie_nodes.clear();
    }


    template<typename TrieIterator>
    TrieIterator erase_impl( TrieIterator where )
    {
        if (where.is_end_iter())
           {
            clear_impl();
            return where;
           }

        trie_node_index             lastNodeIdx   = where.get_node_index();
        trie_node_data_item_index   dataItemIdx   = where.get_node_data_index();

        remove_node_value( lastNodeIdx, dataItemIdx );
        if (!is_node_item_or_childs_payloaded(lastNodeIdx, dataItemIdx))
           {
            remove_node_item( lastNodeIdx, dataItemIdx );
            where.pop_pos();
           }
        return where;
    }

    template<typename KeyIterator, typename TrieIterator>
    TrieIterator insert_key_sequence_impl( KeyIterator keyBegin, KeyIterator keyEnd, TrieIterator where, bool *pNewInserted = 0 )
    {
        if (pNewInserted) *pNewInserted = false;

        if (keyBegin==keyEnd)
          return where;

        if (where.is_end_iter()) // insert starts on trie root
           {
            trie_node_index newNodeIdx = 0;
            if (trie_nodes.empty() || !trie_nodes[0].keys_size()) // no root node
               {
                newNodeIdx = add_trie_node_impl( trie_node( ) ); // trie_node_index_npos
                trie_nodes[newNodeIdx].insert_data_item( this, *keyBegin++ );
                where.push_pos( newNodeIdx, 0 );
                if (pNewInserted) *pNewInserted = true;
               }
            else // has root node
               {
                if (trie_nodes[newNodeIdx].data_items.capacity()<4) trie_nodes[newNodeIdx].data_items.reserve(4);
                where.push_pos( newNodeIdx
                              , find_or_insert_key( *keyBegin++, newNodeIdx ) 
                                //- trie_nodes[newNodeIdx].first_item
                              );
               }
           }

        for(; keyBegin!=keyEnd; ++keyBegin)
           {
            trie_node_index lastNodeIdx   = where.get_node_index();
            //trie_node_data_item &dataItem = where.get_node_data_item();
            if (where.get_node_data_item().child_idx==trie_node_index_npos) // add new node
               {
                trie_node_index newNodeIdx = add_trie_node_impl( trie_node( ) ); // trie_node_index_npos
                trie_nodes[newNodeIdx].insert_data_item( this, *keyBegin );
                where.get_node_data_item().child_idx = newNodeIdx;
                where.push_pos( newNodeIdx , 0 );
                if (pNewInserted) *pNewInserted = true;
               }
            else
               {
                trie_node_index childIdx = where.get_node_data_item().child_idx;
                where.push_pos( childIdx
                              , find_or_insert_key( *keyBegin, childIdx ) 
                                //- trie_nodes[childIdx].first_item
                              );
               }
           }
        return where;
    }

    template<typename KeyIterator, typename TrieIterator>
    TrieIterator insert_key_sequence_impl( KeyIterator keyBegin, KeyIterator keyEnd, TrieIterator where, const mapped_type &val, bool *pNewInserted = 0 )
    {
        TrieIterator resIter = insert_key_sequence_impl( keyBegin, keyEnd, where, pNewInserted );
        if (!resIter.is_end_iter())
           set_node_value( resIter.get_node_index(), resIter.get_node_data_index(), val );
        return resIter;
    }

    template<typename KeyIterator, typename TrieIterator>
    TrieIterator insert_key_sequence_impl_set_default_if_absent( KeyIterator keyBegin, KeyIterator keyEnd, TrieIterator where, bool *pNewInserted = 0 )
    {
        TrieIterator resIter = insert_key_sequence_impl( keyBegin, keyEnd, where, pNewInserted );
        if (!resIter.is_end_iter() && !resIter.is_payloaded())
           set_node_value( resIter.get_node_index(), resIter.get_node_data_index(), mapped_type() );
        return resIter;
    }

}; // class trie





template < typename TrieType
         , typename T
         >
class trie_iterator_base_impl
{
public:

    typedef TrieType trie_type;
    friend TrieType;


    template < typename KeyType
             , typename ValueType
             , typename Traits
             >
    friend class trie_map;


    typedef typename trie_type::key_type       key_type;
    typedef typename trie_type::mapped_type    mapped_type;
    typedef typename trie_type::trie_position  trie_position_type;

    typedef typename trie_type::key_compare key_compare;
    //typedef typename trie_type::value_compare value_compare;
    typedef typename trie_type::value_type value_type;
    typedef typename trie_type::difference_type difference_type;

    typedef typename trie_type::key_type * pointer;
    typedef typename trie_type::key_type * const_pointer;

    typedef std::bidirectional_iterator_tag    iterator_category;


#ifndef TRIE_IMPL_DEBUG
protected:
#endif

    trie_type                         *pTrie;
    std::vector< trie_position_type >  curPos;

    const std::vector< trie_position_type >& get_pos_list() const
        { return curPos; }

    std::vector< trie_position_type >& get_pos_list()
        { return curPos; }

    typename trie_type::trie_node_index get_node_index( const trie_position_type &pos ) const
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( pTrie && "bad container pointer" );
        MARTY_ADT_TRIE_IMPL_ASSERT( pos.node_idx!=trie_type::trie_node_index_npos && "dereferencing end iterator" );
        MARTY_ADT_TRIE_IMPL_ASSERT( pos.node_idx<pTrie->trie_nodes.size() 
                       && pTrie->trie_nodes[pos.node_idx].keys_size()!=0
                       && "node index out of range" );
        return pos.node_idx;
    }

    typename trie_type::trie_node_index get_node_index() const
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( !curPos.empty() && "dereferencing end iterator" );
        return get_node_index(curPos.back());
    }

    typename trie_type::trie_node_data_item_index get_node_data_index( const trie_position_type &pos ) const
    {
        return pos.item_idx;
    }

    typename trie_type::trie_node_data_item_index get_node_data_index( ) const
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( !curPos.empty() && "dereferencing end iterator" );
        return get_node_data_index(curPos.back());
    }

    typename trie_type::trie_node_data_item& get_node_data_item( typename trie_type::trie_node_index nodeIdx
                                                               , typename trie_type::trie_node_data_item_index itemIdx
                                                               ) const
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( pTrie && "bad container pointer" );
        MARTY_ADT_TRIE_IMPL_ASSERT( nodeIdx!=trie_type::trie_node_index_npos && "dereferencing end iterator" );
        MARTY_ADT_TRIE_IMPL_ASSERT( nodeIdx<pTrie->trie_nodes.size() 
                       && pTrie->trie_nodes[nodeIdx].keys_size()!=0
                       && "node index out of range" );
        return pTrie->trie_nodes[nodeIdx].get_data_item( pTrie, itemIdx);
    }

    typename trie_type::trie_node_data_item& get_node_data_item( const trie_position_type &pos ) const
    {
        return get_node_data_item( pos.node_idx, pos.item_idx );
    }

    typename trie_type::trie_node_data_item& get_node_data_item( ) const
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( !curPos.empty() && "dereferencing end iterator" );
        return get_node_data_item(curPos.back());
    }

    bool is_begin_iter() const
    {
        return curPos.size()==1 && curPos[0]==trie_position_type( 0, 0 );
    }

    bool is_end_iter() const
    {
        return curPos.empty();
    }

    void clear_pos() { curPos.clear(); }

    void push_pos( typename trie_type::trie_node_index nodeIdx, typename trie_type::trie_node_data_item_index itemIdx)
    {
        curPos.push_back(trie_position_type( nodeIdx, itemIdx ));
        static_cast<T*>(this)->key_sequence_push_back( 
                                   get_node_data_item(nodeIdx,itemIdx).key
                              );
    }

    void pop_pos()
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( !curPos.empty() && "try to pop position on end iterator" );
        curPos.pop_back();
        static_cast<T*>(this)->key_sequence_pop_back( );
    }

    typename std::vector< trie_position_type >::size_type pos_size() const
    {
        return curPos.size();
    }

    bool move_to_child( const key_type &k )
    {
        typename trie_type::trie_node_index nodeIdx = get_node_index();
        const typename trie_type::trie_node &node = pTrie->trie_nodes[nodeIdx];
        MARTY_ADT_TRIE_IMPL_ASSERT( node.first_item < pTrie->trie_node_data_items.size() 
                       && "node data index out of range" );

        typename trie_node_data_item_holder::const_iterator itemsBegin = pTrie->trie_node_data_items.begin() + node.first_item;
        typename trie_node_data_item_holder::const_iterator itemsEnd   = itemsBegin + node.size;

        typename trie_node_data_item_holder::const_iterator itemFound = 
               ::std::lower_bound( itemsBegin, itemsEnd
                                 , k, typename trie_type::trie_node_data_item_comparator( pTrie->key_comp() )
                                 );
        if (itemFound==itemsEnd) return false;

        curPos.push_back(trie_position_type( itemFound->child_idx, 0 ));
        return true;
    }


    void move_to_next_impl()
    {
        typename trie_type::trie_node_index nodeIdx = get_node_index();

        const typename trie_type::trie_node &node = pTrie->trie_nodes[nodeIdx];
        //MARTY_ADT_TRIE_IMPL_ASSERT( node.first_item < pTrie->trie_node_data_items.size() 
        //               && "node data index out of range" );

        // try to go deeper
        //MARTY_ADT_TRIE_IMPL_ASSERT( curPos.back().item_idx < node.keys_size() && "node data index out of range" );

        const typename trie_type::trie_node_data_item &nodeDataItem = get_node_data_item(); //node.get_node_data_index(curPos.back().item_idx);
                     //= pTrie->trie_node_data_items[ node.first_item + curPos.back().item_idx ];
        if (nodeDataItem.child_idx!=trie_type::trie_node_data_item_index_npos)
           {
            curPos.push_back(trie_position_type( nodeDataItem.child_idx, 0 ));
            static_cast<T*>(this)->key_sequence_push_back( 
                                       get_node_data_item(nodeDataItem.child_idx,0).key
                                                         );
            return;
           }

        typename trie_type::trie_node_data_item_index nodeSize = node.keys_size();//size;

        // try to go wider
        while(++curPos.back().item_idx >= nodeSize)
           {
            // all childs traversed, need to go to upper level
            curPos.pop_back();
            if (curPos.empty()) break;

            static_cast<T*>(this)->key_sequence_pop_back( );

            nodeIdx = curPos.back().node_idx;
            MARTY_ADT_TRIE_IMPL_ASSERT( nodeIdx<pTrie->trie_nodes.size() 
                           && pTrie->trie_nodes[nodeIdx].keys_size()!=0 && "node index out of range" );
            nodeSize = pTrie->trie_nodes[nodeIdx].keys_size();//size;
           }
        
        if (curPos.empty()) return;

        static_cast<T*>(this)->key_sequence_pop_back( );
        static_cast<T*>(this)->key_sequence_push_back( get_node_data_item().key );
    }

    void move_to_prev_impl()
    {
        if (is_end_iter())
           {
            pTrie->construct_last(*this); 
            return;
           }

        typename trie_type::trie_node_index nodeIdx = get_node_index();
        //MARTY_ADT_TRIE_IMPL_ASSERT( pTrie->trie_nodes[nodeIdx].first_item < pTrie->trie_node_data_items.size() 
        //               && "node data index out of range" );

        MARTY_ADT_TRIE_IMPL_ASSERT( !is_begin_iter() && "begin can't be moved" );
        if ( is_begin_iter() ) return; // begin can't be moved

        if (curPos.back().item_idx==0) // pTrie->trie_nodes[nodeIdx].first_item
           { // go up
            MARTY_ADT_TRIE_IMPL_ASSERT( curPos.size()>1 && "position with level 0 can't be removed" );
            curPos.pop_back();
            static_cast<T*>(this)->key_sequence_pop_back( );
            return;
           }

        curPos.back().item_idx--;

        if (!pTrie->trie_nodes[nodeIdx].key_has_child( pTrie, curPos.back().item_idx ))
           return;

        pTrie->construct_last(*this, pTrie->trie_nodes[nodeIdx].get_child_id( pTrie, curPos.back().item_idx )); 
    }

    bool is_equal( const trie_iterator_base_impl &iter ) const
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( pTrie==iter.pTrie && "can't compare iterators from different containers" );
        if (curPos.size()!=iter.curPos.size()) return false;
        std::vector< trie_position_type >::const_iterator it1 = curPos.begin(), it2 = iter.curPos.begin();
        for(; it1!=curPos.end(); ++it1, ++it2)
           {
            if (*it1!=*it2) return false;
           }
        return true;
    }

    bool is_equal( const ref_pair< const trie_type*, const std::vector< trie_position_type > > &data ) const
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( pTrie==data.first && "can't compare iterators from different containers" );
        if (curPos.size()!=data.second.size()) return false;
        std::vector< trie_position_type >::const_iterator it1 = curPos.begin(), it2 = data.second.begin();
        for(; it1!=curPos.end(); ++it1, ++it2)
           {
            if (*it1!=*it2) return false;
           }
        return true;
    }

    void move_to_next_payloaded_impl()
    {
        do{
           move_to_next_impl();
          } while( !is_end_iter() && !is_payloaded());
    }

    void move_to_prev_payloaded_impl()
    {
        do{
           move_to_prev_impl();
          } while( !is_begin_iter() && !is_payloaded());
    }

public:

    trie_iterator_base_impl& get_base_impl()
    {
        return *this;
    }

    const trie_iterator_base_impl& get_base_impl() const
    {
        return *this;
    }

    /*
    ref_pair< const trie_type*, const std::vector< trie_position_type > >
    get_base_data() const
    {
        return ref_pair< const trie_type*, const std::vector< trie_position_type > >( pTrie, curPos );
    }
    */
    ref_pair< trie_type*, std::vector< trie_position_type > >
    get_base_data() const
    {
        return ref_pair< trie_type*, std::vector< trie_position_type > >( const_cast< trie_type*& >(pTrie), const_cast< std::vector< trie_position_type >& >(curPos) );
    }

    
    bool is_payloaded() const
    {
        const trie_type::trie_node_data_item &item = get_node_data_item();
        return (item.value_idx!=trie_type::value_index_npos) ? true : false;
    }


    void swap( trie_iterator_base_impl &iter )
    {
         std::swap(pTrie,iter.pTrie);
         curPos.swap(iter.curPos);
    }

    trie_iterator_base_impl( trie_type *pt, bool bBegin, typename std::vector< trie_position_type >::size_type pos_size = MARTY_ADT_TRIE_ITERATOR_RESERVE_MAGIC_NUMBER )
    : pTrie(pt), curPos()
    {
        if (pTrie->trie_nodes.empty()) return;
        curPos.reserve(pos_size);
        //static_cast<T*>(this)->key_sequence_reserve( pos_size );
        if (bBegin) 
           {
            curPos.push_back( trie_position_type(0,0) );
            //static_cast<T*>(this)->key_sequence_push_back(get_node_data_item().key);
           }
    }

    trie_iterator_base_impl( const trie_iterator_base_impl &iter )
    : pTrie(iter.pTrie), curPos(iter.curPos)
    {
        //curPos.reserve(pos_size);
        //static_cast<T*>(this)->key_sequence_reserve( curPos.capacity() );
    }

    //trie_iterator_base_impl( const ref_pair< const trie_type*, const std::vector< trie_position_type > > &data )
    trie_iterator_base_impl( const ref_pair< trie_type*, std::vector< trie_position_type > > &data )
    : pTrie(data.first), curPos(data.second)
    {
    }

    void assign( const trie_iterator_base_impl &iter )
    {
        if (&iter==this) return;
        trie_iterator_base_impl tmp(iter); swap(tmp);
    }

    //void assign( const ref_pair< const trie_type*, const std::vector< trie_position_type > > &data )
    void assign( const ref_pair< trie_type*, std::vector< trie_position_type > > &data )
    {
        trie_iterator_base_impl tmp(data); swap(tmp);
    }

    trie_iterator_base_impl& operator=(const trie_iterator_base_impl &iter)
    {
        assign(iter);
        return *this;
    }


}; // class trie_iterator_base_impl




template < typename TrieType >
class trie_iterator_impl;


template < typename TrieType >
class trie_const_iterator_impl : public trie_iterator_base_impl< TrieType
                                                               , trie_const_iterator_impl<TrieType>
                                                               >
{

protected:

    typedef trie_iterator_base_impl< TrieType
                                   , trie_const_iterator_impl<TrieType>
                                   > base_impl;
    friend typename base_impl;
    
    typedef TrieType trie_type;
    friend  typename trie_type;

    typedef typename base_impl::key_type            key_type;
    typedef typename base_impl::trie_position_type  trie_position_type;
    typedef typename base_impl::mapped_type         mapped_type;

    //  
    // friend class trie_map;
    //  
    // typedef typename trie_type::mapped_type    mapped_type;
    // typedef typename trie_type::key_compare key_compare;
    // //typedef typename trie_type::value_compare value_compare;
    // typedef typename trie_type::value_type value_type;
    // typedef typename trie_type::difference_type difference_type;
    //  
    // typedef typename trie_type::key_type * pointer;
    // typedef typename trie_type::key_type * const_pointer;
    //  
    // typedef std::bidirectional_iterator_tag    iterator_category;


    void key_sequence_push_back( const key_type &k ) { }
    void key_sequence_pop_back( ) {}

    trie_const_iterator_impl( trie_type *pt, bool bBegin, typename std::vector< trie_position_type >::size_type pos_size = MARTY_ADT_TRIE_ITERATOR_RESERVE_MAGIC_NUMBER )
        : base_impl(pt,bBegin,pos_size) {}

public:

    typedef typename trie_type::key_compare key_compare;
    //typedef typename trie_type::value_compare value_compare;
    typedef typename trie_type::value_type value_type;
    typedef typename trie_type::difference_type difference_type;

    typedef const typename trie_type::key_type       * pointer;
    //typedef const typename trie_type::key_type       * const_pointer;
    typedef const typename trie_type::key_type       & reference;
    //typedef const typename trie_type::key_type       & const_reference;

    trie_const_iterator_impl() : base_impl() {}
    trie_const_iterator_impl( const trie_iterator_impl<TrieType> &i) : base_impl(i.get_base_data()) {}
    trie_const_iterator_impl( const trie_const_iterator_impl &i) : base_impl(i) {}
    //trie_const_iterator_impl( const base_impl &i ) : base_impl(i) {}
    //template <typename T>
    //trie_const_iterator_impl( const trie_iterator_base_impl<TrieType, T> &i ) : base_impl(i) {}
    //trie_const_iterator_impl( const base_impl &i ) : base_impl(i) {}

    trie_const_iterator_impl& operator=(const trie_const_iterator_impl &i)
        { assign(i);  /* base_impl::operator=(i); */  return *this; }

    trie_const_iterator_impl& operator=(const trie_iterator_impl<TrieType> &i)
        { assign(i.get_base_data()); /* base_impl::operator=(i); */  return *this; }

    //template <typename T>
    //trie_const_iterator_impl& operator=(const trie_iterator_base_impl<TrieType, T> &i)
    //    { assign(i); /* base_impl::operator=(i); */  return *this; }


    // prefix
    trie_const_iterator_impl operator++()
        { move_to_next_impl(); return *this; }
             
    // suffix
    trie_const_iterator_impl operator++(int)
    {
        trie_const_iterator_impl res(*this);
        move_to_next_impl();     return res;
    }

    // prefix
    trie_const_iterator_impl operator--()
        { move_to_prev_impl(); return *this; }
             
    // suffix
    trie_const_iterator_impl operator--(int)
    {
        trie_const_iterator_impl res(*this);
        move_to_prev_impl();     return res;
    }

    const key_type& operator*() const 
        { return get_node_data_item().key; }

    bool operator==(const trie_const_iterator_impl &i) const
        { return is_equal(i); }

    bool operator!=(const trie_const_iterator_impl &i) const
        { return !is_equal(i); }
/*
    bool operator==(const trie_iterator_impl<TrieType> &i) const
        { return is_equal(i); }

    bool operator!=(const trie_iterator_impl<TrieType> &i) const
        { return !is_equal(i); }

    template <typename T>
    bool operator==(const trie_iterator_base_impl<TrieType, T> &i) const
        { return is_equal(i); }

    template <typename T>
    bool operator!=(const trie_iterator_base_impl<TrieType, T> &i) const
        { return !is_equal(i); }
*/

    //!!!payload
    const mapped_type& payload() const
    {
        const trie_type::trie_node_data_item &item = get_node_data_item();
        MARTY_ADT_TRIE_IMPL_ASSERT( (item.value_idx!=trie_type::value_index_npos) && "No payload" );
        return pTrie->values[item.value_idx];
    }
    // typedef typename trie_type::key_type       key_type;
    // typedef typename trie_type::mapped_type    mapped_type;

    // bool is_payloaded() const
    // {
    //     const trie_type::trie_node_data_item &item = get_node_data_item();
    //     return (item.value_idx!=trie_type::value_index_npos) ? true : false;
    // }

    // trie_type                         *pTrie;
    // std::vector< trie_position_type >  curPos;



}; // class trie_const_iterator_impl





template < typename TrieType >
class trie_iterator_impl : public trie_iterator_base_impl< TrieType
                                                         , trie_iterator_impl<TrieType>
                                                         >
{

protected:

    typedef trie_iterator_base_impl< TrieType
                                   , trie_iterator_impl<TrieType>
                                   > base_impl;
    friend typename base_impl;
    
    typedef TrieType trie_type;
    friend  typename trie_type;

    typedef typename base_impl::key_type            key_type;
    typedef typename base_impl::trie_position_type  trie_position_type;
    typedef typename base_impl::mapped_type         mapped_type;

    //  
    // friend class trie_map;
    //  
    // typedef typename trie_type::key_compare key_compare;
    // //typedef typename trie_type::value_compare value_compare;
    // typedef typename trie_type::value_type value_type;
    // typedef typename trie_type::difference_type difference_type;
    //  
    // typedef typename trie_type::key_type * pointer;
    // typedef typename trie_type::key_type * const_pointer;
    //  
    // typedef std::bidirectional_iterator_tag    iterator_category;

    void key_sequence_reserve( std::size_t s ) {}
    void key_sequence_push_back( const key_type &k ) { }
    void key_sequence_pop_back( ) {}

    trie_iterator_impl( trie_type *pt, bool bBegin, typename std::vector< trie_position_type >::size_type pos_size = MARTY_ADT_TRIE_ITERATOR_RESERVE_MAGIC_NUMBER )
        : base_impl(pt,bBegin,pos_size) {}

public:

    typedef typename trie_type::key_compare key_compare;
    //typedef typename trie_type::value_compare value_compare;
    typedef typename trie_type::value_type value_type;
    typedef typename trie_type::difference_type difference_type;

    typedef const typename trie_type::key_type       * pointer;
    typedef const typename trie_type::key_type       & reference;

    trie_iterator_impl() : base_impl() {}
    trie_iterator_impl( const trie_iterator_impl &i) : base_impl(i) {}
    //trie_iterator_impl( const base_impl &i) : base_impl(i) {}

    trie_iterator_impl& operator=(const trie_iterator_impl &i)
        { base_impl::operator=(i); return *this; }

    // prefix
    trie_iterator_impl operator++()
        { move_to_next_impl(); return *this; }
             
    // suffix
    trie_iterator_impl operator++(int)
    {
        trie_iterator_impl res(*this);
        move_to_next_impl();     return res;
    }

    // prefix
    trie_iterator_impl operator--()
        { move_to_prev_impl(); return *this; }
             
    // suffix
    trie_iterator_impl operator--(int)
    {
        trie_iterator_impl res(*this);
        move_to_prev_impl();     return res;
    }

    const key_type& operator*() const 
        { return get_node_data_item().key; }

    bool operator==(const trie_iterator_impl &i) const
        { return is_equal(i); }

    bool operator!=(const trie_iterator_impl &i) const
        { return !is_equal(i); }

    //!!!payload
    mapped_type& payload() const
    {
        const trie_type::trie_node_data_item &item = get_node_data_item();
        MARTY_ADT_TRIE_IMPL_ASSERT( (item.value_idx!=trie_type::value_index_npos) && "No payload" );
        return pTrie->values[item.value_idx];
    }


}; // class trie_iterator_impl





template < typename KeyType
         , typename ValueType
         , typename Traits
         >
class trie_map;





template < typename TrieType
         , typename TrieKeyTypeContainer
         >
class trie_map_iterator_base_impl : public trie_iterator_base_impl< TrieType
                                                          , trie_map_iterator_base_impl<TrieType, TrieKeyTypeContainer>
                                                          >
{

protected:

    typedef TrieKeyTypeContainer string_type;
    typedef trie_iterator_base_impl< TrieType
                                   , trie_map_iterator_base_impl<TrieType, TrieKeyTypeContainer>
                                   > base_impl;
    typedef TrieType trie_type;
    typedef trie_map< TrieKeyTypeContainer, typename trie_type::mapped_type, typename trie_type::key_compare >  trie_map;

    typedef typename base_impl::key_type            key_type;
    typedef typename base_impl::trie_position_type  trie_position_type;
    typedef typename base_impl::mapped_type         mapped_type;


    friend typename trie_type;
    friend typename base_impl;
    friend typename trie_map ;


    //---
    // typedef trie_iterator_base_impl< TrieType
    //                                , trie_const_iterator_impl<TrieType>
    //                                > base_impl;
    // friend typename base_impl;
    //  
    // typedef TrieType trie_type;
    // friend  typename trie_type;
    //  

    //  
    // friend class trie_map;
    //  
    // typedef typename trie_type::key_compare key_compare;
    // //typedef typename trie_type::value_compare value_compare;
    // typedef typename trie_type::value_type value_type;
    // typedef typename trie_type::difference_type difference_type;
    //  
    // typedef typename trie_type::key_type * pointer;
    // typedef typename trie_type::key_type * const_pointer;
    //  
    // typedef std::bidirectional_iterator_tag    iterator_category;
    //---


    string_type  str_key;

    void key_sequence_reserve( std::size_t s )
    {
         str_key.reserve( s );
    }

    void key_sequence_push_back( const key_type &k )
    {
         str_key.push_back( k );
    }

    void key_sequence_pop_back( )
    {
         MARTY_ADT_TRIE_IMPL_ASSERT( !str_key.empty() && "keySequence in inconsistent state" );
         str_key.erase( str_key.begin() + str_key.size() - 1u );
    }

    trie_map_iterator_base_impl( trie_type *pt, bool bBegin, typename std::vector< trie_position_type >::size_type pos_size = MARTY_ADT_TRIE_ITERATOR_RESERVE_MAGIC_NUMBER )
        : base_impl(pt,bBegin,pos_size), str_key() {}

    void build_str_key()
    {
        str_key.clear();
        str_key.reserve( curPos.capacity() );
        std::vector< trie_position_type >::const_iterator cit = curPos.begin();
        for(; cit != curPos.end(); ++cit)
           {
            typename trie_type::trie_node_data_item dataItem = get_node_data_item(*cit);
            key_sequence_push_back( get_node_data_item(*cit).key );
           }
    }

    void adjust_from_trie_iterator()
    {
        if (!is_end_iter() && !is_payloaded())
           move_to_next_payloaded_impl();
    }

    mapped_type& get_value_ref() const
    {
        return pTrie->values[ get_node_data_item().value_idx ];
    }

    mapped_type& get_value_ref()
    {
        return pTrie->values[ get_node_data_item().value_idx ];
    }


public:

    trie_map_iterator_base_impl( ) : base_impl() { }

    trie_map_iterator_base_impl( const ref_pair< trie_type*, std::vector< trie_position_type > > &data )
        : base_impl(data), str_key() { adjust_from_trie_iterator(); build_str_key(); }


    trie_map_iterator_base_impl( const trie_map_iterator_base_impl &i )
        : base_impl(i), str_key(i.str_key) { }

    trie_map_iterator_base_impl( const trie_map_const_iterator_impl<TrieType,TrieKeyTypeContainer> &i)
        : base_impl(i), str_key(i.str_key) { }

    trie_map_iterator_base_impl( const trie_map_iterator_impl<TrieType,TrieKeyTypeContainer> &i)
        : base_impl(i), str_key(i.str_key) { }

    trie_map_iterator_base_impl( const trie_const_iterator_impl<TrieType> &i) 
        : base_impl(i.get_base_data()) { adjust_from_trie_iterator(); build_str_key(); }

    trie_map_iterator_base_impl( const trie_iterator_impl<TrieType> &i) 
        : base_impl(i.get_base_data()) { adjust_from_trie_iterator(); build_str_key(); }


    trie_map_iterator_base_impl& operator=(const trie_map_const_iterator_impl<TrieType,TrieKeyTypeContainer> &i)
        { base_impl::assign(i); str_key = i.str_key; return *this; }

    trie_map_iterator_base_impl& operator=(const trie_map_iterator_impl<TrieType,TrieKeyTypeContainer> &i)
        { base_impl::assign(i); str_key = i.str_key; return *this; }

    trie_map_iterator_base_impl& operator=(const trie_const_iterator_impl<TrieType> &i)
        { base_impl::assign(i.get_base_data()); adjust_from_trie_iterator(); build_str_key(); return *this; }

    trie_map_iterator_base_impl& operator=(const trie_iterator_impl<TrieType> &i)
        { base_impl::assign(i.get_base_data()); adjust_from_trie_iterator(); build_str_key(); return *this; }

    void inc() { move_to_next_payloaded_impl(); }
    void dec() { move_to_prev_payloaded_impl(); }

/*
    void assign( const trie_iterator_base_impl &iter )
    {
        if (&iter==this) return;
        base_impl::assign(i); 
        str_key = i.str_key;
    }

    void assign( const ref_pair< trie_type*, std::vector< trie_position_type > > &data )
    {
        base_impl::assign(data); 
    }
*/


}; // trie_map_iterator_base_impl




template < typename TrieType
         , typename TrieKeyTypeContainer
         >
class trie_map_const_iterator_impl : public trie_map_iterator_base_impl< TrieType, TrieKeyTypeContainer>
{

protected:

    typedef trie_map_const_iterator_impl< TrieType, TrieKeyTypeContainer>  this_type;
    typedef trie_map_iterator_base_impl< TrieType, TrieKeyTypeContainer>   base_impl;
    friend trie_type;
    friend class base_impl;

public:

    typedef ref_pair<const string_type, const mapped_type> ref_pair_type;


    typedef typename trie_type::key_compare key_compare;
    //typedef typename trie_type::value_compare value_compare;
    //typedef typename trie_type::value_type value_type;
    typedef typename ref_pair_type                     value_type;
    typedef typename trie_type::difference_type        difference_type;

    typedef          boxed_ptr< ref_pair_type >        pointer;
    typedef          ref_pair_type                     reference;

    trie_map_const_iterator_impl() : base_impl() {}
    trie_map_const_iterator_impl( const trie_map_const_iterator_impl &i) : base_impl(i) {}
    trie_map_const_iterator_impl( const trie_map_iterator_impl<TrieType,TrieKeyTypeContainer> &i) : base_impl(i) {}
    trie_map_const_iterator_impl( const trie_const_iterator_impl<TrieType> &i) : base_impl(i) { }
    trie_map_const_iterator_impl( const trie_iterator_impl<TrieType> &i) : base_impl(i) { }

    trie_map_const_iterator_impl& operator=(const trie_map_const_iterator_impl &i)
        { base_impl::operator=(i); return *this; }

    trie_map_const_iterator_impl& operator=(const trie_map_iterator_impl<TrieType,TrieKeyTypeContainer> &i)
        { base_impl::operator=(i); return *this; }

    trie_map_const_iterator_impl& operator=(const trie_const_iterator_impl<TrieType> &i)
        { base_impl::operator=(i); return *this; }

    trie_map_const_iterator_impl& operator=(const trie_iterator_impl<TrieType> &i)
        { base_impl::operator=(i); return *this; }

    this_type operator++()    { inc(); return *this; } // prefix
    this_type operator++(int) { this_type res(*this); inc(); return res; } // suffix
    this_type operator--()    { dec(); return *this; } // prefix
    this_type operator--(int) { this_type res(*this); dec(); return res; } // suffix

    bool operator==(const this_type &i) const  { return is_equal(i); }
    bool operator!=(const this_type &i) const  { return !is_equal(i); }

    const boxed_ptr< ref_pair_type > operator->() const 
        { return boxed_ptr< ref_pair_type >( ref_pair_type(str_key, get_value_ref() ) ); }
    const ref_pair_type              
        operator* () const { return ref_pair_type(str_key, get_value_ref() ); }

}; // class trie_map_const_iterator_impl




template < typename TrieType
         , typename TrieKeyTypeContainer
         >
class trie_map_iterator_impl : public trie_map_iterator_base_impl< TrieType, TrieKeyTypeContainer>
{

protected:

    typedef trie_map_iterator_impl< TrieType, TrieKeyTypeContainer>  this_type;
    typedef trie_map_iterator_base_impl< TrieType, TrieKeyTypeContainer> base_impl;
    friend trie_type;
    friend class base_impl;
    friend class trie_map_const_iterator_impl< TrieType, TrieKeyTypeContainer >;

public:

    typedef ref_pair<const string_type, mapped_type> ref_pair_type;


    typedef typename trie_type::key_compare key_compare;
    typedef typename ref_pair_type                     value_type;
    typedef typename trie_type::difference_type        difference_type;
    typedef          boxed_ptr< ref_pair_type >        pointer;
    typedef          ref_pair_type                     reference;


    trie_map_iterator_impl() : base_impl() {}
    trie_map_iterator_impl( const trie_map_iterator_impl &i) : base_impl(i) {}
    trie_map_iterator_impl( const trie_iterator_impl<TrieType> &i) : base_impl(i) { }

    trie_map_iterator_impl& operator=(const trie_map_iterator_impl &i)
        { base_impl::operator=(i); return *this; }

    trie_map_iterator_impl& operator=(const trie_iterator_impl<TrieType> &i)
        { base_impl::operator=(i); return *this; }

    this_type operator++()    { inc(); return *this; } // prefix
    this_type operator++(int) { this_type res(*this); inc(); return res; } // suffix
    this_type operator--()    { dec(); return *this; } // prefix
    this_type operator--(int) { this_type res(*this); dec(); return res; } // suffix

    bool operator==(const this_type &i) const  { return is_equal(i); }
    bool operator!=(const this_type &i) const  { return !is_equal(i); }

    boxed_ptr< ref_pair_type > operator->() const 
        { return boxed_ptr< ref_pair_type >( ref_pair_type(str_key, get_value_ref() ) ); }
    ref_pair_type operator* () const 
        { return ref_pair_type(str_key, get_value_ref() ); }

}; // class trie_map_iterator_impl









template < typename KeyType, typename ValueType, typename Traits >
template<typename Iter>
inline void
trie<KeyType,ValueType,Traits > :: construct_last( Iter &iter, typename trie<KeyType,ValueType,Traits > ::trie_node_index nodeIdx ) const
{
    //iter.clear_pos();
    if (nodeIdx==trie_node_index_npos)
       {
        nodeIdx = 0;
        if (trie_nodes.empty() || !trie_nodes[0].keys_size()) // no root node
           return;
       }

    while(nodeIdx!=trie_node_index_npos)
       {
        MARTY_ADT_TRIE_IMPL_ASSERT( nodeIdx<trie_nodes.size() && "node index out of range" );
        MARTY_ADT_TRIE_IMPL_ASSERT( trie_nodes[nodeIdx].keys_size()!=0 && "invalid node: size==0" );
        iter.push_pos( nodeIdx, trie_nodes[nodeIdx].keys_size()-1 );
        nodeIdx = trie_nodes[nodeIdx].get_child_id( this, trie_nodes[nodeIdx].keys_size()-1 ); // .child_idx;
       }
}


template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: const_iterator
trie<KeyType,ValueType,Traits > :: begin() const
{
    return typename trie<KeyType,ValueType,Traits > :: const_iterator( const_cast< trie<KeyType,ValueType,Traits >* >(this), true );
}

template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: begin()
{
    return typename trie<KeyType,ValueType,Traits > :: iterator( this, true );
}

template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: const_iterator
trie<KeyType,ValueType,Traits > :: end() const
{
    return typename trie<KeyType,ValueType,Traits > :: const_iterator( const_cast< trie<KeyType,ValueType,Traits >* >(this), false );
}

template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: end()
{
    return typename trie<KeyType,ValueType,Traits > :: iterator( this, false );
}


template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: non_const_iter_end() const
{
    return typename trie<KeyType,ValueType,Traits > :: iterator( const_cast< trie<KeyType,ValueType,Traits >* >(this), false );
}

template < typename KeyType, typename ValueType, typename Traits >
inline bool
trie<KeyType,ValueType,Traits > :: next( typename trie<KeyType,ValueType,Traits > :: const_iterator &it
                                       , const typename trie<KeyType,ValueType,Traits > :: key_type &k
                                       ) const
{
    return it->move_to_child( k );
}

template < typename KeyType, typename ValueType, typename Traits >
inline bool
trie<KeyType,ValueType,Traits > :: next( typename trie<KeyType,ValueType,Traits > :: iterator &it
                                       , const typename trie<KeyType,ValueType,Traits > :: key_type &k
                                       ) const
{
    return it->move_to_child( k );
}

template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: 
insert( typename trie<KeyType,ValueType,Traits > :: iterator where
      , const typename trie<KeyType,ValueType,Traits > :: key_type &k )
{
    return pTrie->insert_key_sequence_impl( (&k), ((&k)+1), where );
}

template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: 
insert( typename trie<KeyType,ValueType,Traits > :: iterator where
      , const typename trie<KeyType,ValueType,Traits > :: key_type &k
      , const typename trie<KeyType,ValueType,Traits > :: mapped_type &v )
{
    return pTrie->insert_key_sequence_impl( (&k), ((&k)+1), where, v );
}

template < typename KeyType, typename ValueType, typename Traits >
template<typename KeyIter>
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: 
insert( const KeyIter &b, const KeyIter &e )
{
    return insert_key_sequence_impl( b, e, typename trie<KeyType,ValueType,Traits > :: iterator( this, false ) );
}

template < typename KeyType, typename ValueType, typename Traits >
template<typename KeyIter>
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: 
insert( typename trie<KeyType,ValueType,Traits > :: iterator where
      , const KeyIter &b, const KeyIter &e )
{
    return insert_key_sequence_impl( b, e, where );
}

template < typename KeyType, typename ValueType, typename Traits >
template<typename KeyIter>
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: 
insert( const KeyIter &b, const KeyIter &e
      , const typename trie<KeyType,ValueType,Traits > :: mapped_type &v )
{
    return insert_key_sequence_impl( b, e, typename trie<KeyType,ValueType,Traits > :: iterator( this, false ), v );
}

template < typename KeyType, typename ValueType, typename Traits >
template<typename KeyIter>
inline
typename trie<KeyType,ValueType,Traits > :: iterator
trie<KeyType,ValueType,Traits > :: 
insert( typename trie<KeyType,ValueType,Traits > :: iterator where
      , const KeyIter &b, const KeyIter &e
      , const typename trie<KeyType,ValueType,Traits > :: mapped_type &v )
{
    return pTrie->insert_key_sequence_impl( b, e, where, v );
}


template < typename KeyType, typename ValueType, typename Traits >     template<typename KeyIter>   inline
typename trie<KeyType,ValueType,Traits > :: const_iterator 
trie<KeyType,ValueType,Traits > :: find( const KeyIter &b, const KeyIter &e ) const
{
    return find_impl( b, e, typename trie<KeyType,ValueType,Traits > :: const_iterator( this, false ) );
}

template < typename KeyType, typename ValueType, typename Traits >     template<typename KeyIter>   inline
typename trie<KeyType,ValueType,Traits > :: const_iterator 
trie<KeyType,ValueType,Traits > :: find( typename trie<KeyType,ValueType,Traits > :: const_iterator findFrom, const KeyIter &b, const KeyIter &e ) const
{
    return find_impl( b, e, findFrom );
}

template < typename KeyType, typename ValueType, typename Traits >     template<typename KeyIter>   inline
typename trie<KeyType,ValueType,Traits > :: iterator 
trie<KeyType,ValueType,Traits > :: find( const KeyIter &b, const KeyIter &e )
{
    return find_impl( b, e, typename trie<KeyType,ValueType,Traits > :: iterator( this, false ) );
}

template < typename KeyType, typename ValueType, typename Traits >     template<typename KeyIter>   inline
typename trie<KeyType,ValueType,Traits > :: iterator 
trie<KeyType,ValueType,Traits > :: find( typename trie<KeyType,ValueType,Traits > :: iterator findFrom, const KeyIter &b, const KeyIter &e )
{
    return find_impl( b, e, findFrom );
}


template < typename KeyType, typename ValueType, typename Traits >     inline
typename trie<KeyType,ValueType,Traits > :: const_iterator 
trie<KeyType,ValueType,Traits > :: find( typename trie<KeyType,ValueType,Traits > :: key_type k ) const
{
    return find_impl( k, typename trie<KeyType,ValueType,Traits > :: const_iterator( this, false ) );
}

template < typename KeyType, typename ValueType, typename Traits >     inline
typename trie<KeyType,ValueType,Traits > :: const_iterator 
trie<KeyType,ValueType,Traits > :: find( typename trie<KeyType,ValueType,Traits > :: const_iterator findFrom, typename trie<KeyType,ValueType,Traits > :: key_type k ) const
{
    return find_impl( k, findFrom );
}

template < typename KeyType, typename ValueType, typename Traits >     inline
typename trie<KeyType,ValueType,Traits > :: iterator 
trie<KeyType,ValueType,Traits > :: find( typename trie<KeyType,ValueType,Traits > :: key_type k )
{
    return find_impl( k, typename trie<KeyType,ValueType,Traits > :: iterator( this, false ) );
}

template < typename KeyType, typename ValueType, typename Traits >     inline
typename trie<KeyType,ValueType,Traits > :: iterator 
trie<KeyType,ValueType,Traits > :: find( typename trie<KeyType,ValueType,Traits > :: iterator findFrom, typename trie<KeyType,ValueType,Traits > :: key_type k )
{
    return find_impl( k, findFrom );
}


template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: iterator 
trie<KeyType,ValueType,Traits > :: 
erase( typename trie<KeyType,ValueType,Traits > :: iterator  what )
{
    return erase_impl( what );
}

template < typename KeyType, typename ValueType, typename Traits >
inline bool
trie<KeyType,ValueType,Traits > :: 
is_payloaded( const typename trie<KeyType,ValueType,Traits > :: const_iterator &i ) const
{
    return i.is_payloaded();
}

template < typename KeyType, typename ValueType, typename Traits >
inline bool 
trie<KeyType,ValueType,Traits > :: 
is_payloaded( const typename trie<KeyType,ValueType,Traits > :: iterator &i )
{
    return i.is_payloaded();
}

template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: mapped_type& 
trie<KeyType,ValueType,Traits > :: 
payload( typename trie<KeyType,ValueType,Traits > :: iterator where
       , const typename trie<KeyType,ValueType,Traits > :: mapped_type &v )
{
    trie_node_index             lastNodeIdx   = where.get_node_index();
    trie_node_data_item_index   dataItemIdx   = where.get_node_data_index();
    return set_node_value( lastNodeIdx, dataItemIdx, v );
}

template < typename KeyType, typename ValueType, typename Traits >
inline void 
trie<KeyType,ValueType,Traits > :: 
remove_payload( typename trie<KeyType,ValueType,Traits > :: iterator where )
{
    trie_node_index             lastNodeIdx   = where.get_node_index();
    trie_node_data_item_index   dataItemIdx   = where.get_node_data_index();
    remove_node_value( lastNodeIdx, dataItemIdx );
}

//! Получаем ссылку на нагрузку
template < typename KeyType, typename ValueType, typename Traits >
inline
typename trie<KeyType,ValueType,Traits > :: mapped_type& 
trie<KeyType,ValueType,Traits > :: 
payload( typename trie<KeyType,ValueType,Traits > :: iterator where )
{
    return where.payload();
}

//!< Получаем const ссылку на нагрузку
template < typename KeyType, typename ValueType, typename Traits >
inline
const typename trie<KeyType,ValueType,Traits > :: mapped_type& 
trie<KeyType,ValueType,Traits > :: 
payload( typename trie<KeyType,ValueType,Traits > :: const_iterator where ) const
{
    return where.payload();
}





template < typename KeyType
         , typename ValueType
         , typename Traits = std::less< typename KeyType::value_type >
         >
class trie_map
{

public:

    typedef trie< typename KeyType::value_type, ValueType, Traits >      trie_type;

    // typedef typename allocator_type::const_pointer const_pointer;
    // typedef typename allocator_type::const_reference const_reference;
    // typedef typename allocator_type::pointer pointer;
    // typedef typename allocator_type::reference reference;

    typedef trie_map_const_iterator_impl< trie_type, KeyType >  const_iterator;
    typedef trie_map_iterator_impl< trie_type, KeyType >        iterator;
    typedef std::reverse_iterator<iterator>                     reverse_iterator;
    typedef std::reverse_iterator<const_iterator>               const_reverse_iterator;
    //typedef typename iterator::ref_pair_type                    value_type;
    typedef typename std::pair<KeyType,ValueType>               value_type; // !!!
    typedef std::size_t                                         size_type;
    typedef std::ptrdiff_t                                      difference_type;

    // !!!
    typedef KeyType                                             key_type;
    typedef Traits                                              key_compare;
    typedef ValueType                                           mapped_type;

protected:

    trie_type            m_trie;

public:

    trie_map() : m_trie() {}

    explicit 
    trie_map( const Traits& Comp ) : m_trie(Comp) {}

    trie_map( const trie_map& r ) : m_trie(r.m_trie) {}

    template<class InputIterator>
    trie_map( InputIterator f, InputIterator l )
    {
        insert( f, l );
    }

    template<class InputIterator>
    trie_map( InputIterator f, InputIterator l, const Traits& Comp )
    : m_trie(Comp)
    {
        insert( f, l );
    }

    typename trie_type& get_base()                 { return m_trie; }
    const typename trie_type& get_base() const     { return m_trie; }

    size_type get_used_mem() const        { return m_trie.get_used_mem(); }

    void reserve( size_type s, size_type ri = 4 ) { m_trie.reserve( s, ri ); }

    const_iterator begin( ) const         { return m_trie.begin(); }
    iterator begin( )                     { return m_trie.begin(); }
    const_iterator end( ) const           { return m_trie.end(); }
    iterator end( )                       { return m_trie.end(); }

    reverse_iterator rbegin()             { return (reverse_iterator(end())); }
    const_reverse_iterator rbegin() const { return (const_reverse_iterator(end())); }
    reverse_iterator rend()               { return (reverse_iterator(begin())); }
    const_reverse_iterator rend() const   { return (const_reverse_iterator(begin())); }

    void clear( )                  { m_trie.clear(); }

    size_type count( const key_type& k ) const
    {
        typename trie_type::const_iterator it = m_trie.find( k.begin(), k.end() );
        if (it.is_end_iter() || !it.is_payloaded()) return 0;
        return 1;
    }

    size_type size() const  { return m_trie.values_size(); }
    size_type empty() const { return m_trie.values_size()==0; }

    //size_type max_size( ) const;

    iterator erase( iterator where )
    {
        MARTY_ADT_TRIE_IMPL_ASSERT( where.is_payloaded() && "iterator has no payload" );
        iterator res = m_trie.erase_impl(where);
        if (!where.is_end_iter() && !where.is_payloaded())
           where.move_to_prev_payloaded_impl();
        return res;
    }

    iterator erase( iterator f, iterator l )
    {
        if (f==l) return f;
        while( f != --l )
           {
            erase( l );
           }
        return erase( l );
    }

    size_type erase( const key_type& k )
    {
        iterator it = find( k );
        if (it.is_end_iter()) return 0;
        erase( it );
        return 1;
    }

    iterator find( const key_type& k )
    {
        iterator res = m_trie.find_impl( k.begin(), k.end(), end() );
        if (res.is_end_iter() || res.is_payloaded()) return res;
        return end();
    }

    const_iterator find( const key_type& k ) const
    {
        iterator res = m_trie.find_impl( k.begin(), k.end(), end() );
        if (res.is_end_iter() || res.is_payloaded()) return res;
        return end();
    }

    std::pair <iterator, bool> insert( const value_type& v )
    {
        bool newInserted = false;
        MARTY_ADT_TRIE_IMPL_ASSERT( v.first.begin()!=v.first.end() && "can't insert empty sequence" );
        iterator it = m_trie.insert_key_sequence_impl( v.first.begin(), v.first.end(), end(), v.second, &newInserted );
        return std::make_pair(it,newInserted);
    }

    iterator insert( iterator where, const value_type& _Val )
    {
        return insert( v ).first; // ignore hint
    }

    template<class InputIterator>
    void insert( InputIterator f, InputIterator l )
    {
        for(; f!=l; ++f)
           {
            //m_trie.insert_key_sequence_impl( f->first.begin(), f->first.end(), end(), &newInserted );
            insert( *f );
           }
    }

    void swap( trie_map &t )
    {
        m_trie.swap(t.m_trie);
    }

    mapped_type& operator[]( const key_type &k )
    {
        //iterator it = insert( std::make_pair(k,mapped_type()) ).first;
        //return it.get_value_ref();
        bool newInserted = false;
        //if (k=="material") 
        //   newInserted = true;
        MARTY_ADT_TRIE_IMPL_ASSERT( k.begin()!=k.end() && "can't insert empty sequence" );
        iterator it = m_trie.insert_key_sequence_impl_set_default_if_absent( k.begin(), k.end(), end(), &newInserted );
        return it.get_value_ref();
    }

    //UNDONE:
    //equal_range 
    //get_allocator 
    //key_comp
    //lower_bound
    //max_size 
    //upper_bound 
    //value_comp 

}; // trie_map


}; // namespace containers
}; // namespace marty


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif


#endif /* MARTY_TRIE_H */

