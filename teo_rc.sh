teo() {
        echo compile $1
        gcc $1 -I/usr/local/include -L/usr/local/lib -lteo -Wall -DTEO_DEBUG_ALL
        if [ $? = 0 ]; then
                echo run source to ${@:2:($£-2)}
                ./a.out ${@:2:($£-2)}
        else
                echo compile Failure
        fi
}