//
//  ModelProxy.swift
//
//  Created by Serge Vakulenko on 11/11/23.
//
import Foundation
import ModelCxx

class ModelProxy: ObservableObject {
    @Published var count: Int = 0

    static let shared = ModelProxy()

    // Allocate C++ object.
    var model = ModelCxx()

    func increment() {
        model.increment()
    }

    func decrement() {
        model.decrement()
    }
}

public func updateCount(val: Int) {
    ModelProxy.shared.count = val
}
