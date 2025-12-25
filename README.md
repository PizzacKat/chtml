# chtml
Removed the Markup Language from HTML

It's like HTML, but with CSS syntax

## Normal HTML:
```html
<tag attribute...>
  child...
</tag>
```

## CHTML:
```css
tag[attribute...] {
  child...
}
```
### Where:
`attribute`: `key="value"` | `key`

### Use:
An element may not have any attributes
```css
tag[] {
  child...
}
tag {
  child...
}
```
Or children
```css
tag[attribute...] {}
tag[attribute...]
```
Or neither
```css
tag[] {}
tag
```

## Now with extensions!
Extensions are a set of conditions, attributes and children. If any element after an extension matches the conditions, it gets added the attributes and children of the extension.
Conditions act against elements.

### Example:
```css
#tag[condition... | attribute...] {
  child...
}
```

### Where:
`condition`: `key=="value"` | `key!="value"` | `key+="value"` | `key-="value"`  
`==`: equals  
`!=`: not equals  
`+=`: contains  
`-=`: doesn't contain  

### Use:
An extension may not have any added attributes
```css
#tag[condition...] {
  child...
}
```
Or children
```css
#tag[condition... | attribute...]
```
Or neither (even thought it's useless)
```css
#tag[condition...]
```

An extension may also not have any conditions (even though it's not the intended usage)
```css
#tag[| attribute...] {
  child...
}
```
