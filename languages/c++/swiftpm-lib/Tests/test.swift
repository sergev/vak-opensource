import XCTest
@testable import MyCppLib

final class MyCppLibTests: XCTestCase {

    func testSmallNumbers() {
        XCTAssertEqual(fibonacci(0), 0)
        XCTAssertEqual(fibonacci(1), 1)
        XCTAssertEqual(fibonacci(2), 1)
        XCTAssertEqual(fibonacci(3), 2)
        XCTAssertEqual(fibonacci(4), 3)
        XCTAssertEqual(fibonacci(5), 5)
    }

    func testLargerNumbers() {
        XCTAssertEqual(fibonacci(10), 55)
        XCTAssertEqual(fibonacci(15), 610)
    }

    func testNegativeInput() {
        // Catch invalid input in C++ as -1.
        XCTAssertEqual(fibonacci(-1), -1)
    }
}
