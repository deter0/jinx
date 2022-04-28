project('jinx', 'c')

cairo_dep = dependency('cairo')
xlib_dep = dependency('x11')
Xext_dep = dependency('xext')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

executable(
    'jinx',
    'src/main.c',
    dependencies : [Xext_dep, cairo_dep, xlib_dep, m_dep]
)
