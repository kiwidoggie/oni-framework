#include <oni/utils/types.h>
#include <sys/uio.h>

#include <oni/utils/kernel.h>
#include <oni/utils/kdlsym.h>



int proc_rw_mem(struct proc* p, void* ptr, size_t size, void* data, size_t* n, int write)
{
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);

	struct thread* td = curthread;
	struct iovec iov;
	struct uio uio;
	int ret;

	if (!p) {
		ret = EINVAL;
		goto error;
	}

	if (size == 0) {
		if (n)
			*n = 0;
		ret = 0;
		goto error;
	}

	memset(&iov, 0, sizeof(iov));
	iov.iov_base = (caddr_t)data;
	iov.iov_len = size;

	memset(&uio, 0, sizeof(uio));
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	uio.uio_offset = (off_t)ptr;
	uio.uio_resid = (ssize_t)size;
	uio.uio_segflg = UIO_SYSSPACE;
	uio.uio_rw = write ? UIO_WRITE : UIO_READ;
	uio.uio_td = td;

	int(*proc_rwmem)(struct proc *p, struct uio *uio) = kdlsym(proc_rwmem);
	ret = proc_rwmem(p, &uio);
	if (n)
		*n = (size_t)((ssize_t)size - uio.uio_resid);

error:
	return ret;
}