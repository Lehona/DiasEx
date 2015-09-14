#pragma once

#include <boost/variant.hpp>
#include <boost/range/iterator_range.hpp>

#include <iostream>
#include <vector>
#include <string>

namespace AST {
	///////////////////////////////////////////////////////////////////////////
	//  Our AST
	///////////////////////////////////////////////////////////////////////////
	struct binary_op;
	struct unary_op;
	struct nil {
	};
	struct dialog;

	struct daedalus {
		std::string daed;

		daedalus() = default;
		daedalus(std::string daed) : daed{std::move(daed)} {}
		template <typename T> daedalus(boost::iterator_range<T> const &t);
	};

  enum class attribute_type {
    npc,
    condition,
  };

  struct attribute {
  public:
	attribute_type type__;
    std::string content;

    attribute() = default;
    attribute(attribute_type type, std::string content)
      : type__{type}, content{std::move(content)} {}
  };

	struct nspace {
		std::string name;
		std::vector<dialog> dialogs;
		std::vector<nspace> nspaces;
	  std::vector<attribute> attributes;

		nspace() = default;
		nspace(std::string name) : name{std::move(name)} {}

		void setName(std::string n) { name = std::move(n); }

		void addDlg(dialog d);
		void addNsp(nspace n);
	};

	struct output {
		std::string cont;
		bool hero;

		output() = default;
		output(std::string cont, bool hero) : cont{std::move(cont)}, hero{hero} {}
	};

	struct dialog {
		typedef boost::variant<output, daedalus> statement_type;

		std::string name;
		std::vector<statement_type> content;

		dialog() = default;
		dialog(std::string name, std::vector<statement_type> content)
		    : name(std::move(name)), content(std::move(content))
		{
		}

		void addStatement(statement_type const &state);
	};

	struct print {
		typedef void result_type;

		void operator()(nil const &) const {}
		void operator()(nspace const &n) const
		{
			std::cout << "ns(" << n.name << ", ";
			std::for_each(n.dialogs.begin(), n.dialogs.end(), *this);
			std::for_each(n.nspaces.begin(), n.nspaces.end(), *this);
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
	};
}
