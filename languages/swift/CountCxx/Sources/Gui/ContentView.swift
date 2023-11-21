//
//  ContentView.swift
//
//  Created by Serge Vakulenko on 11/10/23.
//
import SwiftUI

public struct ContentView: View {
    @ObservedObject var proxy: ModelProxy = ModelProxy.shared

    public var body: some View {
        HStack {
            Button("-") {
                proxy.decrement()
            }
            Text("\(proxy.count)")
            Button("+") {
                proxy.increment()
            }
        }
        .padding()
    }

    public init() {
        proxy.setup()
    }
}

#Preview {
    ContentView()
}
