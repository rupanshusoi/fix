pushd /home/rsoi/fix
cmake --build build --parallel 256 \
&& \
build/src/tester/stateless-tester tree:3 string:none file:build/applications-prefix/src/applications-build/test/test-linked.wasm string:Hello
popd
