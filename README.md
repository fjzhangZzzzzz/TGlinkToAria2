# TGlinkToAria2

基于 C++ 实现的 Telegram 文件流媒体机器人，支持将文件转换为可流式访问的链接，并自动添加到 Aria2 下载器。

## 功能特性

- 接收 Telegram 中的文件（文档、视频、音频、动画、语音、视频笔记、照片、贴纸）
- 生成文件的流式访问链接（长链接和短链接）
- 支持将链接自动添加到 Aria2 下载器
- 支持代理配置

## 依赖项

- C++17 或更高版本
- CMake 3.15 或更高版本
- tgbot-cpp：Telegram Bot API C++ 库
- cpp-httplib：HTTP 服务器库
- nlohmann/json：JSON 解析库
- spdlog：日志库
- cpp-dotenv：环境变量库
- aria2：Aria2 下载器

## 构建

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 构建项目
cmake --build .

# 运行测试
ctest
```

## 配置

创建 `.env` 文件并设置以下环境变量：

```env
API_ID=your_api_id
API_HASH=your_api_hash
BOT_TOKEN=your_bot_token
BIN_CHANNEL=your_channel_id
PORT=8080
BIND_ADDRESS=0.0.0.0
PROXY=scheme://username:password@hostname:port
ARIA2=true
RPC_URLS=your_aria2_rpc_url
RPC_PORTS=your_aria2_rpc_port
RPC_TOKENS=your_aria2_rpc_token
```

## 许可证

MIT License
