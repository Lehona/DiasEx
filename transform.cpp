#include <string>
#include "AST.hpp"
#include "boost/variant.hpp"
#include "boost/algorithm/string/replace.hpp"

using namespace AST;
struct DlgContentTrans {
	typedef std::string result_type;

	int OUNumber = 0;
	std::string fullName;

	DlgContentTrans(std::string name) : fullName{std::move(name)} {};

	std::string operator()(daedalus d) {
		return d.daed;
	}

	std::string operator()(output o) {
		std::string ret = "AI_Output(";
		if (o.hero)
			ret += "other, self, \"";
		else
			ret += "self, other, \"";
		ret += buildOUString(o.hero);
		ret += "\"); //";
		ret += o.cont;
		return ret;
	}

	std::string buildOUString(bool hero) {
		auto ret = fullName;

		if (hero)
			ret += "_H_";
		else
			ret += "_O_";

		ret += std::to_string(OUNumber++);

		return ret;
	}
};

std::string dialog::transform()
{
	auto ret = std::string{};

#ifdef EXTENSIVEFORMATTING
	ret += "//************************************\n";
	ret += "//******* dialog " + name + "\n";
	ret += "//************************************\n\n";
#endif

	std::string ident = getFullName();
	ret += "instance " + ident + "(C_Info) {\n"
		"\tnpc = " + getAttribute(attribute_type::npc) + ";\n"
		"\tnr = 999;\n"
		"\tdescription = DIALOG_BACK;\n"
		"\tcondition = " + getAttribute(attribute_type::condition) + ";\n"
		"\tinfo = " +	ident + "_info;\n"
		"};";
	ret += "\n\nfunc void " + ident + "_info() {\n";

	DlgContentTrans t(ident);
	for(auto &v : content) {
		ret += "\t" + boost::apply_visitor(t, v) + "\n";
	}

	ret += "};\n\n";

	return ret;
} 

std::string nspace::transform()
{	
	auto ret = std::string{""};

#ifdef EXTENSIVEFORMATTING
	ret += "//************************************\n";
	ret += "//******* namespace " + name + "\n";
	ret += "//************************************\n\n";
#endif
	for (auto s : children) {
		s->setParent(this);
		ret += s->transform();
	}


#ifdef EXTENSIVEFORMATTING
	ret += "//************************************\n";
	ret += "//******* namespace " + name+ " end \n";
	ret += "//************************************\n\n";
#endif

	return ret;
}


