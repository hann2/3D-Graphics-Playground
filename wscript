


from waflib.TaskGen import feature, before_method, after_method

@feature('cxx')
@after_method('process_source')
@before_method('apply_incpaths')
def add_includes_paths(self):
        incs = set(self.to_list(getattr(self, 'includes', '')))
        for x in self.compiled_tasks:
                incs.add(x.inputs[0].parent.path_from(self.path))
        self.includes = list(incs)

def options(ctx):
    ctx.load('compiler_cxx qt4')

def configure(ctx):
    ctx.load('compiler_cxx qt4')
    ctx.env.CXXFLAGS = ['-c', '-O0', '-ggdb', '-Wall', '-std=c++11']
    ctx.env.LINKFLAGS = []

def build(ctx):
    src_files = [
        'src/main.cpp',
        'src/glframe.cpp',
        'src/glrenderthread.cpp',
        'src/mainwindow.cpp',
        'src/mainwindow.ui',
        'src/Geometry.cpp',
        'src/load_collada.cpp',
        'src/ProceduralGenerator.cpp',
        'src/Model.cpp',
        'src/Skeleton.cpp',
        'src/Turtle.cpp',
        'src/TreeGenerator.cpp'
    ]
    ctx.program(
        source = ' '.join(src_files),
        target = 'demo',
        features = 'qt4 cxx cxxprogram',
        uselib = 'QTCORE QTGUI QTOPENGL QTSVG QWIDGET QTSQL QTUITOOLS QTSCRIPT',
        includes = ['lib'],
        lib = ['GL', 'GLU', 'GLEW', 'X11']
    )

