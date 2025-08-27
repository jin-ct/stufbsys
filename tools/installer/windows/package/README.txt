在此处放置安装压缩包，并命名为stufbsys.zip

目录结构：
	stufbsys.zip
	|---- bin							# 配置工具二进制文件目录
	|---- db_data						# pgsql数据库（已初始化并且已创建数据库和导入数据表结构）
	|---- server						# 后端二进制文件目录（包含重命名为stuFBSys_service.exe的WinSW以创建服务）
	|---- vendors
	|	 |---- nginx					# nginx目录，到nginx官网下载nginx包
	|	 |	  |---- conf				# 参考本文件同目录下的nginx/conf
	|	 |	  |---- cert				# 参考本文件同目录下的nginx/cert
	|	 |	  |---- nginx_service.exe	# WinSW二进制文件，用于创建服务，WinSW下载：https://github.com/winsw/winsw/releases
	|	 |	  └----   ... 				# nginx包内的其他文件
	|	 |	  
	|	 |---- pgsql					# pgsql目录，pgsql免安装包：https://www.enterprisedb.com/download-postgresql-binaries
	|	 └---- win-acme					# 用于一键申请ssl证书，官网：https://www.win-acme.com/
	|
	└---- web							# 前端目录，放置vue打包后文件（网站的root）
	 	 |---- admin					# 后台管理
	 	 |---- teacherAdmin				# 教师端后台
	 	 └----  ... 					# web目录下直接放置前台页面的打包后文件
