/*! \file
    \brief Итерация по символам при помощи trie
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

#include "umba/simple_formatter.h"


#ifndef MARTY_ADT_TRIE_IMPL_ASSERT
    #if defined(WIN32) || defined(_WIN32)
        #define MARTY_ADT_TRIE_IMPL_ASSERT(expr)    _ASSERTE(expr)
    #else
        #include <<assert.h>>
    #endif
#endif

#include "marty_adt/trie.h"
 


/* Подразумевается, что в trie все цепочки символов в конце имеют полезную нагрузку.
   Другой вариант - само наличие цепочки даже без полезной нагрузки в конце может что-то означать. Но это не тут.

 */
template <typename IterType> inline
std::size_t findSymbolLen( const marty::adt::trie< char, unsigned> &trie, IterType curIt, IterType endIt )
{
    std::size_t len = 0;

    if (curIt==endIt)
        return len;

    marty::adt::trie< char, unsigned>::const_iterator tit = trie.find(trie.end(), *curIt);
    if (tit==trie.end())
        return len;

    // if (!tit.is_payloaded())
    //     return len;

    //std::vector<std::size_t> payloadedLens;

    std::size_t payloadedLen = 0;
    marty::adt::trie< char, unsigned>::const_iterator payloadedTit = trie.end(); // если нужно получить полезную нагрузку

    ++curIt; ++len;

    if (tit.is_payloaded())
    {
        //payloadedLens.emplace_back(len);
        payloadedLen = len;
        payloadedTit = tit;
    }

    while(curIt!=endIt)
    {
        tit = trie.find(tit, *curIt);
        if (tit==trie.end())
        {
            break;
        }

        ++curIt; ++len;
        if (tit.is_payloaded())
        {
            //payloadedLens.emplace_back(len);
            payloadedLen = len;
            payloadedTit = tit;
        }
    }

    return payloadedLen;

}


int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;
    //using namespace std;
    using std::cout;

    typedef marty::adt::trie< char, unsigned> trie_type;
    trie_type testTrie;

    // https://en.wikipedia.org/wiki/Ligature_(writing)

    for( unsigned i=1u; i!=0x7Fu; ++i)
    {
        std::string str = std::string(1, (char)i);
        testTrie.insert( str.begin(), str.end(), i );
    }

    std::vector<std::string> ligatures = { "aa", "ae", "ao", "aot", "au", "ay", "ee", "ea", "oa", "oe", "oo", "ou", "ia", "ie", "io", "ing", "ua", "ue", "ui", "uit", "uo", "tt", "ll", "dd", "ss", "pp", "zz", "ff"
                                         , "Aa", "Ae", "Ao", "Aot", "Au", "Ay", "Ee", "Ea", "Oa", "Oe", "Oo", "Ou", "Ia", "Ie", "Io", "Ing", "Ua", "Ue", "Ui", "Uit", "Uo", "tt", "Ll", "Dd", "Ss", "Pp", "Zz", "Ff"
                                         , "::" 
                                         };

    for(auto i=0u; i!=ligatures.size(); ++i)
        testTrie.insert( ligatures[i].begin(), ligatures[i].end(), 0x80+i );

    std::string testStr = "Intuitive languages feels achieve out mean Trivial integration Our header dependencies build you'll "
                          "written vanilla all everything should require your settings Serious testing heavily including all exceptional behavior "
                          "leaks Google additionally effectively executing billions maintain quality additionally fuzz quality efficiency "
                          "overhead against pointer union enumeration efficiency default generalization following std::string std::map std::vector "
                          "bool booleans class needs adding support goal speed header already"
                          ""
                          ;

    /*
    "Intuitive syntax. In languages such as Python, JSON feels like a first class data type. We used all the operator "
    "magic of modern C++ to achieve the same feeling in your code. Check out the examples below and you'll know what I mean. "
    "Trivial integration. Our whole code consists of a single header file json.hpp. That's it. No library, no subproject, "
    "no dependencies, no complex build system. The class is written in vanilla C++11. All in all, everything should require "
    "no adjustment of your compiler flags or project settings. "
    "Serious testing. Our code is heavily unit-tested and covers 100% of the code, including all exceptional behavior. "
    "Furthermore, we checked with Valgrind and the Clang Sanitizers that there are no memory leaks. Google OSS-Fuzz "
    "additionally runs fuzz tests against all parsers 24/7, effectively executing billions of tests so far. To maintain "
    "high quality, the project is following the Core Infrastructure Initiative (CII) best practices."
    "Memory efficiency. Each JSON object has an overhead of one pointer (the maximal size of a union) and one enumeration "
    "element (1 byte). The default generalization uses the following C++ data types: std::string for strings, int64_t, "
    "uint64_t or double for numbers, std::map for objects, std::vector for arrays, and bool for Booleans. However, you "
    "can template the generalized class basic_json to your needs. "
    "Speed. There are certainly faster JSON libraries out there. However, if your goal is to speed up your development "
    "by adding JSON support with a single header, then this library is the way to go. If you know how to use a std::vector "
    "or std::map, you are already set. "
    */

    std::string::const_iterator smbIt = testStr.begin();
    const 
    std::string::const_iterator endIt = testStr.end();

    while(smbIt!=testStr.end())
    {
        std::size_t symbolLen = findSymbolLen( testTrie, smbIt, endIt );
        if (!symbolLen)
        {
            cout << "\nFound unknown symbol\n";
            return 0;
        }

        std::size_t restLen = testStr.end() - smbIt;
        if (restLen<symbolLen)
        {
            cout << "\nRest text len is less than found symbol len\n";
            return 0;
        }

        std::string::const_iterator nextSmdIt = smbIt + symbolLen;
        cout << std::string(smbIt, nextSmdIt) << " ";
        smbIt = nextSmdIt;
    }

    return 0;
}



