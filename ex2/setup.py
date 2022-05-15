from setuptools import setup, find_packages, Extension

setup(
    name='mykmeanssp',
    version='0.1.0',
    author='Gaya Talyosef',
    author_email='gayatalyosef@mail.tau.ac.il',
    description='Kmeans project hw 2',
    install_requires=['invoke'],
    packages=find_packages(),
    license='GPL-2',
    ext_modules=[
        Extension(
            'mykmeanssp',
            ['kmeans.c']
        )
    ]
)