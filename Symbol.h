#pragma once

#include <boost/variant.hpp>
#include "AST.hpp"

#include <vector>
#include <string>

namespace AST {

	struct nspace;
	struct dialog;



	struct symbolTable {
		std::vector<symbol> symbols;


		void add(symbol const& s) {
			symbols.emplace_back(s);
		}

	};


	struct symbolName_ {
		typedef std::string result_type;

		std::string operator()(nspace* n);
		std::string operator()(dialog* d);
	};
}