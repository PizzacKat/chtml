# CHTML
Removed the Markup Language from HTML.  
It's like HTML, but with CSS syntax.  
Raw text is written in quotation marks.  

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

### Example index file (the compiler automatically adds \<!DOCTYPE html\> onto the compiled file):
```css
html {
  head {
    title { "CHTML" }
  }
  body {
    "I'm made with CHTML!" br
    div {
      "Here's a picture of a cat: " img[src="cat.png"]
    }
  }
}
```

## Comments
Comments are written by typing an exclamation before a block.  

### Example:
```
!{I'm a comment}
```

## Raw text
Raw text is typed between quotation marks.

### Example:
```css
p {
  "Hello, world!"
}
```

## Multiline text
Multiline text is typed between 3 quotation marks.

### Example:
```css
p {
  """
  I am
  Multiple
  Lines long
  """
}
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

### Example:
```css
!{Apply a certain style to each div with a class of "styled"}
#div[class+="styled" | style="..."]
div[class="styled"]
div[class="styled container"]

!{Give every span with a class of "with-image" an image}
#div[class+="with-image"] {
  img[src="..."]
}
div[class="with-image"]
```
