//
//  ContentView.swift
//
//  Created by Serge Vakulenko on 11/10/23.
//
import SwiftUI

public struct ContentView: View {
    @ObservedObject var proxy: ModelProxy = ModelProxy.shared

    public var body: some View {
        VStack {
            Text("\(proxy.count)")
            Button("+") {
                proxy.increment()
            }
            Button("-") {
                proxy.decrement()
            }
        }
        .padding()
    }

    public init() {}
}

#Preview {
    ContentView()
}
