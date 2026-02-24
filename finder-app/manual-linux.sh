#!/bin/bash
set -e

SCRIPT_DIR=$(dirname "$(realpath "$0")")
REPO_ROOT=$(realpath "$SCRIPT_DIR/..")

OUTDIR=${1:-/tmp/aeld}
OUTDIR=$(realpath "$OUTDIR")

KERNEL_REPO=https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1.36.1

ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

echo "Using OUTDIR: $OUTDIR"

mkdir -p "$OUTDIR"
mkdir -p "$OUTDIR/rootfs"

################################
# Build Linux Kernel
################################

if [ ! -d "$OUTDIR/linux-stable" ]; then
    git clone --depth 1 --branch ${KERNEL_VERSION} $KERNEL_REPO $OUTDIR/linux-stable
fi

cd $OUTDIR/linux-stable

if [ ! -f "arch/${ARCH}/boot/Image" ]; then

	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
	make -j$(nproc) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} Image
fi
cp arch/${ARCH}/boot/Image $OUTDIR/

################################
# Build BusyBox
################################

cd $OUTDIR

if [ ! -d "busybox-${BUSYBOX_VERSION}" ]; then
    wget https://busybox.net/downloads/busybox-${BUSYBOX_VERSION}.tar.bz2
    tar -xjf busybox-${BUSYBOX_VERSION}.tar.bz2
fi

cd busybox-${BUSYBOX_VERSION}
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
sed -i 's/^# CONFIG_STATIC is not set/CONFIG_STATIC=y/' .config
make -j$(nproc) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CONFIG_PREFIX=$OUTDIR/rootfs install

################################
# Create RootFS Layout
################################

cd $OUTDIR/rootfs
mkdir -p dev proc sys tmp home

sudo mknod -m 666 dev/null c 1 3 || true
sudo mknod -m 600 dev/console c 5 1 || true

################################
# Cross Compile Writer
################################

cd $REPO_ROOT/finder-app
make clean
make CROSS_COMPILE=${CROSS_COMPILE}
cp writer $OUTDIR/rootfs/home/
chmod +x $OUTDIR/rootfs/home/writer

################################
# Copy Assignment Files
################################
mkdir -p $OUTDIR/rootfs/home/conf
cp $REPO_ROOT/finder-app/finder.sh $OUTDIR/rootfs/home/
cp $REPO_ROOT/finder-app/finder-test.sh $OUTDIR/rootfs/home/
cp $REPO_ROOT/finder-app/autorun-qemu.sh $OUTDIR/rootfs/home/
cp $REPO_ROOT/finder-app/conf/username.txt $OUTDIR/rootfs/home/conf/
cp $REPO_ROOT/finder-app/conf/assignment.txt $OUTDIR/rootfs/home/conf/

################################
# Fix finder-test.sh path
################################

sed -i 's|../conf/assignment.txt|conf/assignment.txt|' \
$OUTDIR/rootfs/home/finder-test.sh

chmod +x $OUTDIR/rootfs/home/finder.sh
chmod +x $OUTDIR/rootfs/home/finder-test.sh
chmod +x $OUTDIR/rootfs/home/autorun-qemu.sh

################################
# Create initramfs
################################

cd $OUTDIR/rootfs
find . | cpio -H newc -ov --owner root:root > $OUTDIR/initramfs.cpio
gzip -f $OUTDIR/initramfs.cpio

echo "Build complete."
