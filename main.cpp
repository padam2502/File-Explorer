#include <string.h>
#include <cstring>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <termios.h>
#include <fstream>
#include <pwd.h>
#include <grp.h>
using namespace std;
//struct termios orig_termios;
int mx = 0;
string ROOT;
vector<dirent*> content;

#define HOME "/home/padam"
stack<string> back;
stack<string> forw;

int ptr = 0;
int top = 0;
int bottom = top + 10;
void normal_continue();
void curser(int x, int y)
{
	printf("\033[%d;%dH", x, y);
}
// void resetPointers(){
// 	top = 0;
// 	bottom = min(top+10, (int)content.size());
// 	ptr = 0;
// }
void command_init()
{
	struct winsize curr_win_size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &curr_win_size);
	for (int i = 0; i < curr_win_size.ws_row - 5; i++) cout << endl;
	cout << "command : ";
	curser(curr_win_size.ws_row - 4, 12);
	cout << "\033[0K";
}
void editorOpen();
void command_mode();
void scrollUpK()
{

	if (top == 0) return;
	top = max(top - 1, 0);
	bottom = bottom - 1;
	--ptr;
	editorOpen();
	curser(mx + 1, 0);
	return;
}

void scrollDownL()
{
	if (bottom == content.size()) return;
	bottom = min(bottom + 1, (int) content.size());
	top = top + 1;
	++ptr;
	editorOpen();
	curser(mx + 1, 0);
	return;
}

void read_curr()
{
	content.clear();
	char s[100];
	getcwd(s, 100);
	struct dirent * d;
	//struct stat sb{};
	DIR * dr;
	dr = opendir(".");
	if (dr != NULL)
	{
		for (d = readdir(dr); d != NULL; d = readdir(dr))
		{
			content.push_back((d));
		}
	}
}
void editorOpen()
{
	cout << "\033[2J\033[1;1H";
	char s[100];
	getcwd(s, 100);

	struct dirent * d;
	struct stat sb {};
	DIR * dr;
	dr = opendir(".");
	if (dr != NULL)
	{
		//cout<<"List of Files &Folders:-\n";
		//Owner	S_IRUSR = 1 << 8	S_IWUSR = 1 << 7	S_IXUSR = 1 << 6
		//Group	S_IRGRP = 1 << 5	S_IWGRP = 1 << 4	S_IXGRP = 1 << 3
		//Other	S_IROTH = 1 << 2	S_IWOTH = 1 << 1	S_IXOTH = 1 << 0

		for (int curr_it = top; curr_it < bottom; curr_it++)
		{
			d = content[curr_it];
			stat(d->d_name, &sb);
			struct passwd *owner_u = getpwuid(sb.st_uid);
			struct group *owner_g = getgrgid(sb.st_gid);
			string perm = "rwxrwxrwx";
			string permissions = "rwxrwxrwx";
			for (size_t i = 0; i < 9; i++)
			{
				permissions[i] = (sb.st_mode &(1 << (8 - i))) ? perm[i] : '-';
			}
			if ((sb.st_mode &S_IFDIR))
			{
				permissions = "d" + permissions;
			}
			else
				permissions = "r" + permissions;
			string times = ctime(&sb.st_mtime);
			double sizedoc = (sb.st_size) / 1024.0;
			cout << fixed << setprecision(2);
			cout << left << setw(25);
			cout << d->d_name << setw(10) << sizedoc << setw(10) << " KBytes" << setw(15) << permissions << setw(10) << owner_u->pw_name << setw(10) << owner_g->gr_name << setw(10) << times;
		}
	}
	else
		cout << "\nError Occurred!\n";
	curser(mx, 0);
}
char process_key()
{
	//int n = content.size();

	char ch;
	while (1)
	{
		ch = cin.get();
		switch (ch)
		{
			case 10:
				{
				 		// Enter

					if (top <= ptr && ptr <= bottom)
					{
						struct stat sb;

						stat((content[ptr]->d_name), &sb);
						if ((sb.st_mode &S_IFDIR))
						{
							cout << "\033[H\033[2J\033[3J";
							string dest_dir = content[ptr]->d_name;
							dest_dir.insert(0, "/");

							char s[100];
							getcwd(s, 100);
							if (ROOT == s && strcmp(content[ptr]->d_name, "..") == 0)
								dest_dir = s;
							else
								dest_dir = s + dest_dir;

							if (strcmp(content[ptr]->d_name, "..") == 0)
							{

								if (forw.empty())
								{
									forw.push(s);
								}
								if (forw.top() != s)
								{
									forw.push(s);
								}
							}
							else
							{
								if (back.empty())
								{
									back.push(s);
								}
								if (back.top() != s)
								{
									back.push(s);
								}
							}
							int err = chdir(dest_dir.c_str());
							if (err == -1)
							{
								cout << "ERROR Again" << dest_dir << endl;
							}

							ptr = 0;
							top = 0;
							mx = 0;
							read_curr();
							bottom = min(top + 10, (int) content.size());
							editorOpen();
							//n = content.size();
						}
						else
						{
							string dest_dir = content[ptr]->d_name;
							dest_dir.insert(0, "/");

							char s[100];
							getcwd(s, 100);
							dest_dir = s + dest_dir;
							pid_t pid = fork();
							if (pid == 0)
							{

								execl("/usr/bin/xdg-open", "xdg-open", dest_dir.c_str(), (char*) NULL);
								exit(0);
							}
							else if (pid > 0)
							{

								//cout<<"File open\n";
								//wait(0);
							}
							else {}	//cout<<"Error\n";}
						}
					}

					break;
				}
			case 'A':
				{
					// UP
					if (mx == 0)
						break;
					ptr--;
					--mx;
					//--top;
					//--bottom;
					printf("\033[1A");
					break;
				}
			case 'B':
				{
				 		// Down
					if (mx > bottom - top - 1)
						break;
					ptr++;
					++mx;
					//top++;
					//bottom++;
					printf("\033[1B");
					break;
				}
			case 127:
				{

					char s[100];
					getcwd(s, 100);
					if (s != ROOT)
					{
						if (forw.empty())
						{
							forw.push(s);
						}
						if (forw.top() != s)
						{
							forw.push(s);
						}
						cout << "\033[H\033[2J\033[3J";
						string dest_dir = s;
						for (int i = dest_dir.size() - 1; dest_dir[i] != '/'; --i)
						{
							dest_dir.pop_back();
						}
						dest_dir.pop_back();
						chdir(dest_dir.c_str());
						ptr = 0;
						mx = 0;
						curser(mx, 0);
						top = 0;
						//bottom=10;
						read_curr();
						bottom = min(top + 10, (int) content.size());
						editorOpen();
						//n = content.size();
					}
					break;
				}
			case 'h':
				{
					cout << "\033[H\033[2J\033[3J";
					while (!forw.empty())
						forw.pop();
					//cout << "\033[H\033[2J\033[3J" ;
					chdir(ROOT.c_str());
					//contents.clear();

					ptr = 0;
					top = 0;
					mx = 0;
					curser(mx, 0);
					read_curr();

					bottom = min(top + 10, (int) content.size());
					editorOpen();
					//n = content.size();
					break;
				}
			case 'C':
				{
					if (!forw.empty())
					{
						cout << "\033[H\033[2J\033[3J";
						string dest_dir = forw.top();
						forw.pop();
						char s[100];
						getcwd(s, 100);
						if (back.empty())
						{
							back.push(s);
						}
						if (back.top() != s)
						{
							back.push(s);
						}
						chdir(dest_dir.c_str());
						//contents.clear();

						ptr = 0;
						top = 0;
						mx = 0;
						curser(mx, 0);
						read_curr();

						//n = content.size();
						bottom = min(top + 10, (int) content.size());
						editorOpen();
					}
					break;
				}
			case 'D':
				{
					char s[100];
					getcwd(s, 100);
					if (!back.empty())
					{
						cout << "\033[H\033[2J\033[3J";
						if (forw.empty())
						{
							forw.push(s);
						}
						if (forw.top() != s)
						{
							forw.push(s);
						}
						string dest_dir = back.top();
						back.pop();
						chdir(dest_dir.c_str());
						//content.clear();

						ptr = 0;
						mx = 0;
						top = 0;
						curser(mx, 0);
						read_curr();

						bottom = min(top + 10, (int) content.size());
						editorOpen();
						//n = content.size();
					}
					break;
				}
			case ':':
				{
					cout << "\033[H\033[2J\033[3J";
					command_mode();
					return ch;
				}
			case 'k':
				{
					scrollUpK();
					break;
				}
			case 'l':
				{
					scrollDownL();
					break;
				}
			case 'q':
				{
					cout << "\033[H\033[2J\033[3J";
					return ch;
				}
		}
	}
	return ch;
}
void normal_mode()
{
	char ch;
	//vector<string> contents;
	read_curr();
	bottom = min(10, (int) content.size());
	//curser(1,1);
	editorOpen();
	return;
	//
}
void delete_file(string dst)
{
	if (!(remove(dst.c_str()))) cout << "Removed                ";
	else
		printf("%s\n", strerror(errno));
}
void recursive_del(string dest)
{
	DIR * di;
	struct dirent * diren;
	struct stat finfo;

	if (!(di = opendir(dest.c_str())))
	{
		cout << "Can't open the directory" << endl;
		return;
	}
	chdir(dest.c_str());
	while ((diren = readdir(di)))
	{
		stat(diren->d_name, &finfo);
		if (!(finfo.st_mode &S_IFDIR))
		{
			//cout<<diren->d_name<<endl;
			unlink(diren->d_name);
		}
		else
		{
			if (strcmp(".", diren->d_name) == 0 || strcmp("..", diren->d_name) == 0)
			{
				continue;
			}
			recursive_del(diren->d_name);
			//cout<<diren->d_name<<endl;
			rmdir(diren->d_name);
		}
	}
	chdir("..");
	closedir(di);
}

void delete_dir(string dst)
{
	char s[100];
	getcwd(s, 100);
	string wd = s;
	if (dst != wd)
	{
		recursive_del(dst);
	}
	//cout<<dst.c_str();
	rmdir(dst.c_str());

}
void move_file(string source, string dst)
{
	struct stat info;
	stat(source.c_str(), &info);
	__mode_t permission = info.st_mode;
	char old_name[source.size() + 1];
	copy(source.begin(), source.end(), old_name);
	old_name[source.size()] = '\0';
	char new_name[dst.size() + 1];
	copy(dst.begin(), dst.end(), new_name);
	new_name[dst.size()] = '\0';
	if (rename(old_name, new_name) != 0)
		printf("Error moving file                         ");
	else
		cout << "File moved successfully                   ";
	chmod(new_name, permission);
	chown(new_name, info.st_uid, info.st_gid);

}

void copy_file(string source, string dst, string name)
{
	struct stat info;
	stat(source.c_str(), &info);
	__mode_t permission = info.st_mode;
	char *temp = &source[0];
	char *dir = &dst[0];
	ifstream src(source, std::ios::binary);
	ofstream dest(dst, std::ios::binary);
	dest << src.rdbuf();
	src.close();
	dest.close();
	chmod(dir, permission);
	chown(dir, info.st_uid, info.st_gid);
}
void copy_dir(string source, string dst, string name)
{
	vector<string> dr_content;
	struct stat info;
	stat(source.c_str(), &info);
	__mode_t permission = info.st_mode;
	string next_path;
	(mkdir(dst.c_str(), permission));
	struct dirent * d;
	DIR * dr;
	dr = opendir(&source[0]);

	for (d = readdir(dr); d != NULL; d = readdir(dr))
	{
		stat(d->d_name, &info);
		if ((strcmp(".", d->d_name) == 0) || (strcmp("..", d->d_name) == 0))
			continue;
		dr_content.push_back(d->d_name);
	}
	for (auto i = dr_content.begin(); i != dr_content.end(); i++)
	{
		{
			next_path = source + "/" + (*i);
			struct stat n_info;
			stat(next_path.c_str(), &n_info);
			string pass_source = source + "/" + (*i);
			string pass_dest = dst + "/" + (*i);
			if (!(n_info.st_mode &S_IFDIR))
			{
				copy_file(pass_source, pass_dest, (*i));
			}
			else
			{

				copy_dir(pass_source, pass_dest, (*i));
			}
		}
	}
}
void move_dir(string source, string dst, string name)
{
	copy_dir(source, dst, name);
	delete_dir(source);
	//rmdir(dname.c_str());	
}
bool search_file(string s, string des)
{
	struct dirent * d;
	struct stat sb;
	DIR * dr;
	dr = opendir(&des[0]);
	if (dr != NULL)
	{

		for (d = readdir(dr); d != NULL; d = readdir(dr))
		{
			stat(d->d_name, &sb);

			if (strcmp(&s[0], d->d_name) == 0)
				return true;
			if ((strcmp(".", d->d_name) == 0) || (strcmp("..", d->d_name) == 0))
				continue;
			if (S_ISDIR(sb.st_mode))
			{
				if (search_file(s, des + "/" + d->d_name))
					return true;
			}
		}
	}
	return false;
}
void command_mode()
{
	cout << "\033[H\033[2J\033[3J";
	command_init();
	vector<string> cmd;
	string str = "";
	while (1)
	{

		char ch;
		ch = cin.get();
		cout << ch;
		switch (ch)
		{
			case 27:
				{
					normal_continue();
					break;
				}
			case 127:
				{
					if (str.size())
					{
						str.pop_back();
						// 
						cout << "\b \b";
					}
					break;
				}
			case 10:
				{
				 		//cout<<str;
					cmd.clear();
					string tt = "";
					int i = 0;
					int j = 0;
					for (int i = 0; i < str.size(); i++)
					{
						if (str[i] == ' ')
						{
							cmd.push_back(tt);
							++j;
							tt.clear();
						}
						else
						{
							tt.push_back(str[i]);
						}
					}
					cmd.push_back(tt);

					if (cmd[0] == "copy")
					{
						string dest_dir = "";
						string src_dir = "";
						char s[100];
						getcwd(s, 100);
						string cw = s;
						int x = cmd.size() - 1;
						if (cmd[x][0] == '~')
						{
							dest_dir = ROOT + cmd[x].substr(1) + "/";
						}
						else if (cmd[x][0] == '/')
						{
							dest_dir = cmd[x] + "/";
						}
						else if (cmd[x][0] == '.')
						{

							dest_dir = cw + "/";
						}
						else
						{
							dest_dir = cw + "/" + cmd[x] + "/";
						}
						//cout<<dest_dir;
						cmd.pop_back();
						for (int k = 1; k < cmd.size(); k++)
						{
						 				//cout<<cmd[k];
							if (cmd[k][0] == '~')
							{
								src_dir = ROOT + cmd[k].substr(1) + "/";
							}
							else if (cmd[k][0] == '/')
							{
								src_dir = cmd[k] + "/";
							}
							else if (cmd[k][0] == '.')
							{

								src_dir = cw + "/";
							}
							else
							{
								src_dir = cw + "/" + cmd[k] + "/";
							}
							string fname = "";
							for (int i = src_dir.size() - 2; i >= 0; --i)
							{
								if (src_dir[i] == '/') break;
								else
								{
									fname.push_back(src_dir[i]);
								}
							}
							reverse(fname.begin(), fname.end());
							//src_dir += fname;
							string dstdr = dest_dir + fname;
							//dest_dir += fname;
							src_dir.pop_back();
							struct stat sb;
							stat(src_dir.c_str(), &sb);
							if (!(sb.st_mode &S_IFDIR))
								copy_file(src_dir, dstdr, fname);
							else
							{
								copy_dir(src_dir, dstdr, fname);
							}
						}
					}
					else if (cmd[0] == "rename")
					{
						char old_name[cmd[1].size() + 1];
						copy(cmd[1].begin(), cmd[1].end(), old_name);
						old_name[cmd[1].size()] = '\0';
						char new_name[cmd[2].size() + 1];
						copy(cmd[2].begin(), cmd[2].end(), new_name);
						new_name[cmd[2].size()] = '\0';
						if (!rename(old_name, new_name))
							cout << "Renamed                              ";
					}
					else if (cmd[0] == "move")
					{
						string dest_dir = "";
						string src_dir = "";
						char s[100];
						getcwd(s, 100);
						string cw = s;
						int x = cmd.size() - 1;
						if (cmd[x][0] == '~')
						{
							dest_dir = ROOT + cmd[x].substr(1) + "/";
						}
						else if (cmd[x][0] == '/')
						{
							dest_dir = cmd[x] + "/";
						}
						else if (cmd[x][0] == '.')
						{
							dest_dir = cw + "/";
						}
						else
						{
							dest_dir = cw + "/" + cmd[x] + "/";
						}
						//cout<<dest_dir;
						cmd.pop_back();

						for (int k = 1; k < cmd.size(); k++)
						{
							if (cmd[k][0] == '~')
							{
								src_dir = ROOT + cmd[k].substr(1) + "/";
							}
							else if (cmd[k][0] == '/')
							{
								src_dir = cmd[k] + "/";
							}
							else if (cmd[k][0] == '.')
							{

								src_dir = cw + "/";
							}
							else
							{
								src_dir = cw + "/" + cmd[k] + "/";
							}
							string fname = "";
							for (int i = src_dir.size() - 2; i >= 0; --i)
							{
								if (src_dir[i] == '/') break;
								else
								{
									fname.push_back(src_dir[i]);
								}
							}
							reverse(fname.begin(), fname.end());
							string dstdr = dest_dir + fname;

							// char *temp = &src_dir[0];
							// char *dir = &dest_dir[0];

							/*	Deletes the file if exists */
							src_dir.pop_back();
							struct stat sb;
							stat(src_dir.c_str(), &sb);
							if (!(sb.st_mode &S_IFDIR))
							{

								move_file(src_dir, dstdr);
							}
							else
							{
								move_dir(src_dir, dstdr, fname);
							}
						}
					}
					else if (cmd[0] == "delete_file")
					{
						string dest_dir = "";
						int x = cmd.size() - 1;
						char s[100];
						getcwd(s, 100);
						string cw = s;
						if (cmd[x][0] == '~')
						{
							dest_dir = ROOT + cmd[x].substr(1) + "/";
						}
						else if (cmd[x][0] == '/')
						{
							dest_dir = cmd[x] + "/";
						}
						// else if(cmd[x][0]=='.')
						// {  			// 	dest_dir= cw + "/";
						// }
						else
						{
							dest_dir = cw + "/" + cmd[x] + "/";
						}
						// cmd.pop_back();
						// string fname="";
						// for(int i=dest_dir.size()-2;i>=0;--i)
						// 	{ 	 			// 		if(dest_dir[i]=='/')break;
						// 		else
						// 		{ 		 			// 			fname.push_back(dest_dir[i]);
						// 		}
						// 	}
						// 	reverse(fname.begin(), fname.end());
						// dest_dir+=fname;

						dest_dir.pop_back();
						delete_file(dest_dir);
					}
					else if (cmd[0] == "delete_dir")
					{
						string dest_dir = "";
						int x = cmd.size() - 1;
						char s[100];
						getcwd(s, 100);
						string cw = s;
						if (cmd[x][0] == '~')
						{
							dest_dir = ROOT + cmd[x].substr(1) + "/";
						}
						else if (cmd[x][0] == '/')
						{
							dest_dir = cmd[x] + "/";
						}
						// else if(cmd[x][0]=='.')
						// {  			// 	dest_dir= cw + "/";
						// }
						else
						{
							dest_dir = cw + "/" + cmd[x] + "/";
						}
						dest_dir.pop_back();
						delete_dir(dest_dir);
					}
					// 			> perror 0
					// OS error code   0:  Success
					// > perror 1
					// OS error code   1:  Operation not permitted
					// > perror 2
					// OS error code   2:  No such file or directory
					// > perror 3
					// OS error code   3:  No such process
					// > perror 4
					// OS error code   4:  Interrupted system call
					// > perror 5
					// OS error code   5:  Input/output error
					else if (cmd[0] == "create_file")
					{
						string dest_dir = "";
						int x = cmd.size() - 1;
						char s[100];
						getcwd(s, 100);
						string cw = s;
						if (cmd[x][0] == '~')
						{
							dest_dir = ROOT + cmd[x].substr(1) + "/";
						}
						else if (cmd[x][0] == '/')
						{
							dest_dir = cmd[x] + "/";
						}
						else if (cmd[x][0] == '.')
						{
							dest_dir = cw + "/";
						}
						else
						{
							dest_dir = cw + "/" + cmd[x] + "/";
						}
						cmd.pop_back();
						string fname = cmd[1];
						dest_dir += fname;
						fstream file;
						file.open(dest_dir, ios::out);
						file.close();
					}
					else if (cmd[0] == "create_dir")
					{
						string dest_dir = "";
						int x = cmd.size() - 1;
						char s[100];
						getcwd(s, 100);
						string cw = s;
						if (cmd[x][0] == '~')
						{
							dest_dir = ROOT + cmd[x].substr(1) + "/";
						}
						else if (cmd[x][0] == '/')
						{
							dest_dir = cmd[x] + "/";
						}
						else if (cmd[x][0] == '.')
						{
							dest_dir = cw + "/";
						}
						else
						{
							dest_dir = cw + "/" + cmd[x] + "/";
						}
						string fname = cmd[1];

						dest_dir += fname;
						if (!(mkdir(dest_dir.c_str(), 0777))) cout << "Directory created\n";
						else
							cout << "NOT created\n";
					}
					else if (cmd[0] == "goto")

					{

						string dest_dir = "";
						int x = cmd.size() - 1;
						char s[100];
						getcwd(s, 100);
						string cw = s;
						if (cmd[x][0] == '~')
						{
							dest_dir = ROOT + cmd[x].substr(1) + "/";
						}
						else if (cmd[x][0] == '/')
						{
							dest_dir = cmd[x] + "/";
						}
						else if (cmd[x][0] == '.')
						{
							dest_dir = cw + "/";
						}
						else
						{
							dest_dir = cw + "/" + cmd[x] + "/";
						}

						dest_dir.pop_back();
						cmd.pop_back();
						chdir(dest_dir.c_str());
					}
					else if (cmd[0] == "search")
					{
						string file_name = cmd[1];
						char s[100];
						getcwd(s, 100);
						string dest_dir = s;
						if (search_file(file_name, dest_dir))
						{
							cout << "True                       " << endl;
						}
						else
						{
							cout << "False                         " << endl;
						}
					}
					str.clear();
					cmd.clear();
					curser(0, 0);
					command_init();
					break;
				}
			default:
				{
					str += ch;
					break;
				}
		}
	}
}

struct termios orig_termios;

void disableRawMode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disableRawMode);

	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void normal_continue()
{
	char s[100];
	getcwd(s, 100);
	ROOT = s;
	cout << "\033[2J\033[1;1H";
	normal_mode();
	//curser(1,1);
	while (1)
	{
		//curser(1, 1);
		if (process_key() == 'q')
			exit(0);
		if (process_key() == ':')
			command_mode();
	}
}
int main()
{

	enableRawMode();
	char c;
	normal_continue();
	return 0;
}
