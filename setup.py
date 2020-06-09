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
	cflags = [
		"/std:c++latest",
		"/O2",
		"/DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED",
		"/DJUCE_STANDALONE_APPLICATION"
	]
	libs = [
		"Shell32",
		"Ole32"
	]
else:
	cflags = [
		"-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED",
		"-DJUCE_STANDALONE_APPLICATION",
		"-DJUCE_USE_CURL=0",
	 	"-O3",
		"-msse4.2"
	]
	libs = [
		"curl"
	]

extensions = [setuptools.Extension(
	"juce_rsa",
	sources=EXT_SOURCES + INT_SOURCES,
	extra_compile_args=cflags,
	include_dirs=["JUCE/modules"],
	libraries=libs,
	language="c++"
)]


with open("README.md", "r", encoding="utf-8") as fh:
	long_description = fh.read()

setuptools.setup(
	author="sevangelatos",
	name="juce_rsa",
	version="0.1",
	url="https://github.com/sevangelatos/juce_rsa",
	ext_modules=extensions,
	description="Python bindings for juce::RSAKey",
	long_description=long_description,
	long_description_content_type="text/markdown",
	classifiers=[
		"Development Status :: 4 - Beta",
		"Intended Audience :: Developers",
		"Environment :: Win32 (MS Windows)",
		"Environment :: MacOS X",
		"Operating System :: POSIX :: Linux",
		"Operating System :: Microsoft :: Windows",
		"Operating System :: MacOS :: MacOS X",
		"License :: OSI Approved :: MIT License",
		"Programming Language :: C++",
		"Programming Language :: Python :: 3 :: Only",
		"Topic :: Security :: Cryptography",
		"Topic :: Security",
		"Topic :: Utilities"
	],
    python_requires='>=3.6'
)
