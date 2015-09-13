
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include "boost/phoenix/phoenix.hpp"
#include "SimpleCalc.h"
#include "transform.h"
#include "Libs/bfeFile.h"


namespace help {
	namespace spirit = boost::spirit;


	template <typename Expr, typename Iterator = spirit::unused_type>
	struct attribute_of_parser
	{
		typedef typename spirit::result_of::compile<
			spirit::qi::domain, Expr
		>::type parser_expression_type;

		typedef typename spirit::traits::attribute_of<
			parser_expression_type, spirit::unused_type, Iterator
		>::type type;
	};

	template <typename T>
	void display_attribute_of_parser(T const&)
	{
		typedef typename attribute_of_parser<T>::type attribute_type;
		std::cout << typeid(attribute_type).name() << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
void parseDXtoD(std::string const& path) {

	typedef std::string::const_iterator iterator_type;
	typedef DiasEx::gram<iterator_type> grammar;
	namespace qi = boost::spirit::qi;

	grammar gram; // Our grammar

	auto &file = bfe::File{ path }.bytes;
	auto str = std::string{ file.begin(), file.end()};
	auto iter = str.begin();
	auto end = str.end();
	AST::transform transformer;
	AST::nspace ast{ "" };


	bool r = phrase_parse(iter, end, gram, boost::spirit::ascii::space, ast);

	if (r && iter == end) {
		std::cout << "parsing succeeded. See output file.\n"; 
		auto output = transformer(ast);
		bfe::File::write("output.txt", { output.begin(), output.end() });
	}
	else {
		std::cout << "parsing failed at: " + std::string{iter,end} + "\n";
	}
}

int main()
{
	std::cout << "Insert path to sourcefile: ";
		
	std::string str;
	while (std::getline(std::cin, str))
	{
		parseDXtoD(str);
	}

	std::cout << "Bye... :-) \n\n"; 
	
	return 0;
}