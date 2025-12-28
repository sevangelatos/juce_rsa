#!/usr/bin/env python3
import sys
import os
import platform
from io import open
import setuptools

VERSION_STRING='0.2.0'

EXT_SOURCES = [
    "JUCE/modules/juce_core/juce_core.cpp",
    "JUCE/modules/juce_cryptography/juce_cryptography.cpp",
]

INT_SOURCES = [
    "src/juce_rsa.cpp"
]

# Platform-specific configuration
if sys.platform == "win32":
    # Windows
    cflags = [
        "/std:c++17",
        "/O2",
        "/DNDEBUG",
        "/DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED",
        "/DJUCE_STANDALONE_APPLICATION",
        "/DJUCE_USE_CURL=0"
    ]
    libs = [
        "Shell32",
        "Ole32"
    ]
    lflags = []
elif sys.platform == "darwin":
    # macOS
    cflags = [
        "-std=c++17",
        "-O3",
        "-DNDEBUG",
        "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED",
        "-DJUCE_STANDALONE_APPLICATION",
        "-DJUCE_USE_CURL=0",
        "-fPIC"
    ]
    libs = []
    lflags = [
        "-framework", "Cocoa",
        "-framework", "IOKit",
        "-framework", "Security"
    ]

    # Add architecture-specific flags for Apple Silicon
    if platform.machine() == "arm64":
        cflags.extend(["-arch", "arm64"])
        lflags.extend(["-arch", "arm64"])
    else:
        # Intel Mac
        cflags.extend(["-arch", "x86_64"])
        lflags.extend(["-arch", "x86_64"])
else:
    # Linux and other Unix-like systems
    cflags = [
        "-std=c++17",
        "-O3",
        "-DNDEBUG",
        "-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED",
        "-DJUCE_STANDALONE_APPLICATION",
        "-DJUCE_USE_CURL=0",
        "-fPIC"  # Position Independent Code for shared libraries
    ]
    libs = [
    ]
    lflags = ["-s"]

# Obtain JUCE if it is not already here
if not os.path.isfile(EXT_SOURCES[0]):
    print("JUCE not found, downloading...")
    os.system("git clone --depth 1 --branch 8.0.12 https://github.com/juce-framework/JUCE.git")
    print("JUCE downloaded successfully")

extensions = [setuptools.Extension(
    "juce_rsa",
    sources=EXT_SOURCES + INT_SOURCES,
    extra_compile_args=cflags,
    extra_link_args=lflags,
    include_dirs=[
        "JUCE/modules",
        "JUCE/modules/juce_core",
        "JUCE/modules/juce_cryptography"
    ],

    libraries=libs,
    language="c++",
    define_macros=[('VERSION_INFO', VERSION_STRING)]  # Version info for the extension
)]


with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setuptools.setup(
    author="sevangelatos",
    author_email="sevangelatos_at_@gmail.com",
    name="juce_rsa",
    version=VERSION_STRING,
    url="https://github.com/sevangelatos/juce_rsa",
    ext_modules=extensions,
    description="Python bindings for juce::RSAKey",
    long_description=long_description,
    long_description_content_type="text/markdown",
    license="MIT",
    license_files=["LICENSE"],
    classifiers=[
                "Development Status :: 4 - Beta",
                "Intended Audience :: Developers",
                "Environment :: Win32 (MS Windows)",
                "Environment :: MacOS X",
                "Operating System :: POSIX :: Linux",
                "Operating System :: Microsoft :: Windows",
                "Operating System :: MacOS :: MacOS X",
                "Programming Language :: C++",
                "Programming Language :: Python :: 3 :: Only",
                "Topic :: Security :: Cryptography",
                "Topic :: Security",
                "Topic :: Utilities"
    ],
    python_requires='>=3.6'
)
