#include <string>
#include "AST.hpp"

using namespace AST;


std::string symbol::getFullName() {
	auto ret = std::string{};
	if (parent != boost::none)
		ret += parent.get()->getFullName();
	if (name.empty()) return "";
	if (!ret.empty()) ret += "_";
	return ret +  name;
}

std::string symbol::getAttribute(attribute_type type) {
	auto ret = std::string{attribute::getAttribute(attributes, type)};
	if (!ret.empty()) return ret;

	if (parent != boost::none)
		return parent.get()->getAttribute(type);

		/* error */
	return "";
}


std::string attribute::getAttribute(std::vector<attribute> const& attr, attribute_type type) {
	auto ret = std::string{};
	for (auto a : attr) {
		if (a.type == type) {
			return a.content;
		}
	}
	return "";
}

void attribute::setAttribute(std::vector<attribute> & attr, attribute_type type, std::string const& value) {
	for (auto a : attr) {
		if (a.type == type) 
			a.content = value;
			return;
	}
	attr.emplace_back(type, value);
}
