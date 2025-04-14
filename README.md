# 说明

基于 [snakexgc/TGlinkToAria2](https://github.com/snakexgc/TGlinkToAria2) 增加 tg 代理配置，基本使用方法请参照原项目。

增加的代理配置方法：
- 增加 PROXY 环境变量，格式为 `scheme://username:password@hostname:port`，如 `http://127.0.0.1:7890`。可选在 docker-compose.yml 中增加环境变量配置或者 `/app/.env` 中增加 `PROXY=xxx` 项
- `scheme` 代理协议，如 `socks5`、`http`
- `username` 和 `password` 可选，若不需要身份验证可忽略
- `hostname` 代理主机
- `port` 代理端口
