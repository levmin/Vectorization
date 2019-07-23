/*
   Vectorization support in standard C++ - proposal draft

   Copyright © 2019 Lev Minkovsky

   MIT License

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#ifdef __cpp_concepts
#if __has_include(<concepts>)
#include <concepts>
#else
#include <type_traits>
template<class T> concept floating_point = std::is_floating_point_v<T>;
#endif
#define FLOATING_POINT floating_point
#else
#define FLOATING_POINT typename
#endif

#if __has_include(<version>)
#include <version>
#endif

#include <xmmintrin.h>
#include <iostream>

namespace std {
   namespace numbers {
      template<typename T, unsigned N> 
      class FloatVec
      {
      public:
         //TODO - this constructor should take an std::span as a parameter
         FloatVec(const T* p)
         {
            memcpy(data, p, sizeof(T) * N);
         }
         FloatVec& operator+=(FloatVec& v)
         {
            for (int i = 0; i < 4; i++)
               data[i] += v.data[i];
            return *this;
         }
         //TODO - this function should take an std::span as a parameter
         void store(T* p)
         {
            memcpy(p, data, sizeof(T) * N);
         }

      private:
         T data[N];
      };
      
      /* Instantiation for a vector of 4 floats accelerated with intrinsics
      */
      template<> class FloatVec<float, 4>
      {
      public:
         //TODO - this constructor should take an std::span as a parameter
         FloatVec(const float* p)
         {
//            cout << "load\n";
            data = _mm_load_ps(p);
         }
         FloatVec& operator+=(FloatVec& v)
         {
//            cout << "add\n";
            data = _mm_add_ps(data, v.data);
            return *this;
         }
         //TODO - this function should take an std::span as a parameter
         void store(float* p)
         {
//            cout << "store\n";
            _mm_store_ps(p, data);
         }

      private:
         __m128 data;
      };
   }
}
#undef FLOATING_POINT
using namespace std;
int main()
{
   float A[] = { 1,2,3,4 };
   float B[] = { 5,6,7,8 };
   float C[] = { 0,0,0,0 };
   numbers::FloatVec<float,4> a(A);
   numbers::FloatVec<float, 4> b(B);
   a += b;
   a.store(C);
   for (auto c : C)
      cout << c << endl;
}

