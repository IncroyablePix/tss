# TSS
TSS (Trema Style Sheet) is a styling language specific primarly invented for Trema, but that can be generically used for any project requiring a styling language.
Its syntax strongly resembles to CSS with features such as scoping for shared variables.

## Declaring a variable
You can declare a global property for the stylesheet. This variable will be applied for the whole window.

### Syntax
```css
  red: 0xCC0000FF;
```

We can note several things:
- The assigning operator is a colon
- Lines end with a semicolon

Also, variable names must begin with a letter and can contain numbers, letters, hyphens or underscores.

### Variable types
There are three variable types :
- **bool**: true or false
- **number**: 64bits signed integers or floats
- **string**: whatever text delimited by a couple of single or double quotes

```css
  myText: "Some random text";
  myOtherText: 'Some text with single quotes';

  myFloatNumber: -.5;
  myIntegerNumber: 256;
  mySecondFloat: 0.50;

  myBool: true;
```

### Copying
You can also copy a variable that has already been assigned a value.

```css
  invisible: 0x0;
  text-color: invisible;
```

## Scopes
Variables defined outside a scope will be affected to the window in its entirety.
To apply a variable to a specific component, you will need to define them inside a scope.

```css
  #element {
    text-color: 0xCC0000FF;
  }
```

If you want to explicitly set properties for the whole window, you may just use the global selector "#"

```css
  # {
    text-color: 0xCC0000FF;
  }
```

This code will apply a reddish text colour for the element with the "element" id.
You can also nest scopes to copy their variables.

```css
  scope {
    red: 0xCC0000FF;

    #element {
      text-color: red;
    }
  }
```

## C++ Interface
For such a code:

```css 
#element {
  baseWidth: 150;
}
```

Given the C++ code:

```c++
StyleParser parser;
parser.ParseFromCode(code);
const auto variables = parser.GetVariables().at("#element");
const auto value = symbolTable->GetVariable("baseWidth")->GetValue();
```

The variable `value` will be a std::variant containing either
* double
* int64_t
* bool
* std::string
