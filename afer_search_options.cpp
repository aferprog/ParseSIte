#include "afer_search_options.h"
#include <algorithm>


afer_search_options::afer_search_options(const AferGumboNode& node) {
    additional_options.assign(COUNT_OF_AFER_FLAGS, 0);
    struct afer_search_options &res = *this;
    res.type = node.getType();
    if (res.type == GumboNodeType::GUMBO_NODE_ELEMENT) {
        res.tag = node.getTag();
        std::map<std::string, std::string> all_attributes = node.getAttributes();

        for (const auto& el : all_attributes) {
            if (!el.second.empty() && el.second[0] == '%')
                res.to_get[el.first] = el.second.substr(1);
            else if (el.first != "afer")
                res.attributes[el.first] = el.second;
            else {
                std::stringstream afer(&el.second[0]);
                while (!afer.eof()) {
                    std::string t;
                    afer >> t;
                    if (t == "start_entity" || t == "entity_start") res.additional_options[AferProperties::ENTITY_START] = true;
                    else if (t == "float_deep" || t == "deep_float") res.additional_options[AferProperties::FLOAT_DEEP] = true;
                }
            }
        }
    }
    else if (res.type == GumboNodeType::GUMBO_NODE_TEXT) {
        auto str = node.getText();
        if (str[0] == '%')
            res.to_get["text"] = str.substr(1);
        else
            res.attributes["text"] = str;
    }
}

bool afer_search_options::check(const AferGumboNode& node) const
{
    switch (type)
    {
    case GumboNodeType::GUMBO_NODE_TEXT:
        if (node.getType() != GUMBO_NODE_TEXT)
            return false;
        
        return !to_get.empty()||(!attributes.empty() && attributes.at("text") == node.getText());
        
    case GumboNodeType::GUMBO_NODE_ELEMENT: {
        bool check = (node.getTag() == tag && node.getType() == type);
        if (!check) return false;

        for (const auto& pair : attributes) {
            if (!node.hasAttribute(pair.first) || node.getAttribute(pair.first.data()) != pair.second)
                return false;
        }
        for (const auto& pair : to_get) {
            if (!node.hasAttribute(pair.first))
                return false;
        }

        return true;

    }
    default:
        return false;
    }

}

