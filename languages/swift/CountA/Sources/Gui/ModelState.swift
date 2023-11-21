//
//  ModelState.swift
//
//  Created by Serge Vakulenko on 11/11/23.
//
import Foundation
import HelloCxx

class ModelState: ObservableObject {
    @Published var count: Int = 0

    static let shared = ModelState()

    // Allocate C++ object.
    var state = HelloCxx()

    func increment() {
        state.increment()
    }

    func decrement() {
        state.decrement()
    }
}

public func updateCount(val: Int) {
    ModelState.shared.count = val
}
