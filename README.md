# socket-template

* パッと書ける範囲で，外部ライブラリに頼らない各言語のTCP, UDPソケットプログラムテンプレートを作ってみました。
* Linux, Windows両対応です。
* C++, C#, Pythonです。

* C++でWebSocketを自前で書くのは至難の業です。boost::asioをベースにした[websocketpp](https://github.com/zaphoyd/websocketpp)というヘッダオンリーライブラリがあるのでこちらを利用するのが良いでしょう。
* PythonでのWebSocketは外部ライブラリを利用すれば簡単にできるので割愛しています。
[PythonでWebSocket通信】websocketsのインストール](https://self-development.info/%E3%80%90python%E3%81%A7websocket%E9%80%9A%E4%BF%A1%E3%80%91websockets%E3%81%AE%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB/)