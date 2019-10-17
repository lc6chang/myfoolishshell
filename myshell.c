#include<stdio.h>
#include<sys/types.h>
#include<pwd.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>


char buff[1024]={'\0'};
char *cmd[20];

void showShell();
void getInput();
void commands();

void ls();
void cd();
void echo();
void cat();

void ls2file(int flag);
void echo2file(int flag);
void cat2file(int flag);

void cat2grep();

int main(){
        printf("Start myshell!\n");
        /*进入终端循环*/
        while(1){
                /*显示终端提示符*/
                showShell();
                /*获取用户输入*/
                getInput();
                if(buff[0]=='\0'){
                        continue;
                }
                /*分析并执行指令*/
                commands();
        }
        printf("Stop myshell!\n");
}



void showShell(){
        uid_t uid;
        char hostname[100];
        struct passwd *user;
        char cwd[200];

        uid=getuid();
        user=getpwuid(uid);
        gethostname(hostname,100);
        getcwd(cwd,200);
        printf("%s@",user->pw_name);
        printf("%s:",hostname);
        printf("%s",cwd);
        if(uid==0){
                printf("# ");
        }
        else{
                printf("$ ");
        }

        fflush(stdout);
}

void getInput(){
        char ch;
        int len=0;
        buff[0]='\0';

        while(1){
                ch=getchar();
                if(ch=='\n'){
                        buff[len++]='\0';
                        return;
                }
                buff[len++]=ch;
        }
}

void commands(){
        int type=0;
        int flag=0;
        if(strstr(buff,">")){
                type=1;
                if(strstr(buff,">>")){
                        flag=1;
                }
        }
        else if(strstr(buff,"|")){
                type=2;
        }
        int len=0;
        char *ret;
        ret=strtok(buff," ");
        while(1){
                cmd[len++]=ret;
                if(ret==NULL){
                        break;
                }
                ret=strtok(NULL," ");
        }
        if(type==1){
                if(strcmp(cmd[0],"ls")==0){
                        ls2file(flag);
                        return;
                }
                if(strcmp(cmd[0],"echo")==0){
                        echo2file(flag);
                        return;
                }
                if(strcmp(cmd[0],"cat")==0){
                        cat2file(flag);
                        return;
                }
                printf("Error!\n");
                return;
        }
        if(type==2){
                if(strcmp(cmd[0],"cat")==0){
                        cat2grep();
                        return;
                }
                printf("Error!\n");
        }

        if(strcmp(cmd[0],"exit")==0){
                printf("Stop myshell!\n");
                exit(0);
        }
        if(strcmp(cmd[0],"ls")==0){
                ls();
                return;
        }
        if(strcmp(cmd[0],"cd")==0){
                cd();
                return;
        }
        if(strcmp(cmd[0],"echo")==0){
                echo();
                return;
        }
        if(strcmp(cmd[0],"cat")==0){
                cat();
                return;
        }
        printf("Error!\n");
        return;
}



void  ls(){
        DIR *dir;
        struct dirent *it;
        dir=opendir(".");
        while((it=readdir(dir))){
                if(it->d_name[0]!='.'){
                        printf("%s ",it->d_name);
                }
        }
        printf("\n");
        closedir(dir);
}

void ls2file(int flag){
        int flags=O_CREAT|O_WRONLY|O_TRUNC;
        if(flag==1){
                flags=O_CREAT|O_WRONLY|O_APPEND;
        }
        int fd;
        fd=open(cmd[2],flags,0644);
        DIR *dir;
        struct dirent *it;
        dir=opendir(".");
        while((it=readdir(dir))){
                if(it->d_name[0]!='.'){
                        write(fd,it->d_name,strlen(it->d_name));
                        write(fd,"\n",1);
                }
        }
        close(fd);

}

void cd(){
        chdir(cmd[1]);
}

void echo(){
        printf("%s\n",cmd[1]);
}
void echo2file(int flag){
        int flags=O_CREAT|O_WRONLY|O_TRUNC;
        if(flag==1){
                flags=O_CREAT|O_WRONLY|O_APPEND;
        }
        int fd;
        fd=open(cmd[3],flags,0644);
        write(fd,cmd[1],strlen(cmd[1]));
        write(fd,"\n",1);
        close(fd);

}


void cat(){
        int fd,nbytes;
        if((fd=open(cmd[1],O_RDONLY,0644))<0){
                printf("Open file failed!\n");
                return;
        }
        char buf[1];
        while((nbytes=read(fd,buf,1))>0){
                printf("%s",buf);
        }
        close(fd);
}

void cat2file(int flag){
        int fd1,fd2,nbytes;
        int flags=O_CREAT|O_WRONLY|O_TRUNC;
        if(flag==1){
                flags=O_CREAT|O_WRONLY|O_APPEND;
        }
        if((fd1=open(cmd[1],O_RDONLY,0644))<0){
                printf("Open file failed\n");
                return;
        }
        fd2=open(cmd[3],flags,0644);
        char buf[1];
        while((nbytes=read(fd1,buf,1))>0){
                write(fd2,buf,1);
        }
        close(fd1);
        close(fd2);

}

void cat2grep(){
        int fd,nbytes;
        if((fd=open(cmd[1],O_RDONLY,0644))<0){
                printf("Open file failed\n");
                return;
        }
        char buf[1024]={'\0'};
        int len=0;
        char c[10];
        while((nbytes=read(fd,c,1))>0){
                if(c[0]=='\n'){
                        buf[len++]='\0';
                        if(strstr(buf,cmd[4])){
                                printf("%s\n",buf);
                        }
                        len=0;
                        continue;
                }
                buf[len++]=c[0];
        }
        buf[len++]='\0';
        if(strstr(buf,cmd[4])){
                printf("%s\n",buf);
        }
        close(fd);
}
