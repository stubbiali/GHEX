# -*- coding: utf-8 -*-
import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import shutil
import subprocess

# build dependencies
import mpi4py
import pybind11


class CMakeExtension(Extension):
    def __init__(self, name):
        Extension.__init__(self, name, sources=[])


class CMakeBuild(build_ext):
    def run(self):
        out = subprocess.run(["cmake", "--version"], capture_output=True)
        if out.returncode != 0:
            raise RuntimeError(out.stderr)

        this_dir = os.path.dirname(__file__)
        source_dir = os.path.abspath(os.path.join(this_dir, "../.."))
        build_dir = os.path.abspath(os.path.join(this_dir, "_build"))
        pybind11_dir = pybind11.get_cmake_dir()
        mpi4py_dir = os.path.abspath(os.path.join(mpi4py.get_include(), ".."))

        ghex_use_gpu = os.environ.get("GHEX_USE_GPU", "False")
        ghex_gpu_type = os.environ.get("GHEX_GPU_TYPE", "AUTO")
        ghex_gpu_arch = os.environ.get("GHEX_GPU_ARCH", "")

        cmake_args = [
            f"-B{build_dir}",
            "-DCMAKE_BUILD_TYPE=Release",
            "-DGHEX_BUILD_PYTHON_BINDINGS=On",
            "-DHAVE_MPI4PY=True",
            f"-Dpybind11_DIR={pybind11_dir}",
            f"-DPY_MPI4PY={mpi4py_dir}",
            f"-DUSE_GPU={ghex_use_gpu}",
            f"-DGHEX_GPU_TYPE={ghex_gpu_type}",
            f"-DGHEX_GPU_ARCH={ghex_gpu_arch}",
        ]
        subprocess.run(["cmake", source_dir, *cmake_args], capture_output=False)
        subprocess.run(["cmake", "--build", build_dir, "--", "-j 8"], capture_output=False)

        ext_name = self.extensions[0].name
        src_path = os.path.join(build_dir, self.get_ext_filename(ext_name))
        trg_path = self.get_ext_fullpath(ext_name)
        self.copy_file(src_path, trg_path)

        # shutil.rmtree(build_dir)


if __name__ == "__main__":
    setup(ext_modules=[CMakeExtension("ghex_py_bindings")], cmdclass={"build_ext": CMakeBuild})
