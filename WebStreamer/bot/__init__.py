# This file is a part of TG-FileStreamBot
# Coding : Jyothis Jayanth [@EverythingSuckz]


import os
import os.path
from ..vars import Var
import logging
from pyrogram import Client
from urllib.parse import urlparse, unquote
from typing import Dict, Optional

def parse_proxy(proxy_str: str) -> Dict[str, Optional[str]]:
    """
    解析 PROXY 环境变量字符串，返回动态字典（仅包含存在的字段）
    
    Args:
        proxy_str (str): PROXY 字符串，格式如 `scheme://[username:password@]hostname:port`
    
    Returns:
        Dict: 包含存在的字段，格式如：
            {
                "scheme": "socks5",
                "hostname": "127.0.0.1",
                "port": 7897
            }
    """
    parsed = urlparse(proxy_str)
    
    if not parsed.scheme:
        raise ValueError("Missing scheme in proxy URL")
    if not parsed.hostname:
        raise ValueError("Missing hostname in proxy URL")
    if parsed.port is None:
        raise ValueError("Missing port in proxy URL")

    result = {
        "scheme": parsed.scheme,
        "hostname": parsed.hostname,
        "port": int(parsed.port)
    }
    
    if parsed.username is not None:
        username = unquote(parsed.username)
        password = unquote(parsed.password) if parsed.password else None
        
        result.update({
            "username": username
        })
        if password is not None:
            result["password"] = password
    
    return result

logger = logging.getLogger("bot")

sessions_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sessions")
if Var.USE_SESSION_FILE:
    logger.info("Using session files")
    logger.info("Session folder path: {}".format(sessions_dir))
    if not os.path.isdir(sessions_dir):
        os.makedirs(sessions_dir)

proxy_conf = None
print("PROXY: {} - {} - {}".format(Var.PROXY, len(Var.PROXY), type(Var.PROXY)))
if Var.PROXY:
    proxy_conf = parse_proxy(Var.PROXY)
    print("Use proxy: {}".format(proxy_conf))
else:
    print("No proxy")

StreamBot = Client(
    name="WebStreamer",
    api_id=Var.API_ID,
    api_hash=Var.API_HASH,
    workdir=sessions_dir if Var.USE_SESSION_FILE else "WebStreamer",
    plugins={"root": "WebStreamer/bot/plugins"},
    bot_token=Var.BOT_TOKEN,
    sleep_threshold=Var.SLEEP_THRESHOLD,
    workers=Var.WORKERS,
    in_memory=not Var.USE_SESSION_FILE,
    proxy=proxy_conf,
)

multi_clients = {}
work_loads = {}
