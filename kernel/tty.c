#include <rook/core.h>
#include <rook/kernel/tty.h>

rk_device_class_t rk_device_class_tty =
	RK_DEVICE_CLASS(tty, sizeof(rk_tty_device_t));

/* read from tty */
extern size_t rk_tty_device_read(rk_device_t *device, char *buffer, size_t count) {

	rk_tty_device_t *tty = RK_TTY_DEVICE(device);

	if (!tty->read) return 0;

	rk_spinlock_acquire(&tty->read_lock);
	count = tty->read(tty, buffer, count);
	rk_spinlock_release(&tty->read_lock);

	return count;
}

/* write to tty */
extern size_t rk_tty_device_write(rk_device_t *device, const char *buffer, size_t count) {

	rk_tty_device_t *tty = RK_TTY_DEVICE(device);

	if (!tty->write) return 0;

	rk_spinlock_acquire(&tty->write_lock);
	count = tty->write(tty, buffer, count);
	rk_spinlock_release(&tty->write_lock);

	return count;
}
