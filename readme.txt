    ��������һ��ext2/ext3�ļ�ϵͳ�鿴��.���Դ򿪾����ļ���Ӳ���豸�ļ��鿴�����
�ļ�ϵͳ�ṹ��Ϣ,�糬���飬������Ϣ���ļ�inode��Ŀ¼��Ϣ���ļ���Ϣ��.
		
    1.�������
    ֱ����makefile����Ŀ¼������ make clean; make
		
		
    2.���ļ�ϵͳ:
		
    ֱ������ ./check_extfs �ļ�·����  �������г���. �ļ���������ͨ�ļ����豸�ļ�,��������
ext2��ext3��ʽ��.
    ����Ҫ�鿴����Ӳ�̵���Ϣ. ������mount�����df����鿴��Ӳ�̵��豸����
    ����mount �� df �������
		
    $ mount 
    /dev/sda1 on / type ext3 
    proc on /proc type proc (rw)
    ...
			
    $df
    Filesystem           1K-blocks      Used Available Use% Mounted on
    /dev/sda1             13788576   3584448   9503696  28% /
    udev                    254628       224    254404   1% /dev

    �ɲ����Ӳ�̶�Ӧ/dev/sda1�ļ�,��Ϊext3�ļ�ϵͳ. ����./check_extfs /dev/sda1 ��������г���.
    ����Ҳ�ɲ鿴ext2/3�ļ�����,�ļ�������������Բο���һ���ĵ�<<ext3�����ļ�����.txt>>
		
		
		
    3.����ʹ��
		
    �ɹ����г��������������״̬.������������
    ls	[dir] �鿴��ǰĿ¼��ϸ��Ϣ
    export [filepath]  �����ļ�ϵͳ��ĳ�ļ������ص�export_file
    filestat [filepath] �鿴ĳ�ļ�����ϸ��Ϣ
    cd  [dir]		�ı䵱ǰĿ¼
    pwd		��ʾ��ǰĿ¼
    info sb		�鿴��������Ϣ
    info inode num		�鿴���Ϊnum��inode��Ϣ
    info groups [start, end]	�鿴inode������
    help	����
    quit	�˳�
    
	
		