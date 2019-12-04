#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/kthread.h>
#include <linux/string.h>
#include <net/sock.h>
#include <net/inet_connection_sock.h>
#define bufsize 100


#define err(msg) printk(KERN_ERR "%s failed.\n", msg)

static struct task_struct *task;



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
        size = kernel_recvmsg(sock,&msg,&iov,1,len,MSG_WAITALL);
   

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

static int threadfn(void *data)
{
    struct socket *sock;
    struct sockaddr_in addr;
    int size,i;
    char buf[bufsize+1];

    if (sock_create_kern(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock) < 0) {
        err("sock_create_kern");
        goto out;
    }

    sock->sk->sk_reuse = 1;

    memset(&addr, '\0', sizeof(addr));    
    addr.sin_family     = AF_INET;
    addr.sin_addr.s_addr    	= htonl(INADDR_ANY);
    addr.sin_port        = htons(9734);

    if (kernel_bind(sock, &addr, sizeof(addr)) < 0) {
        err("kernel_bind");
        goto err;
    }

    if (kernel_listen(sock, 1024) < 0) {
        err("kernel_listen");
        goto err;
    }

    while (!kthread_should_stop()) {

        struct socket *newsock;
        struct inet_connection_sock *icsk = inet_csk(sock->sk);
    
        	struct socket_wq *wq;
     		rcu_read_lock();
                wq = rcu_dereference(sock->sk->sk_wq);
        wait_event_interruptible(wq->wait,!reqsk_queue_empty(&icsk->icsk_accept_queue) || kthread_should_stop());


        if (!reqsk_queue_empty(&icsk->icsk_accept_queue)) {
            if (kernel_accept(sock, &newsock, MSG_WAITALL) != 0) {
                err("kernel_accept");
                goto err;
            }
		
            printk(KERN_INFO "Server : accept a new connection request.\n");
	   // transfer_data(newsock);


	
		  memset(&buf, 0, bufsize+1);
		  size = receive(newsock, buf, bufsize/2);
 		  printk(KERN_INFO "Server : received %d bytes\n", size);
   //data processing 
 		  printk("Server : received data: %s\n", buf);
		  size = receive(newsock, buf, bufsize/2);
		if (size < 0)
		{
                        printk(KERN_INFO ": error getting datagram, sock_recvmsg error = %d\n", size);
			break;
		}                
		else 
                {
                        printk(KERN_INFO "Server : received %d bytes\n", size);
                         //data processing 
                        printk("Server : received data: %s\n", buf);
			
			for( i=0;i<bufsize/2;i++)
			{
			buf[i]+=1;
			}
			buf[i]=0      ;
			// sending 
			//printk("buf=%s\n",buf);
                        size=send(newsock, buf, bufsize/2+1);
                     	if(size<0)
				printk("Server : error send\n");
                      
                }
	    
	
			//printk("ch =%c\n",ch);
            sock_release(newsock);
        }

    }

err:
    sock_release(sock);
out:
    return 0;
}

static int __init main_init(void)
{
    task = kthread_run(threadfn, NULL, "listen thread");

    return 0;
}

static void __exit main_exit(void)
{
    kthread_stop(task);
}

module_init(main_init);
module_exit(main_exit);
MODULE_LICENSE("GPL");
