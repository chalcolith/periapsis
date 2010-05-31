#ifndef GSGL_DATA_DICT_H
#define GSGL_DATA_DICT_H

//
// $Id: dictionary.hpp 15 2008-07-20 17:36:39Z kulibali $
//
// Copyright (c) 2008-2010, The Periapsis Project. All rights reserved. 
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

#include "data/data.hpp"
#include "data/exception.hpp"
#include "data/comparable.hpp"
#include "data/indexable.hpp"
#include "data/pointer.hpp"
#include "data/pool.hpp"
#include "data/list.hpp"
#include "data/stack.hpp"
#include "data/string.hpp"

#include <typeinfo>

namespace gsgl
{

    namespace data
    {

		template <typename T, typename I>
		class dictionary_iterator;

    
        /// A dictionary or map.
        /// Type T can be any type; type I must implement '<', '>' and '=='.
        ///
        /// Currently implemented by means of a simple unbalanced binary tree.
        /// \todo Improve implementation.
        template <typename T, typename I>
        class dictionary
            : public data_object, 
			  public iterable<T, dictionary_iterator<T,I> >, 
              public indexable<T,I>
        {
			friend class dictionary_iterator<T,I>;
        
            /// \internal
            struct dict_node
            {
				T item;
				I index;
                
                dict_node *left;
                dict_node *right;
                
				dict_node();
                dict_node(const T & item, const I & index, dict_node *left = 0, dict_node *right = 0);
				dict_node(const dict_node &);
				dict_node & operator= (const dict_node &);
                virtual ~dict_node();

				bool operator== (const dict_node &) const;
                
                dict_node *copy(pool<dict_node> &) const;
            }; // struct dict_node
            
            mutable dict_node *root;
            gsgl::index_t count;

			pool<dict_node> node_pool;
            
        public:
            dictionary();
            dictionary(const dictionary &);
            dictionary & operator= (const dictionary &);
            virtual ~dictionary();
            
            /// \name Countable Implementation
            /// \{
            virtual gsgl::index_t size() { return count; }
            virtual void clear();
            /// \}
            

            /// \name Iterable Implementation
            /// \{
			using iterable<T, dictionary_iterator<T,I> >::append;
            virtual void append(const T & item);
            virtual void insert(const iterator & i, const T & item);
            virtual void remove(const iterator & i);
            /// \}


            /// \name Indexable Implementation
            /// \{
            virtual bool contains_index(const I & index) const;
            virtual const T & item(const I & index) const;
            virtual T & item(const I & index);
            /// \}


			/// \name Dictionary Functionality
			/// \{
            bool operator== (const dictionary<T,I> &) const;
	        virtual void remove(const I & index);
			/// \}

                        
        private:
            T & get_item(const I & index, bool create_new = true);

            dict_node *find_node(const I & index, dict_node ***link);
            void remove_node(dict_node *cur, dict_node **link);            

			void deallocate_nodes(dict_node *cur);
        }; // class dict
        

		//////////////////////////////////////////
        
		template <typename T, typename I>
		dictionary<T,I>::dict_node::dict_node()
			: left(0), right(0)
		{
			throw internal_exception(__FILE__, __LINE__, L"Internal dictionary nodes should never use the default constructor.");
		} // dictionary<T,I>::dict_node::dict_node()


        template <typename T, typename I>
        dictionary<T,I>::dict_node::dict_node(const T & item, const I & index, dict_node *left, dict_node *right)
            : item(item), index(index), left(left), right(right)
        {
        } // dictionary<T,I>::dict_node::dict_node()


		template <typename T, typename I>
		dictionary<T,I>::dict_node::dict_node(const dict_node & dn)
			: item(dn.item), index(dn.index), left(dn.left), right(dn.right)
		{
			throw internal_exception(__FILE__, __LINE__, L"Internal dictionary nodes should never be copy constructed.");
		} // dictionary<T,I>::dict_node::dict_node()


		template <typename T, typename I>
		typename dictionary<T,I>::dict_node & dictionary<T,I>::dict_node::operator= (const dict_node & dn)
		{
			throw internal_exception(__FILE__, __LINE__, L"Internal dictionary nodes should never be copied.");
		} // dictionary<T,I>::dict_node::operator= ()


        template <typename T, typename I>
        dictionary<T,I>::dict_node::~dict_node()
        {
			// the dictionary needs to delete the tree!
        } // dictionary<T,I>::dict_node::~dict_node()


		template <typename T, typename I>
		bool dictionary<T,I>::dict_node::operator== (const dict_node &) const
		{
			throw internal_exception(__FILE__, __LINE__, L"Internal dictionary nodes should never be compared.");
		} // dictionary<T,I>::dict_node::operator== ()


        template <typename T, typename I>
        typename dictionary<T,I>::dict_node *dictionary<T,I>::dict_node::copy(pool<dict_node> & p) const
        {
            return new (p.allocate_inplace()) dict_node(item, index, left ? left->copy(p) : 0, right ? right->copy(p) : 0);
        } // dictionary<T,I>::dict_node::copy()
        

        //////////////////////////////////////////
        
        template <typename T, typename I>
        dictionary<T,I>::dictionary() 
            : data_object(), iterable<T, dictionary_iterator<T,I> >(), indexable<T,I>(), 
			  root(0), count(0)
        {
        } // dictionary<T,I>::dictionary()


        template <typename T, typename I>
        dictionary<T,I>::dictionary(const dictionary & d)
            : data_object(), iterable<T, dictionary_iterator<T,I> >(), indexable<T,I>(), 
			  root(0), count(d.count)
        {
            if (d.root)
                root = d.root->copy(node_pool);
        } // dictionary<T,I>::dictionary()
        

        template <typename T, typename I>
        dictionary<T,I> & dictionary<T,I>::operator= (const dictionary & d)
        {
            this->clear();
            if (d.root)
                root = d.root->copy(node_pool);
            count = d.count;
            return *this;
        } // dict<T,I>::operator= ()
        

        template <typename T, typename I> 
        dictionary<T,I>::~dictionary()
        {
            this->clear();
        } // dict<T,I>::~dict()
        

        template <typename T, typename I>
        bool dictionary<T,I>::operator== (const dictionary<T,I> & d) const
        {
            typename const_iterator a = this->iter(), b = d.iter();

            for (; a.is_valid() && b.is_valid(); ++a, ++b)
            {
                if (!(*a == *b))
                    return false;
            }

            if (a.is_valid() || b.is_valid())
                return false;

            return true;
        } // dictionary<T,I>::operator== ()


        template <typename T, typename I> 
        void dictionary<T,I>::clear()
        {
			if (root)
				deallocate_nodes(root);
            root = 0;
            count = 0;
        } // dictionary<T,I>::clear()


        template <typename T, typename I> 
        void dictionary<T,I>::append(const T & item)
        {
            throw internal_exception(__FILE__, __LINE__, L"It is an error to append a value alone to a dictionary.");
        } // dictionary<T,I>::append()


        template <typename T, typename I>
        void dictionary<T,I>::insert(const iterator & i, const T & item)
        {
            throw internal_exception(__FILE__, __LINE__, L"It is an error to insert a value alone to a dictionary.");
        } // dictionary<T,I>::insert()


        template <typename T, typename I>
        void dictionary<T,I>::remove(const iterator & i)
        {
            remove(i.get_index());
        } // dictionary<T,I>::remove()


        //

        template <typename T, typename I>
        void dictionary<T,I>::remove(const I & index)
        {
            if (root)
            {
                dict_node *cur, **link = &root;
                cur = find_node(index, &link);

                // delete node
                if (cur)
                {
                    remove_node(cur, link);
                    return;
                }
            }

            // we never found a node to remove
            throw runtime_exception(L"Attempted to remove an invalid item from a dictionary.");
        } // dictionary<T,I>::remove()
                

        //

        template <typename T, typename I>
        bool dictionary<T,I>::contains_index(const I & index) const
        {
            dict_node *cur, **link = &root;
            cur = const_cast<dictionary<T,I> *>(this)->find_node(index, &link);

            return cur != 0;
        } // dictionary<T,I>::contains_index()


        template <typename T, typename I>
        const T & dictionary<T,I>::item(const I & index) const
        {
            return const_cast<dictionary<T,I> *>(this)->get_item(index);
        } // dict<T,I>::item()
        

        template <typename T, typename I>
        T & dictionary<T,I>::item(const I & index)
        {
            return get_item(index);
        } // dict<T,I>::item()
        

        //

        template <typename T, typename I>
        T & dictionary<T,I>::get_item(const I & index, bool create_new)
        {
            dict_node *cur, **link = &root;
            cur = find_node(index, &link);
            
            if (!cur)
            {
                if (create_new)
                {
                    *link = cur = new (node_pool.allocate_inplace()) dict_node(T(), index);
                    ++count;
                }
                else
                {
                    throw runtime_exception(L"Unable to find item in dictionary!");
                }
            }

            assert(cur);
            return cur->item;
        } // dict<T,I>::get_item()
        

        template <typename T, typename I>
        typename dictionary<T,I>::dict_node *dictionary<T,I>::find_node(const I & index, dict_node ***link)
        {
            dict_node *cur = **link;

            while (cur)
            {
                if (index < cur->index)
                {
                    *link = &cur->left;
                    cur = cur->left;
                }
                else if (index == cur->index)
                {
                    break;
                }
                else
                {
                    *link = &cur->right;
                    cur = cur->right;
                }
            }

            return cur;
        } // dictionary<T,I>::find_node()


        template <typename T, typename I>
        void dictionary<T,I>::remove_node(dict_node *cur, dict_node **link)
        {
            assert(cur);
            assert(link);

            if (cur->left && cur->right)
            {
                // get predecessor
                dict_node **pred_link = &cur->left, *pred = cur->left;
                while (pred->right)
                {
                    pred_link = &pred;
                    pred = pred->right;
                }

                // copy data from predecessor to cur
                cur->index = pred->index;
                cur->item = pred->item;

                // delete pred
                remove_node(pred, pred_link);
            }
            else 
            {
                if (cur->left)
                {
                    *link = cur->left;
                }
                else if (cur->right)
                {
                    *link = cur->right;
                }
                else
                {
                    *link = 0;
                }

                cur->left = cur->right = 0;
				node_pool.deallocate(cur);
                //delete cur;

                --count;
            }
        } // dictionary<T,I>::remove_node()


		template <typename T, typename I>
		void dictionary<T,I>::deallocate_nodes(dict_node *cur)
		{
			assert(cur);

			if (cur->left)
				deallocate_nodes(cur->left);
			if (cur->right)
				deallocate_nodes(cur->right);

			node_pool.deallocate(cur);
		} // dictionary<T,I>::deallocate_nodes()


		//////////////////////////////////////////

        /// \internal
		template <typename T, typename I>
		class dictionary_iterator
        {
			const dictionary<T,I> & parent;

			typename dictionary<T,I>::dict_node *cur, *prev;
			data::simple_stack<typename dictionary<T,I>::dict_node *> nodes_seen;

		protected:
			dictionary_iterator(const iterable<T, dictionary_iterator> & parent_iterable)
				: parent(dynamic_cast<const dictionary<T,I> &>(parent_iterable)),
				  cur(0), prev(0)
			{
				for (cur = const_cast<typename dictionary<T,I>::dict_node *>(parent.root); cur && cur->left; cur = cur->left)
					nodes_seen.push(cur);
			}

            dictionary_iterator(const dictionary_iterator & di)
				: parent(di.parent), cur(di.cur), prev(di.prev), nodes_seen(di.nodes_seen) {}

            dictionary_iterator & operator= (const dictionary_iterator & di)
			{
				parent = di.parent;
				cur = di.cur;
				prev = di.prev;
				nodes_seen = di.nodes_seen;
				return *this;
			}

			inline bool is_valid() const { return cur != 0; }
			
			inline const T & operator* () const
			{
				if (cur)
					return const_cast<T &>(cur->item);
				else
					throw memory_exception(__FILE__, __LINE__, L"Iterator overflow in dictionary iterator dereference.");
			}

			dictionary_iterator & operator++();

		public:
			inline const I & get_index() const
			{
				if (cur)
					return cur->index;
				else
					throw memory_exception(__FILE__, __LINE__, L"Iterator overflow in dictionary iterator key reference.");
			}
        }; // class dictionary_iterator
            
        
		template <typename T, typename I>
        dictionary_iterator<T,I> & dictionary_iterator<T,I>::operator++()
		{
			if (cur)
			{
				if (cur->right && cur->right != prev)
				{
					prev = cur;
					nodes_seen.push(cur);
					cur = cur->right;

					while (cur->left)
					{
						nodes_seen.push(cur);
						cur = cur->left;
					}
				}
				else if (nodes_seen.size())
				{
					prev = cur;
					cur = nodes_seen.top();
					nodes_seen.pop();

					while (nodes_seen.size() && prev == cur->right)
					{
						prev = cur;
						cur = nodes_seen.top();
						nodes_seen.pop();
					}

					if (prev == cur->right)
						cur = 0;
				}
				else
				{
					cur = 0;
				}
			}
			else
			{
				throw memory_exception(__FILE__, __LINE__, L"Iterator overflow in dictionary iterator preincrement.");
			}

            return *this;
		} // dictionary_iterator<T,I>::operator++()
        

    } // namespace data
    
} // namespace gsgl

#endif
