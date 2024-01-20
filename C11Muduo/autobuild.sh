set -e

# 检查是否存在build目录，如果不存在则创建
if [ ! -d "build" ]; then
    mkdir build
fi

rm -fr build/*

cd build &&
    cmake .. && 
    make

# 将头文件复制到/usr/include/C11Muduo，将.so库文件复制到/usr/lib

if [ ! -d /usr/include/C11Muduo ]; then
    mkdir /usr/include/C11Muduo
fi

for header in $(ls ../*.h)
do
    cp $header /usr/include/C11Muduo
done

cp ../lib/libC11Muduo.so /usr/lib

ldconfig
