#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/operator/self.hpp>

#include "AST.hpp"
#include "Symbol.h"

#include <iostream>
#include <vector>
#include <string>

BOOST_FUSION_ADAPT_STRUCT(AST::output, hero, cont)

BOOST_FUSION_ADAPT_STRUCT(AST::dialog, name, attributes, content)

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
			qi::rule<Iterator, AST::dialog(),  ascii::space_type> dlg;

		typedef boost::variant<AST::output, AST::daedalus> statement_type;
		qi::rule<Iterator, statement_type(), ascii::space_type> statement;

		qi::rule<Iterator, AST::output(), ascii::space_type> output;

		qi::rule<Iterator, std::string(), ascii::space_type> daedSingle, daedLine;
		qi::rule<Iterator, std::string()> nestedBrackets;
    
		qi::rule<Iterator, AST::attribute(), ascii::space_type> attribute;
		qi::rule<Iterator, std::vector<AST::attribute>(), ascii::space_type>
		    specialAttr;

		qi::rule<Iterator, std::string(), ascii::space_type> value;
		qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
		qi::rule<Iterator, std::string(), ascii::space_type> identifier;
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
      using qi::alnum;
			using qi::lit;
			using qi::_a;

      using phx::push_back;
      using phx::bind;
			

			quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];

			identifier %= lexeme[alpha >> *(alnum | char_('_'))];

			value %= identifier | quoted_string | daedSingle | daedLine; /* Ich hoffe, niemand nutzt daedLine, aber aus Konsistenzgründen will ich es erlauben */

			start = +(nmspace[push_back(bind(&AST::nspace::children, _val), phx::new_<AST::nspace>(_1))]
								| dlg  [push_back(bind(&AST::nspace::children, _val), phx::new_<AST::dialog>(_1))]);

			nmspace = "namespace" 
						>> identifier[phx::bind(&AST::nspace::name, _val) = _1] 
						 >> -(specialAttr[bind(&AST::nspace::attributes, _val) = _1])
						>> '{'
						>> +(nmspace[push_back(bind(&AST::nspace::children, _val), phx::new_<AST::nspace>(_1))]
									| dlg [push_back(bind(&AST::nspace::children, _val), phx::new_<AST::dialog>(_1))])
						>> '}';
      
			attribute = attribute_type >> '=' >> value;

			specialAttr = '[' >> (attribute % ',') >> ']';

			daedLine %= "##" >> (qi::raw[lexeme[+(char_ - qi::eol)]] >> qi::eol);

			daedSingle %= ("#(" >> (nestedBrackets) >> ')');
			nestedBrackets %= qi::no_skip[

					((char_(' ', '~') - char_('(', ')')) 
					>> nestedBrackets) 
				
				|	  (char_('(') >> nestedBrackets 
					>> char_(')') 
					>> nestedBrackets)]
			|   qi::attr("");

			dlg %= "dialog"  >> identifier >> -specialAttr >> '{' >> +statement >> '}';

			output %= (">>" >> qi::attr(true) >> quoted_string >> ';')
					| ("<<" >> qi::attr(false) >> quoted_string >> ';');

			statement = output 
				| (daedSingle) 
				| (daedLine);
		}
	};
}
