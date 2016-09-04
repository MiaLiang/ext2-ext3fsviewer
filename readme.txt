    本程序是一个ext2/ext3文件系统查看器.可以打开镜像文件及硬盘设备文件查看具体的
文件系统结构信息,如超级块，块组信息，文件inode，目录信息，文件信息等.
		
    1.程序编译
    直接在makefile所在目录下输入 make clean; make
		
		
    2.打开文件系统:
		
    直接输入 ./check_extfs 文件路径名  即可运行程序. 文件可以是普通文件和设备文件,但必须是
ext2或ext3格式的.
    例如要查看本地硬盘的信息. 先输入mount命令或df命令查看主硬盘的设备名。
    假设mount 和 df 结果如下
		
    $ mount 
    /dev/sda1 on / type ext3 
    proc on /proc type proc (rw)
    ...
			
    $df
    Filesystem           1K-blocks      Used Available Use% Mounted on
    /dev/sda1             13788576   3584448   9503696  28% /
    udev                    254628       224    254404   1% /dev

    可查出主硬盘对应/dev/sda1文件,且为ext3文件系统. 输入./check_extfs /dev/sda1 则可以运行程序.
    另外也可查看ext2/3文件镜像,文件镜像的制作可以参考另一个文档<<ext3镜像文件制作.txt>>
		
		
		
    3.命令使用
		
    成功运行程序进入命令输入状态.共有以下命令
    ls	[dir] 查看当前目录详细信息
    export [filepath]  导出文件系统中某文件到本地的export_file
    filestat [filepath] 查看某文件的详细信息
    cd  [dir]		改变当前目录
    pwd		显示当前目录
    info sb		查看超级块信息
    info inode num		查看序号为num的inode信息
    info groups [start, end]	查看inode组描述
    help	帮助
    quit	退出
    
	
		