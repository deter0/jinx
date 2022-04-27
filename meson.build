project('jinx', 'c')

cairo_dep = dependency('cairo')
xlib_dep = dependency('x11')

executable(
    'jinx',
    'src/main.c',
    dependencies : [cairo_dep, xlib_dep]
)
