// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "Demo",
    platforms: [
        .macOS(.v14),
    ],
    targets: [
        .target(
            name: "Gui",
            swiftSettings: [.interoperabilityMode(.Cxx)]
        ),
        .target(
            name: "HelloCxx",
            dependencies: ["Gui"]
        ),
        .executableTarget(
            name: "Demo",
            dependencies: ["HelloCxx", "Gui"],
            swiftSettings: [.interoperabilityMode(.Cxx)]
        ),
    ],
    cxxLanguageStandard: .cxx20
)
