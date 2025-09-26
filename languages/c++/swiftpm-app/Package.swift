// swift-tools-version: 6.2
import PackageDescription

let package = Package(
    name: "Hello",
    products: [
        .executable(name: "hello", targets: ["Hello"]),
    ],
    targets: [
        .executableTarget(
            name: "Hello",
            path: "Sources",
            cxxSettings: [
                .unsafeFlags(["-std=c++20"])
            ]
        ),
    ]
)
