//
//  ModelState.swift
//
//  Created by Serge Vakulenko on 11/11/23.
//
import Foundation
import HelloCxx

class ModelState: ObservableObject {
    @Published var count: Int = 123

    static let shared = ModelState()

    // Allocate C++ object.
    var state = HelloCxx()

    //init() {
    //    setCallBack { stringPtr in
    //        let newString = CFStringCreateWithCString(kCFAllocatorDefault, stringPtr, kCFStringEncodingASCII) ?? "" as CFString
    //        DispatchQueue.main.async {
    //            ModelState.shared.string = newString as String
    //        }
    //    }
    //}

    //func setLibString(string: String) {
    //    string.withCString { stringPointer in
    //        setString(stringPointer)
    //    }
    //}

    func increment() {
        count += 1
        state.increment()
    }

    func decrement() {
        count -= 1
        state.decrement()
    }
}
