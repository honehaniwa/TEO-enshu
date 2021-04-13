# TTのTEO演習

菅谷研の学習用課題のTEO演習をやったリポジトリ

# このリポジトリの利点
- shellscriptでコマンド1個で実行できる！

- 一応実装済のプログラムを置いておく

- 詰まったところなどを随時更新予定

# shellscript実行方法
1. teo.shをローカルに落としてくる(リポジトリクローンかコピペでも)
2. ターミナルで`chmod 755`と打って権限を付与
3. `./teo.sh <ソースコード.c> <プログラム引数1> <プログラム引数2> <3...>`

- 例: 3の`copy.c`実行方法
```sh
$ cd ./3
$ ./teo.sh copy.c TT.teo TT-out.teo
```

## おまけ
`~/.bashrc(/.zshrc)`用(わかる人向け)
(もし知りたかったら[ここ](https://qiita.com/yutat93/items/b5bb9c0366f21bcbea62)見て)
```sh
teo() {
    echo compile $1
    gcc $1 -I/usr/local/include -L/usr/local/lib -lteo -Wall -DTEO_DEBUG_ALL
    echo run source to ${@:2:($#-2)}
    ./a.out ${@:2:($#-2)}
}
```

- 例: 3の`copy.c`実行方法
```sh
$ cd ./3
$ teo copy.c ../TT.teo TT-out.teo
```

# リンク
- 1~2章
    - 特になし。インストールバトルとTEOフォーマットの説明
- [3章](./3/3.md)
    - チュートリアル。実際に動かしてみようの章。
- [4章](./4/4.md)
    - 色々画像処理っぽいことやろう！の章
- [5章](./5/5.md)
    - ここから本番。今年はパノラマ画像生成まででいいらしいのでここまでで終わり

# 参考文献
- [TEOリポジトリ](http://teo.osdn.jp/)
- [学内(研究室内)用](http://iim.cs.tut.ac.jp/member/sugaya/TEO/)
- [学外用](http://teo.osdn.jp/doc/TeoProgrammingGuide/index.html)

# contact
[@TT_beginner](https://twitter.com/tt_beginner)
