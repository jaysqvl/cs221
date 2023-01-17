/**
 * @file fib.cpp
 * Contains Fibonacci-number generating functions both utilizing and not
 * utilizing memoization.
 *
 * @author Matt Joras
 * @date Winter 2013
 */

#include "fib.h"
#include <map>

using std::map;

/**
 * Calculates the nth Fibonacci number where the zeroth is defined to be
 * 0.
 * @param n Which number to generate.
 * @return The nth Fibonacci number.
 */
unsigned long fib(unsigned long n)
{
    /* Your code goes here! */

    // Stub value - remove when you are done
    return  n <=1 ? n : fib(n-1) + fib(n-2);
}

/**
 * Calculates the nth Fibonacci number where the zeroth is defined to be
 * 0. This version utilizes memoization.
 * @param n Which number to generate.
 * @return The nth Fibonacci number.
 */
unsigned long memoized_fib(unsigned long n)
{
    /* Your code goes here! */

    // Stub value - remove when you are done
    static map <unsigned long, unsigned long> memoized = { {0,0}, {1,1}, };
    auto look_up = memoized.find(n);

    if (look_up != memoized.end()) {
        return look_up->second;
    }
    
    unsigned long result = memoized_fib(n-2) + memoized_fib(n-1);
    memoized[n] = result;
    return result;
}
