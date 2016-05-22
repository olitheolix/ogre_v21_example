# MIT License
#
# Copyright (c) 2016 Oliver Nagy
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import sys
import shutil
import subprocess

from Cython.Build import cythonize
from distutils.core import setup, Extension


def main():
    """
    Compile the Cython wrapper for Ogre.
    """
    # Note: the name of the library *MUST* match the name of the pyx file. You
    # are also *NOT* allowed to rename the library file name once it was built!
    # If you do, Python will throw an error when importing the module.
    libname = 'azOgre'

    # Backup the current working directory because we will change it.
    cwd = os.getcwd()
    src_dir = os.path.join(cwd, 'src')

    dir_conda = os.getenv('CONDA_ENV_PATH')
    assert dir_conda != ''

    # Let setuptools' Extension function take care of the compiler options.
    ext = Extension(
        name=libname,
        sources=[
            src_dir + '/' + libname + '.pyx'
        ],
        include_dirs=[
            dir_conda + '/include',
            dir_conda + '/include/OGRE',
            dir_conda + '/include/OGRE/Hlms',
            dir_conda + '/include/OGRE/Hlms/Pbs',
            dir_conda + '/include/OGRE/Hlms/Unlit',
            dir_conda + '/include/OGRE/Overlay',
        ],
        library_dirs=[
            dir_conda + '/lib',
        ],
        runtime_library_dirs=[
            dir_conda + '/lib',
        ],
        libraries=[
            'OgreHlmsPbs',
            'OgreHlmsUnlit',
            'OgreMain',
            'OgreOverlay',
            'boost_atomic',
            'boost_chrono',
            'boost_date_time',
            'boost_system',
            'boost_thread',
            'pthread',
        ],
        extra_compile_args=[
            '-Wall', '-Wextra', '-std=c++11',
            '-Wno-unused-parameter', '-Wno-unused-function',
            '-Wno-unused-variable', '-fPIC',
        ],
        language='c++',
    )

    # Cython options.
    cython_opts = {
        'include_path': [src_dir]
    }

    # Build the extension module.
    setup(
        name=libname,
        version='0.1',
        description='Python Wrapper for Ogre3D (v2.1)',
        author='Oliver Nagy',
        author_email='olitheolix@gmail.com',
        url='https://github.com/olitheolix/azOgre',
        ext_modules=cythonize(ext, **cython_opts),
    )

    # Remove the library and build directory if --clean was specified.
    if 'clean' in sys.argv:
        # Remove library.
        tmp = 'rm -f {}.*.so'.format(libname)
        print(tmp)
        subprocess.call(tmp, shell=True)

        # Remove Cython file.
        tmp = 'rm -f {}/{}.cpp'.format(src_dir, libname)
        print(tmp)
        subprocess.call(tmp, shell=True)


if __name__ == '__main__':
    main()
