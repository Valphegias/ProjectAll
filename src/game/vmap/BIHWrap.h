/*
 * This code is part of MaNGOS. Contributor & Copyright details are in AUTHORS/THANKS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

#pragma once

#include <G3D/Table.h>
#include <G3D/Array.h>
#include <G3D/Set.h>
#include "BIH.h"

template<class T, class BoundsFunc = BoundsTrait<T> >
/**
 * @brief
 *
 */
class BIHWrap
{
        template<class RayCallback>
        /**
         * @brief
         *
         */
        struct MDLCallback
        {
            const T* const* objects; /**< TODO */
            RayCallback& cb; /**< TODO */

            /**
             * @brief
             *
             * @param callback
             * @param constobjects_array
             */
            MDLCallback(RayCallback& callback, const T* const* objects_array) : cb(callback), objects(objects_array) {}

            /**
             * @brief
             *
             * @param r
             * @param Idx
             * @param MaxDist
             * @param bool
             * @return bool operator
             */
            bool operator()(const Ray& r, uint32 Idx, float& MaxDist, bool /*stopAtFirst*/)
            {
                if (const T* obj = objects[Idx])
                    { return cb(r, *obj, MaxDist/*, stopAtFirst*/); }
                return false;
            }

            /**
             * @brief
             *
             * @param p
             * @param Idx
             */
            void operator()(const Vector3& p, uint32 Idx)
            {
                if (const T* obj = objects[Idx])
                    { cb(p, *obj); }
            }
        };

        /**
         * @brief
         *
         */
        typedef G3D::Array<const T*> ObjArray;

        BIH m_tree; /**< TODO */
        ObjArray m_objects; /**< TODO */
        G3D::Table<const T*, uint32> m_obj2Idx; /**< TODO */
        G3D::Set<const T*> m_objects_to_push; /**< TODO */
        int unbalanced_times; /**< TODO */

    public:

        /**
         * @brief
         *
         */
        BIHWrap() : unbalanced_times(0) {}

        /**
         * @brief
         *
         * @param obj
         */
        void insert(const T& obj)
        {
            ++unbalanced_times;
            m_objects_to_push.insert(&obj);
        }

        /**
         * @brief
         *
         * @param obj
         */
        void remove(const T& obj)
        {
            ++unbalanced_times;
            uint32 Idx = 0;
            const T* temp;
            if (m_obj2Idx.getRemove(&obj, temp, Idx))
                { m_objects[Idx] = NULL; }
            else
                { m_objects_to_push.remove(&obj); }
        }

        /**
         * @brief
         *
         */
        void balance()
        {
            if (unbalanced_times == 0)
                { return; }

            unbalanced_times = 0;
            m_objects.fastClear();
            m_obj2Idx.getKeys(m_objects);
            m_objects_to_push.getMembers(m_objects);

            m_tree.build(m_objects, BoundsFunc::getBounds2);
        }

        template<typename RayCallback>
        /**
         * @brief
         *
         * @param r
         * @param intersectCallback
         * @param maxDist
         */
        void intersectRay(const Ray& r, RayCallback& intersectCallback, float& maxDist) const
        {
            MDLCallback<RayCallback> temp_cb(intersectCallback, m_objects.getCArray());
            m_tree.intersectRay(r, temp_cb, maxDist, true);
        }

        template<typename IsectCallback>
        /**
         * @brief
         *
         * @param p
         * @param intersectCallback
         */
        void intersectPoint(const Vector3& p, IsectCallback& intersectCallback) const
        {
            MDLCallback<IsectCallback> temp_cb(intersectCallback, m_objects.getCArray());
            m_tree.intersectPoint(p, temp_cb);
        }
};
