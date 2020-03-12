//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "url.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "utils/logger.h"
//
// C++ 
#include <sstream>
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace trans {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("Url");

static const std::string PROTO_DELIM = "://";

//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------	
//
Url::Url(const std::string& a_str):
	proto(),
	host(),
	path(),
	query(),
	fragment()
{
	from_string(a_str);
}

//------------------------------------------------------------------------------	
//
Url::~Url()
{
}

//------------------------------------------------------------------------------	
//
void Url::from_string(const std::string& a_str)
{
	proto.clear();
	size_t i = a_str.find(PROTO_DELIM);
	if (i != std::string::npos) {
		size_t l = 0;
		size_t r = 0;
		do {
			r = a_str.find('.', l);
			if (r > i) r = i;
			proto.push_back(a_str.substr(l, r - l));
			l = r+1;
		} while (r < i);
		i += PROTO_DELIM.size();
	} else {
		i = 0;
	}
	size_t j = a_str.find('/', i);
	if (j != std::string::npos) {
		host = a_str.substr(i, j - i);
		path = a_str.substr(j);
	} else {
		host = a_str.substr(i);
		path = "";
	}
}

//------------------------------------------------------------------------------	
//
std::string Url::to_string()
{
	std::stringstream ss;

	if (!proto.empty()) {
		for (size_t i = 0; i < proto.size(); i++) {
			if (i > 0) {
				ss << '.';
			}
			ss << proto[i];
		}
		ss << PROTO_DELIM;
	}
	ss << host << path;

	return ss.str();
}


//------------------------------------------------------------------------------
	} // end namespace trans
} // end namespace remo
//------------------------------------------------------------------------------
