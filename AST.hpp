#pragma once

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/range/iterator_range.hpp>

#include <iostream>
#include <vector>
#include <string>

namespace AST {
	struct dialog;
	struct output;
	struct attribute;
	struct daedalus;
	enum class attribute_type;


	struct symbol {
		boost::optional<symbol*> parent{boost::none};
		std::vector<attribute> attributes;
		std::string name;

		symbol() = default;
		symbol(std::string n) : name{std::move(n)} {};
		symbol(std::vector<attribute> attr) : attributes{attr} {}
		symbol(std::string n, std::vector<attribute> attr) : name{std::move(n)}, attributes{attr} {}

		symbol(symbol const& s) : attributes(s.attributes), name(s.name) { /*if (auto p = s.parent.get()) parent = p; */ }
		std::string getFullName();
		std::string getAttribute(attribute_type);
		void setParent(symbol* sym) {
			parent = sym;
		}

		virtual std::string transform() { return "TEST"; }         

	};

	struct nspace : public symbol {
	public:
		std::vector<symbol*> children;
		

		nspace() = default;
		nspace(std::string name) : symbol{std::move(name)} {}
		nspace(std::string name, std::vector<attribute> attr) : symbol{std::move(name), attr} {}

		nspace(nspace const& n) : symbol(n), children(n.children) {  };
		std::string transform();

	};


	struct dialog : public symbol {
		typedef boost::variant<output, daedalus> statement_type;

		std::vector<statement_type> content;

		dialog() = default;
		dialog(std::string name, std::vector<attribute> attr,
			std::vector<statement_type> content)
			: symbol{std::move(name), attr}, content(std::move(content))
		{
		}

		dialog(dialog const& d) : symbol(d), content(d.content) {  };

		std::string transform();

	};


	struct output {
		std::string cont;
		bool hero;

		output() = default;
		output(std::string cont, bool hero) : cont{std::move(cont)}, hero{hero} {}
	};

	struct daedalus {
		std::string daed;

		daedalus() = default;
		daedalus(std::string daed) : daed{ std::move(daed) } {}
		template <typename T> daedalus(boost::iterator_range<T> const &t)
		{
			daed = { t.begin(), t.end() };
		}
	};

	enum class attribute_type {
		npc,
		condition,
	};

	struct attribute {
	public:
		attribute_type type;
		std::string content;

		attribute() = default;
		attribute(attribute_type type, std::string content)
			: type{ type }, content{ std::move(content) }
		{
		}

		static std::string getAttribute(std::vector<attribute> const& attr, attribute_type type);
		static void setAttribute(std::vector<attribute> & attr, attribute_type type, std::string const& value);
	};

	
	
	/*struct print {
		typedef void result_type;

		void operator()(nspace const &n) const
		{
			std::cout << "ns(" << n.name << ", ";
			//std::for_each(n.dialogs.begin(), n.dialogs.end(), *this);
			//std::for_each(n.nspaces.begin(), n.nspaces.end(), *this);
			std::cout << ")";
		}
		void operator()(dialog const &d) const
		{
			std::cout << "dlg(" << d.name << ", ";
			std::for_each(d.content.begin(), d.content.end(),
			              boost::apply_visitor(*this));
			std::cout << ")";
		}
		void operator()(daedalus const &d) const
		{
			std::cout << "d(" << d.daed << ")";
		}
		void operator()(output const &o) const
		{
			std::cout << "out(" << o.cont << ")";
		}
	};*/
}
