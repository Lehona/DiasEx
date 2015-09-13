#include <vector>
#include <string>
#include "boost/range.hpp"
#include "AST.hpp"
#include <boost/variant/apply_visitor.hpp>

namespace AST {

	struct dialog;

	void nspace::addDlg(dialog d) {
		dialogs.push_back(d);
	}

	void nspace::addNsp(nspace n) {
		nspaces.push_back(n);
	}
	
	template<class T>
	daedalus::daedalus(boost::iterator_range<T> const& t) {
		daed = std::string(t.begin(), t.end());
	}




}
