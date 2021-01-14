/** @file
* @brief K-dimension KD tree version
* @author konstantin.burlachenko@kaust.edu.sa
*
* Implementation of KD-tree way for space partitioning (spatial indexing). Algorithm proposed by Jon Bentley undergraduate student from Stanford.
* It allows todo:
* 1. Insert/Search/Delete points in R^N.
* 2. Range search - find all "R" keys that lie in a specific range ~R+lg(N) - typical, ~R+sqrt(N) - worst case. (Assume that number of total points N)
* 3. Range count - find count of keys that lie in a specific range
*
* Benefits over grid implementation
* 1. Grid implementation is great in 2 dimension if choose grid MxM, and choose M=N^0.5 and points are uniformly distributed.
* 2. But points are usually not uniformly distributed over the whole space in 2D and 3D space.
* 3. KD tree adapts to the distribution more pretty.
* 4. KD tree generalizes for higher dimension more then 2 or 3
* 5. I heard from Datamining lectures (http://web.stanford.edu/class/cs246/)  from prof. Jure Leskovec that in high dimensional space is almost the true that every thing is very far from everything.
*
* Applications is everywhere where we use geometric data:
* 1. ray-tracing
* 2. 2d range search
* 3. Collision detection
* 4. Nearest neighbors search, etc
* 5. n-body simulation algorithm proposed by Anrew Appel
* 6. Search in databases
*/

#pragma once

#include "Comparators.h"
#include <assert.h>
#include <stddef.h>
#include <vector>
#include <limits>

namespace lw_index_datastructs
{
    /** Result of test of intersection "plane" (with normal pointed with orth of standart basis in R^n) and "area".
    */
    enum class KdTreepPlaneIntersectTest
    {
        eAreaInLeft,          ///< Area is on the left with respect to test plane
        eAreaInRight,         ///< Area is on the right with respect to test plane
        eAreaIntersectsPlane, ///< Area intersects test plane
        eDontKnow             ///< Test algorithm can not say anything about current situation
    };

    template <class TCoord,                                   ///< Used type for coordinate
              size_t Dimension = 2,                           ///< Used number of dimensions
              class TNorm = TCoord,                           ///< Used type for store norm of the vector
              typename Cmp = Comparator<TCoord> > ///< Used type to perform compare between coordinates
    class KDtree
    {
    public:
        typedef const TCoord* TPointerToTCoordinates;         ///< Typedef for pointer to constand coordinates
        const static size_t kDimension = Dimension;           ///< Size of dimension where KD tree is building
    protected:
        static TCoord L2NormSqr(const TCoord* a, const TCoord* b)
        {
            TCoord distance = TCoord();
            for (size_t i = 0; i < Dimension; ++i)
            {
                TNorm tmp = a[i] - b[i];
                distance += tmp*tmp;
            }
            return distance;
        }

        /** Get coordinate from pointCoordinates with index
        */
        static const TCoord& getCoord(const TCoord* pointCoordinates, size_t index)
        {
            if (index > Dimension)
                assert(!"Not legal access");
            return pointCoordinates[index];
        }

        struct KDtreeNode
        {
            /** KDtreeNode ctor. Setup internal pointers to zero. Memory in which nodes are lying does not belong to KDtreeNode.
            */
            KDtreeNode()
            : left(nullptr)
            , right(nullptr)
            , pointCoordinates(nullptr)
            , enable(true)
            {}

            /** Do nothing. Does not touch left or right
            */
            ~KDtreeNode() 
            {
            }
            
            /** Is current node is a leaf ?
            * @return true if node is leaf
            */
            bool isLeaf() const {
                return left == nullptr && right == nullptr;
            }

            /** Get coordinate of point stored in this KDTree node
            * @param index index of coordinate
            * @return reference to coordinate
            */
            const TCoord& getCoord(size_t index) {
                return KDtree::getCoord(pointCoordinates, index);
            }

            KDtreeNode* left;               ///< points for which point[depth-of-node % Dimension] is less then in the node (what less means was defined by the depth of the node)
            KDtreeNode* right;              ///< points for which point[depth-of-node % Dimension] is greater or equal then in the node (what less means was defined by the depth of the node)
            const TCoord* pointCoordinates; ///< raw pointer to array of coordinates for point
            bool enable;                    ///< marker that point is enable for futher evaluations
        };

    public:
        /** Get number of dimensions for points.
        * @return dimension for KD-tree
        */
        static size_t kDimensions() {
            return Dimension;
        }

        /** Construct KD-tree from points. Only raw pointers are copying inside the tree.
        * @param ctr container with points
        * @param num number of points in container.
        */
        template <class Container>
        KDtree(Container& ctr, size_t num)
        : top(nullptr)
        , numPoints(0)
        , numDisablePoints(0)
        {
            for (size_t i = 0; ctr.size(); ++i)
                pushInTree(ctr[i]);
        }

        /** Default ctor
        */
        KDtree()
        : top(nullptr)
        , numPoints(0)
        , numDisablePoints(0)
        {}

        /** Copy ctor
        */
        KDtree(const KDtree& rhs)
        : top(nullptr)
        , numPoints(rhs.numPoints)
        , numDisablePoints(rhs.numDisablePoints)
        {
            auto visitFunction = [&](KDtreeNode* leftSubtree, KDtreeNode* rightSubtree, KDtreeNode* x) -> KDtreeNode*
                                 {
                                     KDtreeNode* res = new KDtreeNode();
                                     res->left = leftSubtree;
                                     res->right = rightSubtree;
                                     res->pointCoordinates = x->pointCoordinates;
                                     return res;
                                 };
            // postorder garantees that 'leftSubtree' and 'rightSubtree' have already been constructed
            top = postOrderNodesTraverse(rhs.top, visitFunction);
        }

        /** Dtor
        */
        ~KDtree() {
            removeAll();
        }

        /** Assignment operator
        */
        KDtree& operator = (const KDtree& rhs)
        {
            removeAll();
            auto visitFunction = [&](KDtreeNode* leftSubtree, KDtreeNode* rightSubtree, KDtreeNode* x) -> KDtreeNode*
            {
                KDtreeNode* res = new KDtreeNode();
                res->left = leftSubtree;
                res->right = rightSubtree;

                res->point = x->point;
                res->enable = x->enable;
                return res;
            };
            // postorder guarantees that 'leftSubtree' and 'rightSubtree' have already been constructed
            top = postOrderNodesTraverse(rhs.top, visitFunction);
        }

        /** Remove all, i.e. clean all KD-tree
        */
        void removeAll()
        {
            auto visitFunction = [&](const KDtreeNode* /*leftSubtree*/, const KDtreeNode* /*rightSubtree*/, const KDtreeNode* x) -> KDtreeNode*
            {
                delete x;
                return nullptr;
            };
            top = postOrderNodesTraverse(top, visitFunction);            
            numPoints = 0;
            numDisablePoints = 0;
        }

        /** Get number of points inside KD-tree
        */
        size_t size() const {
            return numPoints;
        }

        /** Get number of points inside KD-tree
        */
        size_t sizeOfDisabledPoints() const {
            return numDisablePoints;
        }

        /** Get number of points inside KD-tree which are currently enable for queries
        */
        size_t sizeOfEnablePoints() const {
            return numPoints - numDisablePoints;
        }

        
        /** Get number of points inside KD-tree which are currently enable for queries
        */
        void makeAllPointsEnable() {
            auto visitFunction = [&](const KDtreeNode* /*leftSubtree*/, const KDtreeNode* /*rightSubtree*/, KDtreeNode* x) -> KDtreeNode*
            {
                x->enable = true;
                return x;
            };
            top = postOrderNodesTraverse(top, visitFunction);
            numDisablePoints = 0;
        }

        /** Calculate height of the tree. Time is ~N
        * @return height of the tree
        * @remark rather expensive operation. Implementation do not store height of the tree currently explicitly.
        */
        size_t height()
        {
            if (!top)
                return 0;

            size_t h = 0;
            inorderLeafsTraverseWithDepth(top, [&](KDtreeNode* node, size_t depth)
                                                {
                                                    if (depth > h)
                                                        h = depth;
                                                }, 
                                                1);
            return h;
        }

        /** Search all points which are inside area. Area was defined implicitly by couple functions
        * @param outContainer container with lightweight pointers to coordinates. Please don't modify data to which this const pointers are leading.
        * @param areaRelativeToPlane function object with two arguments "position" and "index". Inside a function you should define what a relation of area w.r.t. to the plane which pass through "position" and which has normal e_i pointed into positive direction.
        * @param isPointInsideArea function which should return true if test point which comes as input first argument is inside area
        * @sa rangeSearchWithBoundingBox
        */
        template<class AreaRelativeToPlane, class IsPointInsideArea, class TContainer = std::vector<const TCoord*>>
        void rangeSearchWithPredicats(TContainer& outContainer, const AreaRelativeToPlane& areaRelativeToPlane, const IsPointInsideArea& isPointInsideArea) {
            rangeSearchInternal(outContainer, top, areaRelativeToPlane, isPointInsideArea, 0);
        }

        /** Search all point which lie inside [minPoint, maxPoint]
        * @param outContainer container with lightweight pointers to coordinates. Please don't modify data to which this const pointers are leading.
        * @param minPoint point which element wise store minimum coordinate for Axis Aligned Bounding Box
        * @param maxPoint point which element wise store maximum coordinate for Axis Aligned Bounding Box
        */
        template<class Point, class TContainer = std::vector<const TCoord*>>
        void rangeSearchWithBoundingBox(TContainer& outContainer, const Point& minPoint, const Point& maxPoint)
        {
            size_t Dim = kDimensions();
            auto areaRelativeToPlanePredicate = [&](const TCoord* position, size_t coordIndex)
                                                {
                                                    if (minPoint[coordIndex] > position[coordIndex])
                                                        return KdTreepPlaneIntersectTest::eAreaInRight;
                                                    else if (maxPoint[coordIndex] < position[coordIndex])
                                                        return KdTreepPlaneIntersectTest::eAreaInLeft;
                                                    else
                                                        return KdTreepPlaneIntersectTest::eAreaIntersectsPlane;
                                                };

            auto isPointInsidePredicate = [&](const TCoord* testPoint)
                                              {
                                                  for (size_t c = 0; c < Dim; ++c)
                                                  {
                                                      if (testPoint[c] < minPoint[c] || testPoint[c] > maxPoint[c])
                                                          return false;
                                                  }
                                                  return true;
                                              };

            return rangeSearchWithPredicats(outContainer, areaRelativeToPlanePredicate, isPointInsidePredicate);
        }

        /** Append point to the KD-tree. Difference sequence of insertions leads to different trees.
        * @param point appended point
        * @remark just to have some guarantees about bad constructed tree you can append points in some shuffle order        
        */
        void pushInTree(const TCoord* pointCoordinates)
        {
            KDtreeNode* node = new KDtreeNode();
            node->pointCoordinates = pointCoordinates;
            top = pushInTreeInternal(top, node, 0);
            numPoints++;
        }

        /** Find nearest point to query point by Euclidean (L2) metric
        * @param point requested point
        * @param leavePointsAsEnable special flag which can be used in scenario when after search you want temporary disable found other points which are enable in KDTree
        * @return coord of closest point and zero if the tree is empty
        */
        const TCoord* nearestPointInEuclidianMetric(const TCoord* pointCoordinates, bool leavePointsAsEnable = true)
        {
            if (!top)
                return nullptr;

            TNorm bestNorm = std::numeric_limits<TNorm>::max();
            KDtreeNode* bestNode = nullptr;
            nearestPointInternal(top, bestNorm, &bestNode, pointCoordinates, 0);
            if (!bestNode)
                return nullptr;

            if (bestNode->enable != leavePointsAsEnable)
            {
                if (leavePointsAsEnable)
                {
                    bestNode->enable = true;
                    numDisablePoints--;
                }
                else
                {
                    bestNode->enable = false;
                    numDisablePoints++;
                }
            }
            return bestNode->pointCoordinates;
        }
        
        /** Find nearest point to query point by Euclidian (L2) metric
        * @param pointCoordinates requested point
        * @param K upper bound on number of nearest points in which you're interesting in
        * @return array with coordinates to closest point and zero if the tree is empty
        * @param leavePointsAsEnable special flag which can be used in scenario when after search you want temporary disable found other points which are enable in KDTree
        * @remark naive implementation
        */
        template <class Point, class TContainer = std::vector<const TCoord*>>
        void findKnearestPointInEuclidianMetric(TContainer& outContainer, const TCoord* pointCoordinates, size_t K, bool leavePointsAsEnable = true)
        {
            if (!top)
                return nullptr;

            std::vector<KDtreeNode*> nodes;
            for (;;)
            {
                TNorm bestNorm = std::numeric_limits<TNorm>::max();
                KDtreeNode* bestNode = nullptr;
                nearestPointInternal(top, bestNorm, &bestNode, pointCoordinates, 0);
                if (!bestNode)
                {
                    break;
                }
                else
                {
                    bestNode->enable = false;
                    nodes.push_back(bestNode);
                }
            }

            size_t sz = nodes.size();
            for (size_t i = 0; i < sz; ++i)
            {
                outContainer.push_back(nodes[i].pointCoordinates);
                if (nodes[i].enable != leavePointsAsEnable)
                {
                    if (leavePointsAsEnable)
                    {
                        nodes[i].enable = true;
                        numDisablePoints--;
                    }
                    else
                    {
                        nodes[i].enable = false;
                        numDisablePoints++;
                    }
                }
            }
        }

    protected:
        /** Find nearest point with pruning
        */
        void nearestPointInternal(KDtreeNode* root, TNorm& bestNormSquare, KDtreeNode** bestNode, const TCoord* requestPoint, size_t depth)
        {
            if (!root)
                return;

            size_t curCoord = depth % Dimension;
            size_t nextDepth = depth + 1;

            // Possible update norm square. Only in case when node is enable.
            if (root->enable)
            {
                TNorm newNormSquare = KDtree::L2NormSqr(requestPoint, root->pointCoordinates);
                if (newNormSquare < bestNormSquare)
                {
                    bestNormSquare = newNormSquare;
                    (*bestNode) = root;
                }
            }

            // Recursive calls. Even root is disabled it can be used as anchor in which part it's better to search
            if (CmpHelper::IsLess(cmp(KDtree::getCoord(requestPoint, curCoord), root->getCoord(curCoord))))
            {
                // request point is from the left relative to current root - goto left first, maybe it we will decrease best norm
                nearestPointInternal(root->left, bestNormSquare, bestNode, requestPoint, nextDepth);
                TNorm tmpDistanceToSeparatePlane = root->getCoord(curCoord) - KDtree::getCoord(requestPoint, curCoord);
                TNorm tmpDistanceToSeparatePlaneSqr = tmpDistanceToSeparatePlane*tmpDistanceToSeparatePlane;

                if (tmpDistanceToSeparatePlaneSqr < bestNormSquare)                                      // check that now bestNorm is big enough to check points from other half space
                    nearestPointInternal(root->right, bestNormSquare, bestNode, requestPoint, nextDepth); // need go to right because also possible we will decrease norm
            }
            else
            {
                // request point is from the right relative to current root - goto right first, maybe we will decrease norm square
                nearestPointInternal(root->right, bestNormSquare, bestNode, requestPoint, nextDepth);
                TNorm tmpDistanceToSeparatePlane = root->getCoord(curCoord) - KDtree::getCoord(requestPoint, curCoord);
                TNorm tmpDistanceToSeparatePlaneSqr = tmpDistanceToSeparatePlane*tmpDistanceToSeparatePlane;
                if (tmpDistanceToSeparatePlaneSqr < bestNormSquare)                                      // check that now bestNorm is big enough to check points from other half-space
                    nearestPointInternal(root->left, bestNormSquare, bestNode, requestPoint, nextDepth); // need go to right because also possible we will decrease norm
            }
        }

        /** Typical case complexity ~R + lg(N), worst case ~R+sqrt(N)
        * @param out container in which founded points will be collected
        * @param root root of the tree
        * @param areaRelativeToPlane function 
        * @param isPointInsideArea test that point is inside user defined area
        */
        template<class AreaRelativeToPlane, class IsPointInsideArea, class TContainer>
        static void rangeSearchInternal(TContainer& out,  KDtreeNode* root, const AreaRelativeToPlane& areaRelativeToPlane,  const IsPointInsideArea& isPointInsideArea, size_t depth)
        {
            if (root == nullptr)
                return;
            {
                size_t coord = depth % Dimension;

                switch (areaRelativeToPlane(root->pointCoordinates, coord))
                {
                case KdTreepPlaneIntersectTest::eAreaInLeft:
                    rangeSearchInternal(out, root->left, areaRelativeToPlane, isPointInsideArea, depth + 1);
                    break;
                case KdTreepPlaneIntersectTest::eAreaInRight:
                    rangeSearchInternal(out, root->right, areaRelativeToPlane, isPointInsideArea, depth + 1);
                    break;
                case KdTreepPlaneIntersectTest::eDontKnow:
                case KdTreepPlaneIntersectTest::eAreaIntersectsPlane:
                    if (root->enable && isPointInsideArea(root->pointCoordinates))
                        out.push_back(root->pointCoordinates);
                    rangeSearchInternal(out, root->left, areaRelativeToPlane, isPointInsideArea, depth + 1);
                    rangeSearchInternal(out, root->right, areaRelativeToPlane, isPointInsideArea, depth + 1);
                    break;
                }
            }
        }

        /**
        * @param root root of the tree
        * @param toAppend item you want to append
        */
        KDtreeNode* pushInTreeInternal(KDtreeNode* root, KDtreeNode* toAppend, size_t depth)
        {
            if (root == nullptr)  {
                return toAppend;
            }

            // Select which coordinate use to do comparision
            size_t coord = depth % Dimension;
            int cmpRes = cmp(toAppend->getCoord(coord), root->getCoord(coord));

            if (CmpHelper::IsLess(cmpRes))
                root->left = pushInTreeInternal(root->left, toAppend, depth + 1);
            else
                root->right = pushInTreeInternal(root->right, toAppend, depth + 1);

            return root;
        }

        template<class F>
        static KDtreeNode* postOrderNodesTraverse(KDtreeNode* x, const F& f)
        {
            if (x == nullptr)
                return nullptr;
            KDtreeNode* leftCompletion = postOrderNodesTraverse(x->left, f);
            KDtreeNode* rightCompletion = postOrderNodesTraverse(x->right, f);
            return f(leftCompletion, rightCompletion, x);
        }

#if 0
        template<class F>
        static void inorderKeyTraverseWithDepth(KDtreeNode* x, const F& f, size_t depth)
        {
            if (x == nullptr)
                return;
            inorderKeyTraverseWithDepth(x->left, f, depth + 1);
            f(x, depth);
            inorderKeyTraverseWithDepth(x->right, f, depth + 1);
        }
#endif

        template<class F>
        static void inorderLeafsTraverseWithDepth(KDtreeNode* x, const F& f, size_t depth)
        {
            if (x->left == nullptr && x->right == nullptr)
                f(x, depth);
            else if (x->left)
                inorderLeafsTraverseWithDepth(x->left, f, depth + 1);
            else if (x->right)
                inorderLeafsTraverseWithDepth(x->right, f, depth + 1);
        }

    private:
        KDtreeNode* top;         ///< Pointer to the root of the tree (depth 0)
        size_t numPoints;        ///< Number of points in data structure
        size_t numDisablePoints; ///< Number of points temporary disabled in data structure
        Cmp cmp;                 ///< Used comparator
    };
}
