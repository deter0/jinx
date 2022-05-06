# JINXST

**Jinx** **St**yle Sheet


## Spec

File Extension: `.jinxst`

Dashes in names are **not** allowed.
```css
/* This is a comment */
global_light { /* This is a namespace or group */
    $background_color: color = #FFFFFFFF; /* $VARIABLE_NAME: VARIABLE_TYPE = VARIABLE_VALUE */
}

global_dark | global_light { /* This is a namespace that inherits from global-light */
    $background_color != #000000FF;
    /* Note prefix `!` indicates overriding the value from the name space it is inheriting from. The variable will also inherit it's type. */
}
```

### Color

Possible formats are:
* #RRGGBBAA
* #RRGGBB
* rgb(R, G, B)
* rgba(R, G, B, A)

Where R, G, B, A are floating point numbers (0.0 - 1.0)

Keywords such as `green`, `red`, `white`, etc. are currently unsupported.
