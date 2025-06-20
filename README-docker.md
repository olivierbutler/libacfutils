# Cross-compile to linux,windows from a mac

## Pre-requisites

- docker for mac 

that's it 

## Settings

```
# docker's internal path /xpl_dev is mapped 1 level up from the current folder
# the libacfutils folder is expected to be at the same level of your projet, if not
# modify docket-compose.yml accordingly. 

# Host folders         | Internal docker folders
# ---------------------|---------------------
# ../projets/          | /xpl_dev/
# ├── libacfutils/     | ├── libacfutils/
# └── X-RAAS2-xp12/    | └── X-RAAS2-xp12/

```

## Building the lib
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