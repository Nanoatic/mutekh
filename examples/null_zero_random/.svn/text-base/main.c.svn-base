
#include <drivers/char/random/random.h>
#include <drivers/char/null/null.h>
#include <drivers/char/zero/zero.h>

#include <stdint.h>
#include <stdio.h>

struct device_s random_dev, zero_dev, null_dev;

void app_start()
{
  uint8_t buf[256];

  device_init(&random_dev);
  device_init(&zero_dev);
  device_init(&null_dev);

  dev_random_init(&random_dev, NULL);
  dev_null_init(&null_dev, NULL);
  dev_zero_init(&zero_dev, NULL);

  memset(buf, 0x55, sizeof(buf));

  dev_char_wait_read(&null_dev, buf, sizeof(buf));
  printk("%P\n", buf, sizeof(buf));

  dev_char_wait_read(&zero_dev, buf, sizeof(buf));
  printk("%P\n", buf, sizeof(buf));

  dev_char_wait_read(&random_dev, buf, sizeof(buf));
  printk("%P\n", buf, sizeof(buf));
}

