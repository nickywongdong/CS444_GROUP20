/*
 * A sample, extra-simple block driver.
 *
 * Copyright 2003 Eklektix, Inc.  Redistributable under the terms
 * of the GNU GPL.
 */

/* 
 * Sample from https://lwn.net/Articles/58720/
 * Used for Operating Systems II Spring Term 2018 - Oregon State University
*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/crypto.h>   /* Adding for crypto capabilities */

MODULE_LICENSE("Dual BSD/GPL");
static char *Version = "1.3";

static int major_num = 0;
module_param(major_num, int, 0);
static int hardsect_size = 512;
module_param(hardsect_size, int, 0);
static int nsectors = 1024;  /* How big the drive is */
module_param(nsectors, int, 0);

/*
 * We can tweak our hardware sector size, but the kernel talks to us
 * in terms of small sectors, always.
 */
#define KERNEL_SECTOR_SIZE 512

/*
 * Our request queue.
 */
static struct request_queue *Queue;

/*
 * The internal representation of our device.
 */
static struct sbd_device {
    unsigned long size;
    spinlock_t lock;
    u8 *data;
    struct gendisk *gd;
} Device;


/*
 * Crypto Instantiation
 * Crypto Key defined here
 * Crypto Key must be 16 chars long
 */
struct crypto_cipher *tfm;
static char *key = "1234567890123456";
module_param(key, charp, 0644);
static int keylen = 16;
module_param(keylen, int, 0644);



/*
 * Handle an I/O request.
 */
static void sbd_transfer(struct sbd_device *dev, unsigned long sector,
		unsigned long nsect, char *buffer, int write)
{
    int i;

    if (write)
        printk("simple_blk_dev.c: sbd_transfer() -- Writing Data\n");
    else if (!write)
        printk("simple_blk_dev.c: sbd_transfer() -- Reading Data\n");

    unsigned long offset = sector*hardsect_size;
    unsigned long nbytes = nsect*hardsect_size;
    
    if ((offset + nbytes) > dev->size) {
	printk (KERN_NOTICE "sbd: Beyond-end write (%ld %ld)\n", offset, nbytes);
	return;
    }


    /* 
     * Area change to allow for Crypto
     *
     * Encrypt/Decrypt data as it is transferring one block at a time
    */
    if (write) {
        //memcpy(dev->data + offset, buffer, nbytes);
        printk("simple_blk_dev.c -- Write %lu bytes to device data\n", nbytes);

        destination = dev->data + offset;
        source = buffer;

        for (i = 0; i < nbytes; i += crypto_cipher_blocksize(tfm)) {
            /* Use crypto cipher handler and tfm to encrypt data one block at a time*/
            crypto_cipher_encrypt_one(
                    tfm,                    /* Cipher handler */
                    dev->data + offset + i, /* Destination */
                    buffer + i              /* Source */
                    );
        }

        printk("simple_blk_dev.c -- UNENCRYPTED DATA VIEW:\n");
        for (i = 0; i < 100; i++) {
            printk("%u", (unsigned) *destination++);
        }

        printk("\nsimple_blk_dev.c -- ENCRYPTED DATA VIEW:\n");
        for (i = 0; i < 100; i++) {
            printk("%u", (unsigned) *source++);
        }
        printk("\n");

    }
    else {
        printk("simple_blk_dev.c -- Read %lu bytes to device data\n", nbytes);

        destination = dev->data + offset;
        source = buffer;

        for (i = 0; i < nbytes; i += crypto_cipher_blocksize(tfm)) {
            /* Use crypto cipher handler and tfm to decrypt data one block at a time*/
            crypto_cipher_decrypt_one(
                    tfm,                    /* Cipher handler */
                    buffer + i,             /* Destination */
                    dev->data + offset + i  /* Source */
                    );
        }

        printk("simple_blk_dev.c -- UNENCRYPTED DATA VIEW:\n");
        for (i = 0; i < 100; i++) {
            printk("%u", (unsigned) *destination++);
        }

        printk("\nsimple_blk_dev.c -- ENCRYPTED DATA VIEW:\n");
        for (i = 0; i < 100; i++) {
            printk("%u", (unsigned) *source++);
        }
        printk("\n");
    }
}

static void sbd_request(request_queue_t *q)
{
    struct request *req;

    while ((req = elv_next_request(q)) != NULL) {
	if (! blk_fs_request(req)) {
	    printk (KERN_NOTICE "Skip non-CMD request\n");
	    end_request(req, 0);
	    continue;
	}
	sbd_transfer(&Device, req->sector, req->current_nr_sectors,
			req->buffer, rq_data_dir(req));
	end_request(req, 1);
    }
}



/*
 * Ioctl.
 */
int sbd_ioctl (struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	long size;
	struct hd_geometry geo;

	switch(cmd) {
	/*
	 * The only command we need to interpret is HDIO_GETGEO, since
	 * we can't partition the drive otherwise.  We have no real
	 * geometry, of course, so make something up.
	 */
	    case HDIO_GETGEO:
		size = Device.size*(hardsect_size/KERNEL_SECTOR_SIZE);
		geo.cylinders = (size & ~0x3f) >> 6;
		geo.heads = 4;
		geo.sectors = 16;
		geo.start = 4;
		if (copy_to_user((void *) arg, &geo, sizeof(geo)))
			return -EFAULT;
		return 0;
    }

    return -ENOTTY; /* unknown command */
}




/*
 * The device operations structure.
 */
static struct block_device_operations sbd_ops = {
    .owner           = THIS_MODULE,
    .ioctl	     = sbd_ioctl
};

static int __init sbd_init(void)
{

    //Testing for Output
    printk("simple_blk_dev.c -- Initializing\n");

/*
 * Set up our internal device.
 */
    printk("simple_blk_dev.c -- Set up internal device\n");


    Device.size = nsectors*hardsect_size;
    spin_lock_init(&Device.lock);
    Device.data = vmalloc(Device.size);
    if (Device.data == NULL)
	return -ENOMEM;
/*
 * Get a request queue.
 */

    printk("simple_blk_dev.c -- Get a request queue\n");

    Queue = blk_init_queue(sbd_request, &Device.lock);
    if (Queue == NULL)
	    goto out;
    blk_queue_hardsect_size(Queue, hardsect_size);
/*
 * Get registered.
 */

    printk("simple_blk_dev.c -- Get registered\n");

    major_num = register_blkdev(major_num, "sbd");
    if (major_num <= 0) {
	printk(KERN_WARNING "sbd: unable to get major number\n");
	goto out;
    }

    /* Initialize cypto and set key
     * ctrypto_alloc_cipher are: crypto driver name, type, and mask
     */
    tfm = crypto_alloc_cipher("aes", 0, 0);

    if (IS_ERR(tfm))
        printk("simple_blk_dev.c -- Unable to allocate cipher\n");
    else
        printk("simple_blk_dev.c -- Allocated cipher\n");

    /* Crypto debugging print statements */
    printk("simple_blk_dev.c -- Block Cipher Size: %u\n", crypto_cipher_blocksize(tfm));
    printk("simple_blk_dev.c -- Crypto key: %s\n", key);
    printk("simple_blk_dev.c -- Key Length: %d\n", keylen);

/*
 * And the gendisk structure.
 */

    printk("simple_blk_dev.c -- And the gendisk structure\n");

    Device.gd = alloc_disk(16);
    if (! Device.gd)
    goto out_unregister;
    Device.gd->major = major_num;
    Device.gd->first_minor = 0;
    Device.gd->fops = &sbd_ops;
    Device.gd->private_data = &Device;
    strcpy (Device.gd->disk_name, "sbd0");
    set_capacity(Device.gd, nsectors*(hardsect_size/KERNEL_SECTOR_SIZE));
    Device.gd->queue = Queue;
    add_disk(Device.gd);

    printk("simple_blk_dev.c - Successful initialization\n");

    return 0;

  out_unregister:
    unregister_blkdev(major_num, "sbd");
  out:
    vfree(Device.data);
    return -ENOMEM;
}

static void __exit sbd_exit(void)
{
    del_gendisk(Device.gd);
    put_disk(Device.gd);
    unregister_blkdev(major_num, "sbd");
    blk_cleanup_queue(Queue);
    vfree(Device.data);
}
	
module_init(sbd_init);
module_exit(sbd_exit);

MODULE_AUTHOR("OS2_Spring2018_Group20");
MODULE_DESCRIPTION("Simple Block Device Driver With Crypto");