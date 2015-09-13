#include <string>
#include "transform.h"
#include "boost/variant.hpp"
#include "boost/algorithm/string/replace.hpp"

namespace AST {
	std::string transform::operator()(dialog const& d) {
		m_context.add(d.name);

		auto ret = std::string("");

#ifdef EXTENSIVEFORMATTING
		ret += "//************************************\n";
		ret += "//******* dialog " + d.name + "\n";
		ret += "//************************************\n\n";
#endif		
		ret += m_context.buildC_InfoString();
		ret += "\n\nfunc void "+m_context.buildDIAidentifier()+"_info() {\n";

		for (auto v : d.content) {
			ret += "\t" + boost::apply_visitor(*this, v) + "\n";
		}

		ret += "};\n\n";

		m_context.rewind();

		return ret;
	}

	std::string transform::operator()(nspace const& n) {
		m_context.add(n.name);

		std::string ret = "";

#ifdef EXTENSIVEFORMATTING
		ret += "//************************************\n";
		ret += "//******* namespace " + n.name + "\n";
		ret += "//************************************\n\n";
#endif

		for (auto d : n.dialogs) {
			ret += operator()(d);
		}

		for (auto d : n.nspaces) {
			ret += operator()(d);
		}
		
#ifdef EXTENSIVEFORMATTING
		ret += "//************************************\n";
		ret += "//******* namespace " + n.name + "end \n";
		ret += "//************************************\n\n";
#endif
		m_context.rewind();

		return ret;
	}

	std::string transform::operator()(daedalus const& d) {
		return d.daed;
	}

	std::string transform::operator()(output const& o) {
		std::string ret = "AI_Output(";
		if (o.hero)
			ret += "other, self,  \"";
		else
			ret += "self, other, \"";
		ret += m_context.buildOUString(o.hero);
		ret += "\"); //";
		ret += o.cont;

		return ret;
	}

	std::string context::getPrefix() {
		return "DX_"+boost::replace_all_copy(name, "::", "_");
	}

	std::string context::buildOUString(bool hero) {
		std::string ret = getPrefix();
		
		if (hero)
			ret += "_H_";
		else
			ret += "_O_";

		ret += std::to_string(nextOUnumber());
		return ret;
	}

	std::string context::buildDIAidentifier() 
	{
		return getPrefix(); // the contextname does include the dialog name, thus it's fit to be used as identifier
	}

	std::string context::buildC_InfoString() //TODO: add stuff like npc, nr
	{
		auto ident = buildDIAidentifier();
		return std::string{ "instance " + ident + "(C_Info) {\n"
			"\tnpc = NONE_100_Xardas;\n"
			"\tnr = 999;\n"
			"\tdescription = DIALOG_BACK;\n"
			"\tinfo = "+ident+"_info;\n"
			"};" };
	}

	void context::add(std::string const& cont) {
		if (cont.empty()) return;
		if (name.empty()) 
			name += cont;  // I want foo::bar::dialog, not ::foo::bar::dialog
		else
			name += "::" + cont;
	}

	void context::rewind(bool restart) {
		if (restart) {
			OUnumber = 0;
		}

		name = name.substr(0, name.find_last_of("::")-3); //TODO: Test

	}
}