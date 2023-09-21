// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "HelloSwift",
    targets: [
        .target(name: "HelloCxx"),
        .executableTarget(
            name: "hello",
            dependencies: ["HelloCxx"],
            swiftSettings: [.interoperabilityMode(.Cxx)]
        ),
    ]
)
