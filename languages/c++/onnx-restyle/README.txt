How to install package 'onnxruntime'.
Last version for now is 1.17.0.

On Mac:

    brew install onnxruntime

On Ubuntu x86_64, install to /usr/local:

    wget -O ort.zip https://www.nuget.org/api/v2/package/Microsoft.ML.OnnxRuntime/1.17.0
    unzip ort.zip
    cp runtimes/linux-x64/native/libonnxruntime.so /usr/local/lib/libonnxruntime.so.1.17.0
    ln -s libonnxruntime.so.1.17.0 /usr/local/lib/libonnxruntime.so
    cp -r build/native/include/ /usr/local/include/onnxruntime/

On Ubuntu arm64, install to /usr/local:

    wget -O ort.zip https://www.nuget.org/api/v2/package/Microsoft.ML.OnnxRuntime/1.17.0
    unzip ort.zip
    cp runtimes/linux-arm64/native/libonnxruntime.so /usr/local/lib/libonnxruntime.so.1.17.0
    ln -s libonnxruntime.so.1.17.0 /usr/local/lib/libonnxruntime.so
    cp -r build/native/include/ /usr/local/include/onnxruntime/
