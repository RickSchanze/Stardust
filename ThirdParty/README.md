# ThirdParty

第三方库以 **git submodule** 形式放在本目录，**禁止** FetchContent / 自动下载。

| 库 | 路径 | 仓库 |
|----|------|------|
| spdlog | `ThirdParty/spdlog` | https://github.com/RickSchanze/spdlog.git |
| mimalloc | `ThirdParty/mimalloc` | https://github.com/RickSchanze/mimalloc |
| glm | `ThirdParty/glm` | https://github.com/g-truc/glm.git |

```bash
git submodule update --init --recursive

# GitHub 拉不下时走本地代理 7890
git -c http.proxy=http://127.0.0.1:7890 -c https.proxy=http://127.0.0.1:7890 submodule update --init --recursive
```

升级某库：进入子模块 `git fetch` / `git checkout <tag>`，回主仓库提交指针。
