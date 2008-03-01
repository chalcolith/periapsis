//
// $Id: array.cpp 314 2008-03-01 16:33:47Z Gordon $
//
// Copyright (c) 2008, The Periapsis Project. All rights reserved. 
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met: 
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer. 
// 
// * Redistributions in binary form must reproduce the above copyright 
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the distribution. 
// 
// * Neither the name of the The Periapsis Project nor the names of its 
//   contributors may be used to endorse or promote products derived from 
//   this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "data/array.hpp"

#include <cstdlib>
#include <cstring>

namespace gsgl
{

	namespace data
	{

		void *array_base::allocate(const gsgl::index_t & num_bytes)
		{
			return ::malloc(static_cast<size_t>(num_bytes));
		} // array_base::allocate()


		void *array_base::reallocate(void *ptr, const gsgl::index_t & num_bytes)
		{
			return ::realloc(ptr, static_cast<size_t>(num_bytes));
		} // array_base::reallocate()


		void array_base::deallocate(void *ptr)
		{
			::free(ptr);
		} // array_base::deallocate()


		void array_base::move(void *dest, void *src, const gsgl::index_t & num_bytes)
		{
			::memmove(dest, src, static_cast<size_t>(num_bytes));
		} // array_base::move()


		void array_base::copy(void *dest, void *src, const gsgl::index_t & num_bytes)
		{
			::memcpy(dest, src, static_cast<size_t>(num_bytes));
		} // array_base::copy()


		void array_base::set(void *dest, const unsigned char & val, const gsgl::index_t & num_bytes)
		{
			::memset(dest, val, static_cast<size_t>(num_bytes));
		} // array_base::set()


	} // namespace data

} // namespace gsgl
