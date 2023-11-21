//
//  ContentView.swift
//
//  Created by Serge Vakulenko on 11/10/23.
//
import SwiftUI

struct ContentView: View {
    @ObservedObject var state: ModelState = ModelState.shared

    var body: some View {
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
}

#Preview {
    ContentView()
}
