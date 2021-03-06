/*
 * ----------------------------------------------------------------------------
 * "THE JUICE-WARE LICENSE" (Revision 42):
 * <derevenetc@cs.uni-kl.de> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a glass of juice in return.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <ostream>

#include <boost/unordered_map.hpp>

#include "Dfs.h"

namespace trench {

template<class Automaton>
class DotPrinter: public EmptyDfsVisitor<Automaton> {
	const Automaton &automaton_;
	std::ostream &out_;
	boost::unordered_map<typename Automaton::State, std::size_t> state2id_;

public:
	DotPrinter(const Automaton &automaton, std::ostream &out):
		automaton_(automaton), out_(out)
	{}

	bool onStateEnter(const typename Automaton::State &state) {
		out_ << 's' << getId(state) << " [shape=ellipse,label=\"" << automaton_.getName(state) << "\"];" << std::endl;

		if (automaton_.initialState() == state) {
			out_ << "initial [shape=none,label=\"\"];" << std::endl;
			out_ << "initial -> s" << getId(state) << ';' << std::endl;
		}
		return false;
	}

	bool onTransition(const typename Automaton::Transition &transition) {
		out_ << 's' << getId(automaton_.getSourceState(transition))
		     << " -> "
		     << 's' << getId(automaton_.getDestinationState(transition))
		     << " [label=\"" << automaton_.getLabel(transition) << "\"];" << std::endl;
		return false;
	}

private:
	std::size_t getId(const typename Automaton::State &state) {
		auto &result = state2id_[state];
		if (result == 0) {
			result = state2id_.size();
		}
		return result;
	}
};

template<class Automaton>
void printAutomatonAsDot(const Automaton &automaton, std::ostream &out) {
	out << "digraph {" << std::endl;

	DotPrinter<Automaton> visitor(automaton, out);
	dfs<Automaton, DotPrinter<Automaton> &>(automaton, visitor);

	out << "}" << std::endl;
}

} // namespace trench
