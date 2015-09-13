#pragma once

#include <string>
#include "AST.hpp"

#define EXTENSIVEFORMATTING

namespace AST {
	struct context {
		std::string name;
		int OUnumber = 0;

		context() = default;
		context(std::string name) : name{std::move(name)} {}

		inline int nextOUnumber() { return OUnumber++; }

		void add(std::string const &cname);
		void rewind(bool restartOU = false);

		std::string getPrefix();
		std::string buildDIAidentifier();
		std::string buildOUString(bool);
		std::string buildC_InfoString();
	};

	struct transform {
		typedef std::string result_type;

		context m_context;

		transform() = default;
		transform(std::string c) : m_context{std::move(c)} {}

		std::string operator()(dialog const &d);
		std::string operator()(nspace const &n);
		std::string operator()(daedalus const &d);
		std::string operator()(output const &o);
	};
}