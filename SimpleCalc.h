#pragma once

#include <boost/spirit/include/qi.hpp>

#include "AST.hpp"

#include <iostream>
#include <vector>
#include <string>

BOOST_FUSION_ADAPT_STRUCT(AST::output, hero, cont)

BOOST_FUSION_ADAPT_STRUCT(AST::daedalus, daed)

BOOST_FUSION_ADAPT_STRUCT(AST::dialog, name, content)

BOOST_FUSION_ADAPT_STRUCT(AST::attribute, type, content)

namespace DiasEx {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	namespace phx = boost::phoenix;
	namespace fus = boost::fusion;

	template <typename Iterator>
	struct gram : qi::grammar<Iterator, AST::nspace(), ascii::space_type> {
		qi::rule<Iterator, AST::nspace(), ascii::space_type> start;

		qi::rule<Iterator, AST::nspace(), ascii::space_type> nmspace;

		qi::rule<Iterator, AST::dialog(), ascii::space_type> dlg;

		typedef boost::variant<AST::output, AST::daedalus> statement_type;
		qi::rule<Iterator, statement_type(), ascii::space_type> statement;

		qi::rule<Iterator, AST::output(), ascii::space_type> output;

		qi::rule<Iterator, AST::daedalus(), ascii::space_type> daedSingle, daedLine;
		qi::rule<Iterator, std::string()> nestedBrackets;
    
		qi::rule<Iterator, AST::attribute(), ascii::space_type> attribute;
		qi::rule<Iterator, std::vector<AST::attribute>(), ascii::space_type>
		    specialAttr;

		qi::rule<Iterator, std::string(), ascii::space_type> quoted_string,
		    identifier;

		struct attribute_type_ : qi::symbols<char, AST::attribute_type> {
			attribute_type_()
			{
				add("npc", AST::attribute_type::npc);
				add("cond", AST::attribute_type::condition);
			}
		} attribute_type;

		gram() : base_type(start)
		{
			using qi::_val;
			using qi::_1;
			using qi::_2;
			using qi::char_;
			using qi::double_;
			using qi::lexeme;
			using qi::alpha;
			using qi::lit;
			using qi::_a;

			quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];

			identifier %= lexeme[+alpha];

			start = +(nmspace[phx::bind(&AST::nspace::addNsp, _val, _1)] |
			          dlg[phx::bind(&AST::nspace::addDlg, _val,
			                        _1)]); // "Global" namespace

			nmspace = "namespace" >>
			          identifier[phx::bind(&AST::nspace::setName, _val, _1)] >>
			          -specialAttr >> '{' >>
			          +(nmspace[phx::bind(&AST::nspace::addNsp, _val, _1)] |
			            dlg[phx::bind(&AST::nspace::addDlg, _val, _1)]) >>
			          '}';
      
			attribute = attribute_type >> '=' >> identifier;

			specialAttr = '[' >> +(attribute % ',')[_val = _1] >> ']';
      // help::display_attribute_of_parser('[' >> +(attribute % ',') >> ']');

			daedLine %= "##" >> (qi::raw[lexeme[+(char_ - qi::eol)]] >> qi::eol);

			daedSingle %= ("#(" >> (nestedBrackets) >> ')');
			nestedBrackets %=
			    qi::no_skip[((char_(' ', '~') - char_('(', ')')) >> nestedBrackets) |
			                (char_('(') >> nestedBrackets >> char_(')') >>
			                 nestedBrackets)] |
			    qi::attr("");

			dlg %= "dialog" >> identifier >> -specialAttr >> '{' >> +statement >> '}';

			output %= (">>" >> qi::attr(true) >> quoted_string >> ';') |
			          ("<<" >> qi::attr(false) >> quoted_string >> ';');

			statement = output | (daedSingle) | (daedLine);
		}
	};
}
