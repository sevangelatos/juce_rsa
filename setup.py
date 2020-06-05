import sys
from io import open
import setuptools

EXT_SOURCES = [
	"JUCE/modules/juce_core/juce_core.cpp",
	"JUCE/modules/juce_cryptography/juce_cryptography.cpp",
]

INT_SOURCES = [
	"src/juce_rsa.cpp"
]

if sys.platform == "win32":
	cflags = ["/O2"]
else:
	cflags = [
		"-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED",
		"-DJUCE_STANDALONE_APPLICATION",
	 	"-O3",
		 "-msse4.2"]

extensions = [setuptools.Extension(
	"juce_rsa",
	sources=EXT_SOURCES + INT_SOURCES,
	extra_compile_args=cflags,
	include_dirs=["JUCE/modules"],
	language="c++"
)]


with open("README.md", "r", encoding="utf-8") as fh:
	long_description = fh.read()

setuptools.setup(
	author="sevangelatos",
	name="juce_rsa",
	version="0.1",
	url="https://github.com/accusonus/juce_rsa",
	ext_modules=extensions,
	description="Python bindings for juce::RSA",
	long_description=long_description,
	long_description_content_type="text/markdown",
	classifiers=[
		"Development Status :: 4 - Beta",
		"Intended Audience :: Developers",
		"Intended Audience :: Science/Research",
		"License :: OSI Approved :: ISC License (ISCL)",
		"Operating System :: OS Independent",
		"Programming Language :: C++",
		"Topic :: Internet",
		"Topic :: Scientific/Engineering",
		"Topic :: Utilities"
	],
    python_requires='>=3.6'
)
