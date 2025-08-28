# 学生反馈网站系统
### 概述

这是一个适用于中学的学生反馈网站项目，具有简单易用的特点，您无需任何技术即可在服务器上完成网站部署，并快速投入使用，使用该网站系统可有效拓宽学生反馈渠道，促进学生与学校共同发展。该项目后端使用c++/drogon框架构建，前端使用vue3+vite构建。

该网站系统具有以下优点：

- 简单易用，无需任何技术即可完成网站搭建
- 高度匿名，让学生能大胆写出内心真实的想法和对学校的建议
- 投票公平，系统通过浏览器指纹+IP+手写轨迹多重验证，实现无需用户输入隐私信息来判断唯一性，并保证用户无法重复投票
- 支持单独对老师提建议，学校和其他老师无法查看（除公开的建议外）
- 支持AI评分等AI功能
- 用户可自由选择是否公开及是否匿名
- 可自定义网站标题、Logo等
- UI整体简洁美观

## 开始使用

### 方式一：使用一键部署工具（推荐）

一键部署目前仅支持Windows系统服务器，[点击此处](https://github.com/jin-ct/stufbsys/blob/master/docs/%E4%B8%80%E9%94%AE%E9%83%A8%E7%BD%B2%E8%AF%A6%E7%BB%86%E6%93%8D%E4%BD%9C%E8%AF%B4%E6%98%8E.md)查看详细部署说明。

### 方式二：自定义部署

使用自定义部署则默认你已熟悉Nginx、PostgreSQL的安装及基本配置，目前后端仅有Windows系统的打包文件，如需要Linux系统运行请自行编译。

#### 1. 发布版打包文件下载

到[此处](https://github.com/jin-ct/stufbsys/releases)下载最新的前端、后端已构建完成的包。

#### 2. 初始化数据库

可使用以下命令初始化数据库，也可自行使用其他方式，在仓库的根目录可获取[stufbsys_db.sql](https://github.com/jin-ct/stufbsys/blob/master/stufbsys_db.sql)

```bash
psql -U "posetgres" -c "CREATE DATABASE stufbsys;"  # 创建数据库
psql -U "posetgres" -d "stufbsys" -f "/stufbsys_db.sql"
# -f "/stufbsys_db.sql" 是你的stufbsys_db.sql文件路径
```

#### 3. 部署后端

将`server-win64.zip`解压到任意合适的位置，编辑`config.json`配置文件，完成数据库连接配置，可参考[drogon文档：数据库配置](https://drogonframework.github.io/drogon-docs/#/CHN/CHN-11-%E9%85%8D%E7%BD%AE%E6%96%87%E4%BB%B6?id=db_clients%e6%95%b0%e6%8d%ae%e5%ba%93%e5%ae%a2%e6%88%b7%e7%ab%af)

并编辑以下自定义选项

```
// 其他配置...
{
    "name": "JwtPlugin",
    "dependencies": [],
    "config": {
        	"secret": "",                      // jwt的加密密钥，数字和字母组成
        	"sessionTime": 86400,  			   // jwt会话时间，单位为s，通常不用更改
    }
}
// 其他配置...
"custom_config": {
    "defaultTeacherPassword": "teacher123456", // 教师后台默认密码
    "encryptTextKey": "",                      // 32字节，ase加密的密钥，数字和字母组成
    "encryptTextIv": "",                       // 16字节，ase加密的偏移量，数字和字母组成
    "verifyImageCompareThreshold": 0.86,       // 轨迹图片对比阈值，通常不用更改
    "verifyImageSimpleCompareThreshold": 0.75  // 轨迹图片简单对比阈值，通常不用更改
}
```

完成配置后直接启动`stuFBSys.exe`即可，如需创建Windows服务，请使用[winsw](https://github.com/winsw/winsw)

#### 4. 部署前端

将`front-admin.zip`、`front-teacherAdmin.zip` 、`front-reception.zip`解压到nginx网站根目录，请使用以下目录结构

```
网站根目录
|--- admin           # front-admin.zip解压文件
|--- teacherAdmin    # front-teacherAdmin.zip解压文件
└─-- ···             # front-reception.zip解压文件，直接放至网站根目录
```

nginx配置可参考[此处](https://github.com/jin-ct/stufbsys/blob/master/tools/installer/windows/package/nginx/conf/nginx.conf)

### 方式三：源码编译

[点此查看](https://github.com/jin-ct/stufbsys/blob/master/docs/%E6%BA%90%E7%A0%81%E7%BC%96%E8%AF%91.md)

## 主要依赖

后端：[drogon](https://github.com/drogonframework/drogon)、[opencv](https://github.com/opencv/opencv)

前端：[vue.js](https://cn.vuejs.org/)、[element-plus](https://github.com/element-plus/element-plus)、[pure-admin](https://github.com/pure-admin/vue-pure-admin)

配置工具、一键部署工具：[Qt](https://www.qt.io/)
