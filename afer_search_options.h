#pragma once
#include <gumbo.h>
#include "AferGumboNode.h"
#include <map>
#include <string>
#include <sstream>
#include <vector>

#define COUNT_OF_AFER_FLAGS 3
enum AferProperties{
	ENTITY_START,
	ENTITY_COMPLATED,
	FLOAT_DEEP
};

struct afer_search_options {
	GumboTag tag; // Which tag we are lookeing for
	GumboNodeType type; // Which type we are looking for (text or element)
	std::map<std::string, std::string> attributes; // which attributes this node have to have
	std::map<std::string, std::string> to_get; // attributes which we need to get
	std::vector<char> additional_options;
	afer_search_options(const AferGumboNode& node);
	bool check(const AferGumboNode& node) const;
};
