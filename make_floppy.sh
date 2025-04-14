#!/bin/bash

sudo /sbin/losetup /dev/loop13 floppy.img
sudo mount /dev/loop13 /mnt
sudo cp src/kernel /mnt/kernel
sudo umount /dev/loop13
sudo /sbin/losetup -d /dev/loop13
