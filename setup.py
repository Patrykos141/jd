from setuptools import setup, Extension
import pybind11
import sys
import sysconfig

# Flagi dla turbo kompilacji
if sys.platform == "win32":
    compile_args = ["/O2", "/openmp", "/arch:AVX2"]
    link_args = []
else:
    compile_args = [
        "-O3", "-march=native", "-ffast-math", "-funroll-loops",
        "-fopenmp", "-mtune=native", "-fstrict-aliasing"
    ]
    link_args = ["-fopenmp"]

ext_modules = [
    Extension(
        "classify",  # nazwa modułu
        ["classify_texts.cpp"],  # źródło C++
        include_dirs=[
            pybind11.get_include(),
            sysconfig.get_path("include")
        ],
        language="c++",
        extra_compile_args=compile_args,
        extra_link_args=link_args
    )
]

setup(
    name="classify",
    version="1.0",
    author="Hardmode",
    description="Ultra szybka klasyfikacja warunkowa na int32 z OpenMP",
    ext_modules=ext_modules,
    zip_safe=False,
)
