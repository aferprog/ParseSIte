#include "AferFormat.h"
#include <algorithm>
#include <iostream>
#include "prettyPrint.h"

void AferFormat::find_all_by_recursion_help(std::vector<AferGumboNode>& res, const afer_search_options& format, const AferGumboNode& node, int deep) {

    if (format.check(node)) 
        res.push_back(node);
    else {
        for (auto& el : node.getChildren())
            find_all_by_recursion_help(res, format, el, deep+1);
    }
}

std::vector<AferGumboNode> AferFormat::find_all_by_recursion(afer_search_options format, AferGumboNode& root)
{
    std::vector<AferGumboNode> res;
    find_all_by_recursion_help(res, format, root);
    return res;
}

void AferFormat::build_entity(AferEntity& entity, std::vector<AferGumboNode> whatFind, const AferGumboNode& node)
{
    bool is_node_found = false;
    for (auto& format : whatFind) {
        afer_search_options opt(format);
        if (opt.check(node)) {
            is_node_found = true;
            switch (opt.type)
            {
            case GumboNodeType::GUMBO_NODE_TEXT:
                if (opt.type == GumboNodeType::GUMBO_NODE_TEXT) {
                    if (opt.to_get.size() > 0) {
                        std::string key(opt.to_get["text"]);
                        entity[key] = node.getText();
                    }
                }
                break;
            case GumboNodeType::GUMBO_NODE_ELEMENT:
                for (auto& el : opt.to_get) {
                    entity[el.second] = node.getAttribute(el.first);
                }
                for (auto& child : node.getChildren()) {
                    auto children = format.getChildren();
                    build_entity(entity, children, child);
                }
                break;
            default:
                throw std::runtime_error("MISTIKA. YA NE ZNAU, CHTO PROIZOSHLO");
            }

            break;
        }
    }
    if (!is_node_found && !whatFind.empty()) {
        for (auto& child : node.getChildren())
            build_entity(entity, whatFind, child);
    }
}

GumboOutput* AferFormat::open_source(std::string_view html_doc)
{
    
    /*GumboOptions options = kGumboDefaultOptions;
    GumboOutput* output = gumbo_parse_with_options(&options, html_doc.data(), html_doc.length());
    return output;*/
    return gumbo_parse_with_options(
        &kGumboDefaultOptions, html_doc.data(), html_doc.length());
}
void AferFormat::close_source(GumboOutput* output)
{
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

bool AferFormat::check_end_of_path(const AferGumboNode& format)
{
    afer_search_options opt(format);
    if (opt.additional_options[AferProperties::ENTITY_START] == 1)
        return true;

    auto children = format.getChildren();
    if (children.size() != 1) {
        std::cout << format;
        throw std::runtime_error("Wrong path to enities: every node in path must have only one child.");
    }
    return false;

}

int AferFormat::count_of_parameters(const AferGumboNode& node)
{
    int res = afer_search_options(node).to_get.size();
    for (auto& child : node.getChildren())
        res += count_of_parameters(child);
    return res;
}


std::vector<AferGumboNode> AferFormat::find_entities_roots(AferGumboNode& format, const AferGumboNode& root)
{
    std::vector<AferGumboNode> vect0 = { root };
    std::vector<AferGumboNode> vect1;


    auto* temp_vect = &vect0;
    auto* next_vect = &vect1;

    while (!check_end_of_path(format)) {
        
        for (auto& node : *temp_vect) {
            auto temp_roots = find_all_by_recursion(format, node);
            next_vect->insert(next_vect->end(), 
                std::make_move_iterator( temp_roots.begin()), 
                std::make_move_iterator(temp_roots.end())
            );
        }

        std::swap(temp_vect, next_vect);
        next_vect->clear();
        format = format.getChildren()[0];
    }
    for (auto& node : *temp_vect) {
        auto temp_roots = find_all_by_recursion(format, node);
        next_vect->insert(next_vect->end(),
            std::make_move_iterator(temp_roots.begin()),
            std::make_move_iterator(temp_roots.end())
        );
    }
    return *next_vect;
}

std::vector<AferEntity> AferFormat::get_formated_enteties(const GumboOutput* format_output, const GumboOutput* root_output)
{
    //std::cout<<prettyprint(format_output->root, 0, "  ");
    //std::cout<<prettyprint(root_output->root, 0, "  ");

    AferGumboNode format_root(format_output->root, true);
    AferGumboNode root(root_output->root);

    std::vector<AferGumboNode> entities = find_entities_roots(format_root, root);
    int parameters_count = count_of_parameters(format_root);
    //std::cout << parameters_count << "\n";
    temp_entities.resize(entities.size());
    for (int i = 0; i < entities.size(); ++i)
        build_entity(temp_entities[i], { format_root }, entities[i]);
    
    /*auto border = std::remove_if(temp_entities.begin(), temp_entities.end(), [parameters_count](const auto& entity) {
        return entity.size() != parameters_count;
        });
    temp_entities.erase(border, temp_entities.end());
    */
    return temp_entities;
}

std::vector<AferEntity> AferFormat::get_formated_enteties(std::string_view format, std::string_view html_page)
{
    GumboOutput* format_output = open_source(format);
    GumboOutput* page_output = open_source(html_page);

    get_formated_enteties(format_output, page_output);

    close_source(page_output);
    close_source(format_output);
    return temp_entities;
}
