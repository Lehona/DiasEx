#pragma once

#include <string>
#include <vector>
#include "AST.hpp"

#define EXTENSIVEFORMATTING

namespace AST {

	struct context;

	struct contextStack {
		std::vector<context> contexts;

		contextStack() = default;
		contextStack(std::string const& name) {contexts.emplace_back(std::move(name)); }

		int nextOUnumber();

		void add(std::string const &cname);
		void add(dialog const& d);
		void add(nspace const& n);
		void rewind();

		std::string getPrefix();
		std::string buildDIAidentifier();
		std::string buildOUString(bool);
		std::string buildC_InfoString();

		std::string getAttribute(attribute_type);
	};

	struct context {
			std::string name;
			
			/*identifier*/
			std::string npc;
			std::string cond;
			/*...*/

			int OUnumber = 0;

			context(std::string name) : name{ std::move(name) } {}
			context(std::string name, std::vector<attribute> attrs);
			context(dialog const& d) : context{d.name, d.attributes} {}
			context(nspace const& d) : context{d.name, d.attributes} {}

			bool hasAttribute(attribute_type);
			std::string getAttribute(attribute_type);
			void setAttribute(attribute_type, std::string);
	};

	struct transform {
		typedef std::string result_type;

		contextStack m_context;

		transform() = default;
		transform(std::string c) : m_context{std::move(c)} {}

		std::string operator()(dialog const &d);
		std::string operator()(nspace const &n);
		std::string operator()(daedalus const &d);
		std::string operator()(output const &o);
	};
}