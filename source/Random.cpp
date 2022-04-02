/* 
   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include "Random.h"
#include <stdio.h>
#include <Windows.h>

#ifdef USE_MERSENNE
HANDLE Random::hMutex = CreateMutex(0, false, 0);
#endif

Random::Random()
{
#ifdef USE_MERSENNE
	mti =N+1; /* mti==N+1 means mt[N] is not initialized */
	init_genrand((unsigned long)this);
#else
		m_lastNumber = 0;
#endif
}

Random::~Random()
{
}

Random::Random(unsigned int seed)
{
#ifndef USE_MERSENNE
    m_lastNumber = seed;
#else
	init_genrand(seed);
#endif
}

int Random::GetInt(int a, int b)
{
    if(b-a <= 0)
        return a;
    m_lastNumber = m_lastNumber * 1664525 +	1013904223;
    return a + m_lastNumber % (1 + b - a);
}

float Random::GetFloat(float a, float b)
{
    if(b-a <= 0)
        return a;
#ifdef USE_MERSENNE
   // WaitForSingleObject(hMutex, INFINITE);
	return a + genrand_real2() * (b-a);
   // ReleaseMutex(hMutex);
#else
	int number;

	//m_lastNumber = m_lastNumber * 0x343FD + 0x269EC3; 
	//number = m_lastNumber >> 16 & 0x7FFF;
    m_lastNumber = m_lastNumber * 1664525 +	1013904223;

	//return a + ((float) number / (float) 0x7FFF)*(b-a);
    return a + ((float) m_lastNumber / (float) 0xFFFFFFFF)*(b-a);
#endif
}

void Random::Seed(unsigned int seed)
{
#ifndef USE_MERSENNE
	m_lastNumber = seed;
#endif
}

// m_lastNumber = (1103515245*m_lastNumber + 12345) % 0xFFFFFFFF;