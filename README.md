# ParseSite
This library allows to extract enities from **html** page using spetial html-based markup language.

## General idea
Entity is a unit of properties and their values. Every property has a name. All values are consisted in html-page. An html-page can coonsist couple of entities.
_________
## Markup language for discribing entities
### Example "Books in amazon"
URL: https://www.amazon.com/Marissa-Mayer-Fight-Save-Yahoo/dp/B00R3DEUBW/ref=sr_1_1?keywords=yahoo&qid=1665437799&qu=eyJxc2MiOiI0LjU0IiwicXNhIjoiNC4yNCIsInFzcCI6IjMuOTcifQ%3D%3D&s=books&sr=1-1

### Afer-tag
Main idea is to discrabe way to all propeties for all entities in string. Tag which is root for entities is marked with attribute `afer="entity_start"`. The `afer=` attribute is special attribute for extra information about current tag for algorithm.
For example 
```html 
<div class="offer-book" afer="entity_start float_deep">
```
