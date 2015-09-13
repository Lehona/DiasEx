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
	struct nil {};
	struct dialog;

	struct nspace
	{

		std::string name;
		std::vector<dialog> dialogs;
		std::vector<nspace> nspaces;

		nspace() {}
		nspace(std::string s) : name(s) {}

		void setName(std::string s) { name = s; }

		void addDlg(dialog d);
		void addNsp(nspace n);


	};
	
	struct daedalus 
	{
		std::string daed;

		daedalus(std::string const& d) : daed(d) {}
		template <typename T>
		daedalus(boost::iterator_range<T> const& t);
		daedalus() {}

	};




	struct output
	{
		bool hero;
		std::string cont;

		output(std::string const& said, bool const& her) : cont(said), hero(her) {}

		output() {}

	};
	


	struct dialog
	{	
		typedef  boost::variant<output, daedalus> statement_type;

		std::string name;
		std::vector<statement_type> content;

		


		void addStatement(statement_type const& state);
		dialog(std::string n, std::vector<statement_type> stuff) : name(n), content(stuff) { }

		dialog() {}

	};

	struct print
	{
		typedef void result_type;

		void operator()(nil) const {}
		void operator()(nspace n) const
		{
			std::cout << "ns(" << n.name << ", ";
			std::for_each(n.dialogs.begin(), n.dialogs.end(), *this);
			std::for_each(n.nspaces.begin(), n.nspaces.end(), *this);
			std::cout << ")";
		}
		void operator()(dialog d) const
		{
			std::cout << "dlg(" << d.name << ", ";
			std::for_each(d.content.begin(), d.content.end(), boost::apply_visitor(*this));
			std::cout << ")";
		}
		void operator()(daedalus d) const
		{
			std::cout << "d(" << d.daed << ")";
		}
		void operator()(output o) const
		{
			std::cout << "out(" << o.cont << ")";
		}
			
	};

}
