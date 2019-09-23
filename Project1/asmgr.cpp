#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;

// 参数: 配置文件位置
int main(int argc, char**argv)
{

	//// 网络连通性测试, 若失败, 直接退出; 在service中添加失败自动重启功能
	//int stat = system("/usr/bin/python3 check_network.py");

	//if (stat != 0)
	//{
	//	cout << "network error! errno:" << stat << "; abort." << endl;
	//	return -1;
	//}

	char* defPath;
	char* defConfPath;
	char* defRsaPath;
	// 默认配置文件位置

	if (argc < 4)
	{
		defPath = "/usr/bin/autossh";
		defConfPath = "/opt/conf/autossh.conf";
		defRsaPath = "/root/.ssh/id_rsa";
		cout << "too few arguments." << endl
			<< "using default autossh path:" << defPath << endl
			<< "using default config file:" << defConfPath << endl
			<< "using default rsa file:" << defRsaPath << endl;
	}
	else
	{
		defPath = argv[1];
		defConfPath = argv[2];
		defRsaPath = argv[3];
	}

	// 读取配置文件
	ifstream ifs;
	ifs.open(defConfPath);
	vector<string> confs;
	while (1)
	{
		char buf[64];
		ifs.getline(buf, 64);
		string conf(buf);
		if (conf.length() > 10)
		{
			confs.push_back(conf);
		}
		else
		{
			break;
		}
	}
	ifs.close();

	// 开始fork
	int size = confs.size();
	for (int iconf = 0; iconf < size; ++iconf)
	{
		int pid = fork();
		if (pid == 0)
		{
			string conf = confs[iconf];

			int psrc = conf.find_first_not_of(' ');
			int pesrc_1 = conf.find_first_of(' ', psrc);

			int pdst = conf.find_first_not_of(' ', pesrc_1);
			int pedst_1 = conf.find_first_of(' ', pdst);

			int phst = conf.find_first_not_of(' ', pedst_1);

			string arg1_s = conf.substr(pdst, pedst_1 - pdst) + ":127.0.0.1:" + conf.substr(psrc, pesrc_1 - psrc);
			string arg2_s = conf.substr(phst);

			char arg1[24];
			int len1 = arg1_s.length();
			len1 = len1 > 24 ? 24 : len1;
			int i = 0;
			for (; i < len1; ++i)
			{
				arg1[i] = arg1_s[i];
			}

			arg1[i] = '\0';

			char arg2[40];
			int len2 = arg2_s.length();
			len2 = len2 > 40 ? 40 : len2;
			for (i = 0; i < len2; ++i)
			{
				arg2[i] = arg2_s[i];
			}
			arg2[i] = '\0';

			char* args[] = {
				defPath, 
				"-NR",
				arg1,
				arg2,
				"-i",
				defRsaPath,
				(char*)0
			};
			cout << "subproc: "<< getpid() << " ; port: " << arg1 << " ; host: " << arg2 << endl;
			execv(defPath, args);
			exit(0);
		}
	}

	wait(NULL);
	return 0;
}