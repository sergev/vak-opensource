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
    private var model = ModelCxx()

    func setup() {
        model.setup()
    }

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
