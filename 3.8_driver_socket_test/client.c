#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <net/sock.h>
#define port 9734
#define bufsize 100
#define IP "127.0.0.1"
struct socket*     sock = NULL;



int receive(struct socket* sock, unsigned char* buf, int len)
{
        struct msghdr msg;
        struct kvec iov;
       
        int size = 0;

        if (sock->sk==NULL) return 0;

        iov.iov_base = buf;
        iov.iov_len = len;
/*
        msg.msg_flags = 0;
        msg.msg_name = addr;
        msg.msg_namelen  = sizeof(struct sockaddr_in);
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = NULL;
*/
        size = kernel_recvmsg(sock,&msg,&iov,1,len,0);
   

        return size;
}


int send(struct socket* sock, unsigned char* buf, int len)
{
        struct msghdr msg;
        struct kvec iov;
       
        int size = 0;

        if (sock->sk==NULL) return 0;

        iov.iov_base = buf;
        iov.iov_len = len;
        size = kernel_sendmsg(sock,&msg,&iov,1,len);
   
        return size;
}

int  init_module(void)
{

  struct sockaddr_in*   dest = {0};
  int                   retVal = 0;
  char buf[bufsize];
  int i;


  dest = (struct sockaddr_in*)kmalloc(sizeof(struct sockaddr_in), GFP_KERNEL);
  sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  dest->sin_family = AF_INET;
  dest->sin_addr.s_addr = in_aton(IP);
  dest->sin_port = htons(port);
  printk(KERN_EMERG "Connect to %X:%u\n", dest->sin_addr.s_addr, port);

  retVal = sock->ops->connect(sock, (struct sockaddr*)dest, sizeof(struct sockaddr_in), 0);

	for(i=0;i<50;i++)
	{
		buf[i] = 'a';  // 設定欲傳送的資料到 buff
	}
	for(i=50;i<100;i++)
	{
		buf[i]='x';
	}	
	//printk("buf=%s\n",buf);
  if (retVal >= 0) {
   printk(KERN_EMERG "Connected\n");
	// transfer_data(sock);
	send(sock,buf,bufsize);
   	memset(buf, '\0', sizeof(buf));
	receive(sock,buf,bufsize/2);
	printk("Client : receive data is %s\n",buf);
   }
  else
    printk(KERN_EMERG "Error %d\n", -retVal); 
 
  
  return (0);
sock_release(sock);

/*out:
  sock_release(sock);	
  return (0);
*/
}
void cleanup_module(void)
{
	sock_release(sock);
	printk(KERN_INFO "Goodbye client\n");
}

MODULE_LICENSE("GPL");
