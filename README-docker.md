# cross-compile to linux,windows from a mac

just run : 
```
docker compose run --rm win-lin-build
```
inside the docker:
```
cd libacfutils
```
then, the same commands
```
./build_deps
./build_redist
```