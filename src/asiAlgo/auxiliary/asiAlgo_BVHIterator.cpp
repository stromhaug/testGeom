//-----------------------------------------------------------------------------
// Created on: 23 September 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_BVHIterator.h>

//-----------------------------------------------------------------------------

//! Constructs iterator for traversing BVH tree.
//! \param[in] bvh structure to traverse.
asiAlgo_BVHIterator::asiAlgo_BVHIterator(const opencascade::handle< BVH_Tree<double, 4> >& bvh)
: m_bvh         (bvh),
  m_iStackHead  (-1),
  m_currentNode (0) // First index
{}

//-----------------------------------------------------------------------------

//! \return true if there is still something to iterate over,
//!         false -- otherwise.
bool asiAlgo_BVHIterator::More() const
{
  if ( m_currentNode == -1 )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

//! \return current item.
const BVH_Vec4i& asiAlgo_BVHIterator::Current() const
{
  return m_bvh->NodeInfoBuffer()[m_currentNode];
}

//-----------------------------------------------------------------------------

//! \return current node's index.
int asiAlgo_BVHIterator::CurrentIndex() const
{
  return m_currentNode;
}

//-----------------------------------------------------------------------------

//! Moves iterator to the next position.
void asiAlgo_BVHIterator::Next()
{
  // The purpose of this method is to set the value of m_currentNode properly

  if ( this->IsLeaf() )
    m_currentNode = (m_iStackHead < 0) ? -1 : m_stack[m_iStackHead--]; // Return to parent
  else
  {
    const BVH_Item leftChild        = this->Current().y();
    const BVH_Item rightChild       = this->Current().z();
    BVH_Item       items2Process[2] = {-1, -1};
    int            itemsCount       = 0;

    // Check if children are not blocked externally
    if ( !m_blocked.Contains(leftChild) )
      items2Process[itemsCount++] = leftChild;
    //
    if ( !m_blocked.Contains(rightChild) )
      items2Process[itemsCount++] = rightChild;

    // Set current node to one of the children. It can be -1 if
    // there is nothing to iterate over.
    m_currentNode = items2Process[0]; // Left item is iterated first (because it is left ;)

    // Fill stack to be able to return to non-traversed items
    for ( int i = 1; i < itemsCount; ++i ) // Notice that we start from 1
      m_stack[++m_iStackHead] = items2Process[i];

    // If there is no any allowed child, move back in the stack
    if ( !itemsCount && m_iStackHead >= 0 )
      m_currentNode = m_stack[m_iStackHead--];
  }
}

//-----------------------------------------------------------------------------

//! \return true if the current BVH node is leaf, false -- otherwise.
bool asiAlgo_BVHIterator::IsLeaf() const
{
  const int x = this->Current().x();
  return (x != 0);
}

//-----------------------------------------------------------------------------

//! Prevents iterator from visiting left child of the current node.
void asiAlgo_BVHIterator::BlockLeft()
{
  m_blocked.Add( this->Current().y() );
}

//-----------------------------------------------------------------------------

//! Prevents iterator from visiting right child of the current node.
void asiAlgo_BVHIterator::BlockRight()
{
  m_blocked.Add( this->Current().z() );
}