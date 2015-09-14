#include <string>
#include "transform.h"
#include "boost/variant.hpp"
#include "boost/algorithm/string/replace.hpp"

namespace AST {
	std::string transform::operator()(dialog const &d)
	{
		m_context.add(d);

		auto ret = std::string{};

#ifdef EXTENSIVEFORMATTING
		ret += "//************************************\n";
		ret += "//******* dialog " + d.name + "\n";
		ret += "//************************************\n\n";
#endif

		ret += m_context.buildC_InfoString();
		ret += "\n\nfunc void " + m_context.buildDIAidentifier() + "_info() {\n";

		for(auto &v : d.content) {
			ret += "\t" + boost::apply_visitor(*this, v) + "\n";
		}

		ret += "};\n\n";

		m_context.rewind();

		return ret;
	}

	std::string transform::operator()(nspace const &n)
	{
		m_context.add(n.name);

		auto ret = std::string{};

#ifdef EXTENSIVEFORMATTING
		ret += "//************************************\n";
		ret += "//******* namespace " + n.name + "\n";
		ret += "//************************************\n\n";
#endif

		for(auto &d : n.dialogs) {
			ret += operator()(d);
		}

		for(auto &d : n.nspaces) {
			ret += operator()(d);
		}

#ifdef EXTENSIVEFORMATTING
		ret += "//************************************\n";
		ret += "//******* namespace " + n.name + " end \n";
		ret += "//************************************\n\n";
#endif
		m_context.rewind();

		return ret;
	}

	std::string transform::operator()(daedalus const &d) { return d.daed; }

	std::string transform::operator()(output const &o)
	{
		std::string ret = "AI_Output(";
		if(o.hero)
			ret += "other, self, \"";
		else
			ret += "self, other, \"";
		ret += m_context.buildOUString(o.hero);
		ret += "\"); //";
		ret += o.cont;

		return ret;
	}

	std::string contextStack::getPrefix()
	{
		auto ret = std::string{"DX"};
		for (context c : contexts) {
			if (c.name.empty()) continue;
			ret += "_"+c.name;
		}
		return ret;
	}

	std::string contextStack::buildOUString(bool hero)
	{
		auto ret = getPrefix();

		if(hero)
			ret += "_H_";
		else
			ret += "_O_";

		ret += std::to_string(nextOUnumber());

		return ret;
	}

	std::string contextStack::buildDIAidentifier()
	{
		return getPrefix(); // the contextname does include the dialog name, thus
		                    // it's fit to be used as identifier
	}

	std::string contextStack::buildC_InfoString() // TODO: add stuff like npc, nr
	{
		auto ident = buildDIAidentifier();
		return {"instance " + ident + "(C_Info) {\n"
		                              "\tnpc = "+getAttribute(attribute_type::npc)+";\n"
		                              "\tnr = 999;\n"
		                              "\tdescription = DIALOG_BACK;\n"
																	"\tcondition = "+getAttribute(attribute_type::condition)+";\n"
		                              "\tinfo = " +
		        ident + "_info;\n"
		                "};"};
	}

	void contextStack::add(std::string const &cont)
	{
		contexts.emplace_back(cont);
	}

	void contextStack::add(dialog const& d) {
		contexts.emplace_back(d);
	}

	void contextStack::rewind()
	{
		contexts.pop_back();
	}

	int contextStack::nextOUnumber() {
		return contexts.back().OUnumber++;
	}

	std::string contextStack::getAttribute(attribute_type attr) {
		auto ret = std::string{};
		for (auto c : contexts)
		{
			if (c.hasAttribute(attr))
				ret = c.getAttribute(attr);
		}
		return ret;
	}


	context::context(std::string name, std::vector<attribute> attr) : context{ std::move(name) }
	{
		for (auto a : attr) 
		{
			setAttribute(a.type, a.content);
		}
	}

	bool context::hasAttribute(attribute_type type)
	{
		return !getAttribute(type).empty();
	}

	std::string context::getAttribute(attribute_type type) 
	{
		switch(type) {
			case attribute_type::npc:
				return npc;
			case attribute_type::condition:
				return cond;
			default: /*Error*/ return "";
		}
	}

	void context::setAttribute(attribute_type type, std::string n) {
		switch (type) {
			case attribute_type::npc:
				npc = n;  break;
			case attribute_type::condition:
				cond = n; break;
			default: /*Error*/ break;
		}
	}
}