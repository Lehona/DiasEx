#include <vector>
#include <string>
#include "boost/range.hpp"
#include "AST.hpp"
#include <boost/variant/apply_visitor.hpp>

namespace AST {
	struct dialog;

	void nspace::addDlg(dialog d) { dialogs.emplace_back(std::move(d)); }

	void nspace::addNsp(nspace n) { nspaces.emplace_back(std::move(n)); }

	template <class T> daedalus::daedalus(boost::iterator_range<T> const &t)
	{
		daed = {t.begin(), t.end()};
	}
}
