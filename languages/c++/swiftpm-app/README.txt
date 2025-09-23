SwiftPM (short for Swift Package Manager) is Swift’s official
build system and dependency manager, included with the Swift
toolchain. It plays the same role as Cargo for Rust, npm for
JavaScript, or pip + setuptools for Python.

SwiftPM is designed around Swift, but it also has first-class
support for C and C++ only projects. The trick is that you
still use a Package.swift manifest, but your sources can be
entirely C or C++. SwiftPM will build them into modules
(libraries or executables) that other SwiftPM packages can
depend on.

This example shows an executable in C++:

    swiftpm-app/
    ├── Package.swift
    └── Sources/
        └── Hello/
            └── main.cpp

Build and run:

    swift build
    swift run
