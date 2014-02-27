


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

        'src/ui/glframe.cpp',
        'src/ui/glrenderthread.cpp',
        'src/ui/mainwindow.cpp',
        'src/ui/mainwindow.ui',

        'src/model/IndexedFaceSet.cpp',
        'src/model/load_collada.cpp',
        'src/model/Model.cpp',

        'src/procedural/ProceduralGenerator.cpp',
        'src/procedural/Turtle.cpp',
        'src/procedural/TreeGenerator.cpp',
        'src/procedural/load_cylinder.cpp',

        'src/scene/Scene.cpp',
        'src/scene/WireframeScene.cpp',
        'src/scene/ProceduralScene.cpp',
        'src/scene/TreeScene.cpp',
        'src/scene/PerlinScene.cpp',
        'src/scene/EfficientScene.cpp'
    ]
    ctx.program(
        source = ' '.join(src_files),
        target = 'demo',
        features = 'qt4 cxx cxxprogram',
        uselib = 'QTCORE QTGUI QTOPENGL QTSVG QWIDGET QTSQL QTUITOOLS QTSCRIPT',
        includes = ['lib'],
        lib = ['GL', 'GLU', 'GLEW', 'X11']
    )

