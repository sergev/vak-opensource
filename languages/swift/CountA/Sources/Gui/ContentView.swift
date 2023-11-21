//
//  ContentView.swift
//
//  Created by Serge Vakulenko on 11/10/23.
//
import SwiftUI

public struct ContentView: View {
    @ObservedObject var state: ModelState = ModelState.shared

    public var body: some View {
        VStack {
            Text("\(state.count)")
            Button("+") {
                state.increment()
            }
            Button("-") {
                state.decrement()
            }
        }
        .padding()
    }

    public init() {}
}

#Preview {
    ContentView()
}
