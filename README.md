# socket-template

[![Gitpod ready-to-code](https://img.shields.io/badge/Gitpod-ready--to--code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/husty530/socket-template)  

#### How to Run in Gitpod Terminal
* Python
```
cd /workspace/mmap-template/py && python main.py
```
* C++ 
```
cd /workspace/mmap-template/cpp && ./main
```
* C#
```
cd /workspace/mmap-template/cs/bin/Debug/net7.0 && ./cs
```

* パッと書ける範囲で，外部ライブラリに頼らない各言語のソケットプログラムテンプレートを作ってみました。
* Linux, Windows両対応です。
* C++, C#, Pythonです。
* node.jsはrequire('net')...で秒で構築することができます。ここでは割愛します。

* C++でWebSocketを自前で書くのは至難の業です。boost::asioをベースにした[websocketpp](https://github.com/zaphoyd/websocketpp)というヘッダオンリーライブラリがあるのでこちらを利用するのが良いでしょう。
* PythonでのWebSocketは外部ライブラリを利用すれば簡単にできるので割愛しています。
[PythonでWebSocket通信】websocketsのインストール](https://self-development.info/%E3%80%90python%E3%81%A7websocket%E9%80%9A%E4%BF%A1%E3%80%91websockets%E3%81%AE%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB/)