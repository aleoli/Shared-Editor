# Shared-Editor

![Server](https://github.com/aleoli/Shared-Editor/workflows/Server/badge.svg)
![Client](https://github.com/aleoli/Shared-Editor/workflows/Client/badge.svg)

### Link alla documentazione

[wiki](https://github.com/aleoli/Shared-Editor/wiki)

## How to run it

### Docker (on linux)

Server
```bash
./scripts/run_server_docker.sh -m -l debug
```

Client
```bash
./scripts/run_client_docker.sh -l debug
```

If you want to run it at specific commit, run this before scripts in each shell
```bash
export TAG=<commit_sha>
```
