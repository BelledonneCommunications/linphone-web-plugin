/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#pragma once
#ifndef extended_variant_h__
#define extended_variant_h__

#include <APITypes.h>
#include <variant_list.h>
#include <variant_map.h>
#include <iterator>

namespace FB {
    namespace variant_detail {
        namespace conversion {
			template<class Cont>
			typename boost::enable_if<
				boost::mpl::and_<
					FB::meta::is_non_assoc_container<Cont>,
					boost::mpl::not_<
						boost::mpl::or_<
							boost::mpl::or_<
								boost::is_same<std::vector<variant>, Cont>,
								boost::is_same<std::map<std::string, variant>, Cont>
							>,
							boost::mpl::or_<
								boost::is_same<std::wstring, Cont>,
								boost::is_same<std::string, Cont>
							>
						>
					>
				>
				,variant>::type
				make_variant(const Cont& var) {
					return make_variant_list(var);
			}

			template<class Dict>
			typename boost::enable_if<
				boost::mpl::and_<
					FB::meta::is_pair_assoc_container<Dict>,
					boost::mpl::not_<
						boost::is_same<std::map<std::string, variant>, Dict>
					>
				>
				,variant>::type
				make_variant(const Dict& var) {
					return make_variant_map(var);
			}
		}
	}


    /// @brief Create a FB::VariantMap from any STL-style dictionary (i.e. exposes begin() and end())
    /// @param cont A STL-style container.
    /// @return A FB::VariantMap constructed from the cont.
    template<typename Dict>
    inline FB::VariantMap make_variant_map(const Dict& dict);
    /// @brief Create a FB::VariantMap from the range [begin, end).
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @return A FB::VariantMap containing variants constructed from the contents of the range.
    template<class InputIterator>
    inline FB::VariantMap make_variant_map(InputIterator begin, InputIterator end);
    /// @brief Fill a FB::VariantMap with the contents of the range [begin, end). 
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @param result An iterator to the begin of a VariantMap range big enough to hold last-first values.
    template<class InputIterator>
    inline void make_variant_map(InputIterator begin, InputIterator end, FB::VariantMap::iterator result);

    /// @brief Convert a FB::VariantMap to STL-style dictionary (i.e. supports value_type and insert-iterators).
    /// @return A Cont with the converted contents of the FB::VariantList.
    template<class Dict>
    inline Dict convert_variant_map(const FB::VariantMap& v);
    /// @brief Fill to with the contents of from, converted to Cont::value_type.
    /// @param from The input.
    /// @param to The container to store the converted values in.
    template<class Dict>
    inline void convert_variant_map(const FB::VariantMap& from, Dict& to);
    /// @brief Convert a range of FB::VariantMap to STL-style dictionary (i.e. supports value_type and insert-iterators).
    /// @return A Cont with the converted contents of the FB::VariantMap range.
    template<class Dict>
    inline Dict convert_variant_map(FB::VariantMap::const_iterator begin,
                              FB::VariantMap::const_iterator end);
    /// @brief Fills the range [result, result+(end-begin)) with the contents of the range [begin,end), converted to To
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @param result Out parameter, shall be an output iterator.
    template<typename To, class OutputIterator>
    inline void convert_variant_map(FB::VariantMap::const_iterator begin,
                              FB::VariantMap::const_iterator end,
                              OutputIterator result);



    template<class InputIterator>
    inline void make_variant_map(InputIterator begin, 
                           InputIterator end, 
                           FB::VariantMap::iterator result)
    {
        while(begin != end)
            *result++ = *begin++;
    }

    template<class InputIterator>
    inline FB::VariantMap make_variant_map(InputIterator begin, InputIterator end)
    {
        FB::VariantMap result(end-begin);
        std::copy(begin, end, result.begin());
        return result;
    }

    template<class Dict>
    inline FB::VariantMap make_variant_map(const Dict& d)
    {
        FB::VariantMap result;
        std::copy(d.begin(), d.end(), std::inserter(result, result.begin()));
        return result;
    }

    template<typename To, class OutputIterator>
    inline void convert_variant_map(FB::VariantMap::const_iterator begin,
                              FB::VariantMap::const_iterator end,
                              OutputIterator result)
    {
        while(begin != end){
            FB::VariantMap::const_iterator it = begin++;
            *result++ = To(it->first, it->second.convert_cast<typename To::second_type>());
        }
    }

    template<class Dict>
    inline Dict convert_variant_map(FB::VariantMap::const_iterator begin,
                              FB::VariantMap::const_iterator end)
    {
        Dict to;
        convert_variant_map<typename Dict::value_type>(begin, end, std::inserter(to, to.begin()));
        return to;
    }

    template<class Dict>
    inline Dict convert_variant_map(const FB::VariantMap& from)
    {
        Dict to;
        convert_variant_map<typename Dict::value_type>(from.begin(), from.end(), std::inserter(to, to.begin()));
        return to;
    }

    template<class Dict>
    inline void convert_variant_map(const FB::VariantMap& from, Dict& to)
    {
        convert_variant_map<typename Dict::value_type>(from.begin(), from.end(), std::inserter(to, to.begin()));
    }
}
#endif //extended_variant_h__
