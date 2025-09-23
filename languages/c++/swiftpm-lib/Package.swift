// swift-tools-version: 6.2
import PackageDescription

let package = Package(
    name: "MyCppLib",
    products: [
        .library(name: "MyCppLib", targets: ["MyCppLib"]),
    ],
    targets: [
        .target(
            name: "MyCppLib",
            path: "Sources/MyCppLib",
            publicHeadersPath: "include",
            cxxSettings: [
                .define("USE_FEATURE_X", to: "1"),
                .unsafeFlags(["-std=c++20"])
            ]
        ),
        .testTarget(
            name: "MyCppLibTests",
            dependencies: ["MyCppLib"],
            path: "Tests"
        ),
    ]
)
