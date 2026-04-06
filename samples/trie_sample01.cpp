/*! \file
    \brief Тестируем trie

    F:\work\pfs\trunk\src\graphoman\
      graphoman-test-04.cpp
      graphoman-test-05.cpp
      graphoman-test-05a.cpp

*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <random>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <random>


#ifndef MARTY_ADT_TRIE_IMPL_ASSERT
    #if defined(WIN32) || defined(_WIN32)
        #define MARTY_ADT_TRIE_IMPL_ASSERT(expr)    _ASSERTE(expr)
    #else
        #include <<assert.h>>
    #endif
#endif

#include "marty_adt/trie.h"

 


template<typename StreamType, typename IterType> inline
void printMapIter(StreamType &s, IterType it)
{
    s << "first: " << it->first << ", second: " << it->second;
}


template<typename StreamType, typename IterType> inline
void printTrieIterPayloadMark(StreamType &s, IterType it)
{
    if (it.is_payloaded())
        s << "*";
    else
        s << " ";
}


template<typename StreamType, typename TrieType> inline
void printTrie(StreamType &s, const TrieType &testTrie)
{
    typedef TrieType trie_type;
    typedef typename trie_type::value_type char_type;


    s << "Direct order     " << ": ";
    trie_type::const_iterator ttIt = testTrie.begin();
    for(; ttIt!=testTrie.end(); ++ttIt )
    {
        s << *ttIt;   printTrieIterPayloadMark(s,ttIt);   s << " ";
    }
    s << "\n";


    s << "Reverse order (1)" << ": ";
    ttIt = testTrie.end();
    for(; ttIt!=testTrie.begin(); --ttIt )
    {
        trie_type::const_iterator ttItPrev = ttIt;
        --ttItPrev;
        s << *ttItPrev;   printTrieIterPayloadMark(s,ttItPrev);   s << " ";
    }
    s << "\n";


    s << "Reverse order (2)" << ": ";
    trie_type::const_reverse_iterator rtIt = testTrie.rbegin();
    for(; rtIt!=testTrie.rend(); ++rtIt )
    {
        s << *rtIt;   s << " "; /* Пока рисуем всегда пустой payload mark */   s << " ";
        // s << *rtIt;   printTrieIterPayloadMark(s,rtIt.base());   s << " "; // тут хуета какая-то происходит, потом разберусь
    }
    // s << "\n";
    s << " // Without payload marks\n";


    s << "--- As map" << "\n";
    typedef marty::adt::trie_map_const_iterator_impl< trie_type, std::basic_string<char_type> > map_const_iterator_type;
    map_const_iterator_type mit = testTrie.begin();
    for(; mit!=testTrie.end(); ++mit )
    {
        s << "  ";   printMapIter(std::cout, mit);   std::cout << "\n";
    }

}



int main( int argc, char* argv[] )
{
    using std::cout;


    typedef marty::adt::trie< char, unsigned> trie_type;
    trie_type testTrie;

    std::cout << "- First time map\n";
    std::vector<std::string> test_insert_sequences = { "ab"  , "ad"  , "ae"  , "adg" , "adga", "adgb", "adgc", "aef" , "abc"  };
    for(auto str : test_insert_sequences)
        testTrie.insert( str.begin(), str.end() );
    printTrie(std::cout, testTrie);
    std::cout << "\n";


    std::cout << "- Second time map\n";
    std::vector<std::string> test_valued_sequences = { "adga", "adgb", "adgc", "aef" , "abc"  };
    for(auto i=0u; i!=test_valued_sequences.size(); ++i)
        testTrie.insert( test_valued_sequences[i].begin(), test_valued_sequences[i].end(), i );
    printTrie(std::cout, testTrie);
    std::cout << "\n";


    // Теперь хотим потестить на произвольных последовательностях
    // Чтобы совсем левого не было, используем только те символы, которые уже используются

    std::set<char> charSet;
    auto addCharsToSet = [&](const std::string &str)  { for(auto ch : str) charSet.insert(ch); };

    for(auto str : test_insert_sequences)
        addCharsToSet(str);
    
    for(auto str : test_valued_sequences)
        addCharsToSet(str);


    std::vector<char> charSetVec;
    std::cout << "All used chars: ";
    for(auto ch : charSet)
    {
        charSetVec.emplace_back(ch);
        std::cout << ch;
    }
    std::cout << "\n\n";


    // https://en.cppreference.com/w/cpp/header/random
    // https://stackoverflow.com/questions/7560114/random-number-c-in-some-range

    std::random_device rndDevice; // obtain a random number from hardware

    std::mt19937 rndGeneratorChar(rndDevice());
    std::mt19937 rndGeneratorLen (rndDevice());

    std::uniform_int_distribution<std::size_t> charIndexDistribution(0, charSetVec.size()-1); // задаем диапазон генерируемых индексов символов
    std::uniform_int_distribution<std::size_t> stringLenDistribution(1, 5); // диапазон длин строк

    std::cout << "Index Distribution Sample Sequence: ";
    for(auto n=0u; n!=10; ++n)
        std::cout << charIndexDistribution(rndGeneratorChar) << "  ";
    std::cout << "\n\n";

    std::cout << "Len Distribution Sample Sequence  : ";
    for(auto n=0u; n!=10; ++n)
        std::cout << stringLenDistribution(rndGeneratorLen) << "  ";
    std::cout << "\n\n";



    std::unordered_set<std::string>  addedVals;
    std::vector<std::string>         valsForTest;

    auto numTests = 100;
    //auto numTests = 1;

    //for(auto i=0u; i!=numTests; ++i)
    while(valsForTest.size()<numTests)
    {
        std::string str;
        std::size_t strLen = stringLenDistribution(rndGeneratorLen);
        str.reserve(strLen);
        for(auto li=0u; li!=strLen; ++li)
        {
            std::size_t chIdx = charIndexDistribution(rndGeneratorChar);
            str.append(1, charSetVec[chIdx]);
        }

        if (addedVals.find(str)!=addedVals.end())
            continue;

        addedVals.insert(str);
        valsForTest.emplace_back(str);
        // std::cout << str << "\n";
    
    }


    auto makeFill = [&](const std::string &str, std::size_t minLen) -> std::string
    {
        std::size_t fillSize = 0;
        if (str.size()<minLen)
            fillSize = minLen - str.size();

        return std::string(fillSize,' ');
    };



    auto printTrieIterInfo = [&](trie_type::const_iterator it)
    {
        if (it==testTrie.end())
        {
            cout << "-  Not found at all\n";
        }
        else if (!it.is_payloaded())
        {
            cout << "+  Found but no payload\n";
        }
        else
        {
            const auto &pl = testTrie.payload(it);
            cout << "++ Found, payload: " <<  /* foundIt */ pl << "\n";
        }
    };


    auto printValAndTrieIterInfo = [&](const std::string &val, trie_type::const_iterator findFrom)
    {
        if (val.empty())
            return;

        // std::size_t fillSize = 0;
        // if (val.size()<6)
        //     fillSize = 6 - val.size();

        cout << val << makeFill(val, 6)  /* std::string(fillSize,' ') */  << " : " ;

        //trie_type::const_iterator foundIt = testTrie.find(testTrie.begin(), val.begin(), val.end());
        trie_type::const_iterator foundIt = testTrie.find(findFrom, val.begin(), val.end());

        printTrieIterInfo(foundIt);
    };


    auto incSearchTest = [&](const std::string &val)
    {
        cout << "SrchFor: " << val << makeFill(val,6) << " - ";

        std::string::const_iterator vit = val.begin();
        if (vit==val.end())
        {
            // cout << "[-]";
            cout << "\n";
            return;
        }

        // cout << "         ";

        trie_type::const_iterator foundIt = testTrie.find(testTrie.end(), *vit);
        while( /* (vit!=val.end()) && */  (foundIt!=testTrie.end()))
        {
            cout << *foundIt;
            if (foundIt.is_payloaded())
                cout << "*";
            else
                cout << " ";

            ++vit;
            if (vit==val.end())
                break;

            foundIt = testTrie.find(foundIt, *vit);
        }

        if (foundIt==testTrie.end())
        {
            // cout << "[-]";
            cout << " - char";
            if (vit==val.end())
                cout << " ";
            else
                cout << " '" << *vit << "' ";
            cout << "not found at this step";
        }
        else
        {
            if (!foundIt.is_payloaded())
                cout << " - input breaks at mid of seq";
            else
                cout << " - +++ found final";
        }

        cout << "\n";
    };

    cout << "Increment Search Tests\n";
    {
        incSearchTest("azx");
        incSearchTest("abc");
        incSearchTest("aef");
        incSearchTest("ad");
        incSearchTest("a");
        incSearchTest("aeukus");
    }
    cout << "\n";


    // Если нужен поиск с "с нуля", надо передавать testTrie.end() как findFrom
    cout << "Looking for test vals (1)\n";
    printValAndTrieIterInfo("abc", testTrie.end());
    printValAndTrieIterInfo("aef", testTrie.end());
    printValAndTrieIterInfo("ad" , testTrie.end());
    printValAndTrieIterInfo("a"  , testTrie.end());
    printValAndTrieIterInfo("aa" , testTrie.end());
    std::cout << "\n";

    cout << "Looking for test vals (2)\n";
    printValAndTrieIterInfo("abc", testTrie.begin());
    printValAndTrieIterInfo("aef", testTrie.begin());
    printValAndTrieIterInfo("ad" , testTrie.begin());
    printValAndTrieIterInfo("a"  , testTrie.begin());
    printValAndTrieIterInfo("aa" , testTrie.begin());
    std::cout << "\n";


    cout << "Looking for test vals (3)\n";
    for(auto val : valsForTest)
    {
        if (val.empty())
            continue;

        printValAndTrieIterInfo(val, testTrie.end());
        
    }
    
    return 0;
}



