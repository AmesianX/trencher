/*
 * ----------------------------------------------------------------------------
 * "THE JUICE-WARE LICENSE" (Revision 42):
 * <derevenetc@cs.uni-kl.de> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a glass of juice in return.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

namespace trench {

template<class Key, class Value, class Vector = std::vector<std::pair<Key, Value>>>
class SmallMap {
	Vector vector_;
	std::size_t hash_;
public:
	typedef typename Vector::iterator iterator;
	typedef typename Vector::const_iterator const_iterator;
	typedef typename Vector::value_type value_type;

	iterator begin() { return vector_.begin(); }
	iterator end() { return vector_.end(); }

	const_iterator cbegin() const { return vector_.cbegin(); }
	const_iterator cend() const { return vector_.cend(); }

	const_iterator begin() const { return vector_.begin(); }
	const_iterator end() const { return vector_.end(); }

	SmallMap(): hash_(0) {}

	const_iterator lower_bound(const Key &key) const {
		return std::lower_bound(
			vector_.begin(),
			vector_.end(),
			key,
			[](const value_type &item, const Key &key){
				return item.first < key;
			}
		);
	}

	void set(const Key &key, Value value) {
		auto i = lower_bound(key);
		if (i == vector_.end() || i->first != key) {
			if (value != Value()) {
				vector_.insert(i, std::make_pair(key, std::move(value)));
				hash_ ^= boost::hash_value(std::make_pair(key, value));
			}
		} else {
			hash_ ^= boost::hash_value(*i);
			if (value != Value()) {
				const_cast<Value &>(i->second) = std::move(value);
				hash_ ^= boost::hash_value(std::make_pair(key, value));
			} else {
				vector_.erase(i);
			}
		}
	}

	Value get(const Key &key) const {
		auto i = lower_bound(key);
		if (i != vector_.end() && i->first == key) {
			return i->second;
		} else {
			return Value();
		}
	}

	template<class Pred>
	void filterOut(Pred pred) {
		vector_.erase(
			std::remove_if(vector_.begin(), vector_.end(), [&](const value_type &item) {
				if (pred(item.first)) {
					hash_ ^= boost::hash_value(item);
					return true;
				} else {
					return false;
				}
			}),
			vector_.end());
	}

	std::size_t hash() const {
		return hash_;
	}

	const Vector &vector() const { return vector_; }
};

template<class Key, class Value, class Vector>
inline bool operator==(const SmallMap<Key, Value, Vector> &a, const SmallMap<Key, Value, Vector> &b) {
	return a.hash() == b.hash() && a.vector() == b.vector();
}

template<class Key, class Value, class Vector>
inline std::size_t hash_value(const trench::SmallMap<Key, Value, Vector> &map) {
	return map.hash();
}

} // namespace trench
