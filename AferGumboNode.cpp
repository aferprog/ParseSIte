#include "AferGumboNode.h"
#include <stdexcept>
//#include <algorithm>

AferGumboNode::AferGumboNode(GumboNode* node, bool without_additions, bool simple_mode)
{
	this->node = node;
	this->without_additions = without_additions;
	this->simple_mode = simple_mode;

	tag = getTag();
	type = getType();
}

 AferGumboNode::AferGumboNode(const AferGumboNode& node){
	 this->node = node.node;
	 this->without_additions = node.without_additions;
	 this->simple_mode = node.simple_mode;

	 tag = getTag();
	 type = getType();
 }

 AferGumboNode::~AferGumboNode()
 {
	 if (attributes != nullptr) delete attributes;
 }

GumboNodeType AferGumboNode::getType() const
{
	return node->type;
}

GumboTag AferGumboNode::getTag() const
{
	return node->v.element.tag;
}

std::map<std::string, std::string> AferGumboNode::getAttributes() const
{
	loadAttributes();
	return *attributes;
}

bool AferGumboNode::hasAttribute(const std::string& attr) const
{
	loadAttributes();
	return attributes->contains(attr);
}

std::string AferGumboNode::getAttribute(const std::string& attr) const
{
	if (hasAttribute(attr)) return (*attributes)[attr];
	else throw std::runtime_error("No such attribute '"+std::string(attr) + "'");
}


 AferGumboNode AferGumboNode::getParent() const
{
	return AferGumboNode(node->parent);
}

size_t AferGumboNode::getIndex_within_parent() const
{
	return node->index_within_parent;
}

GumboParseFlags AferGumboNode::getParse_flags() const
{
	return node->parse_flags;
}

std::string AferGumboNode::getText() const
{
	if (node->type!=GUMBO_NODE_TEXT) 
		throw std::runtime_error("Type of node isn't text. It's imposible to take text from it.");

	auto str = std::string_view(node->v.text.text);
	str.remove_prefix(str.find_first_not_of(" \n\t\r"));
	str.remove_suffix(str.length() - str.find_last_not_of(" \n\t\r") - 1);
	return std::string(str);
}

std::vector< AferGumboNode> AferGumboNode::getChildren() const
{
	if (node->type == GUMBO_NODE_TEXT)
		return std::vector<AferGumboNode>();
		// throw std::runtime_error("Text-node can't have children.");
	GumboVector* children = &node->v.element.children;
	std::vector<AferGumboNode> res;
	for (unsigned int i = 0; i < children->length; ++i) {
		AferGumboNode child = ((GumboNode*)children->data[i]);
		if (child.getType() == GumboNodeType::GUMBO_NODE_ELEMENT || child.getType() == GumboNodeType::GUMBO_NODE_TEXT) {
			if (!(without_additions && this->getTag()==GumboTag::GUMBO_TAG_HTML && !child.hasChildren()))
				res.push_back(child);
		}
		else if (!simple_mode)
			res.push_back(child);
	}
	return res;
}

bool AferGumboNode::hasChildren() const
{
	if (node->type == GUMBO_NODE_TEXT)
		return false;

	return getChildren().size()!=0;
}

AferGumboNode& AferGumboNode::operator=(const AferGumboNode& node)
{
	this->node = node.node;
	this->without_additions = node.without_additions;
	this->simple_mode = node.simple_mode;

	tag = getTag();
	type = getType();
	delete attributes;
	attributes = nullptr;
	return *this;
}

bool AferGumboNode::checkAttributes(const std::map<std::string, std::string>& attr) const
{
	for (const auto& el : attr) {
		if (hasAttribute(el.first) && getAttribute(el.first) != el.second) return false;
	}
	return true;
}

void AferGumboNode::loadAttributes() const
{
	if (attributes != nullptr) return;
	attributes = new std::map<std::string, std::string>;
	if (node->type == GUMBO_NODE_TEXT) return;
	const GumboVector* attr = &node->v.element.attributes;
	auto& res = *attributes;
	for (unsigned int i = 0; i < attr->length; ++i) {
		GumboAttribute* temp = (GumboAttribute*)attr->data[i];
		res[std::string(temp->name)] = std::string(temp->value);
	}


}

std::ostream& operator<< (std::ostream &out, const AferGumboNode &node)
{
	if (node.node->v.element.tag < GumboTag::GUMBO_TAG_LAST)
		out << gumbo_normalized_tagname(node.node->v.element.tag) << ' ';
	else out << node.node->v.element.tag<<' ';
	for (auto& pair : node.getAttributes())
		out << pair.first << "='" << pair.second << "' ";
	return out;
}

