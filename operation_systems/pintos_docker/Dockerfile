FROM --platform=linux/amd64 ubuntu:16.04 

RUN apt-get update && \
    apt-get install -y \
        gcc-multilib \
        make \
        perl \
        qemu \
        qemu-utils \
        bochs \
        gdb \
        vim \
        nano \
    && rm -rf /var/lib/apt/lists/*

RUN ln -sf /usr/bin/qemu-system-x86_64 /usr/bin/qemu

COPY ./pintos /pintos

RUN cd /pintos/src/utils && \
    cp -f backtrace pintos pintos-mkdisk pintos-set-cmdline Pintos.pm pintos-gdb /usr/bin/ && \
    cp ../misc/gdb-macros /usr/bin/ && \
    chmod 755 /usr/bin/backtrace /usr/bin/pintos /usr/bin/pintos-mkdisk /usr/bin/pintos-set-cmdline /usr/bin/pintos-gdb && \
    chmod 644 /usr/bin/Pintos.pm /usr/bin/gdb-macros

WORKDIR /pintos/src/threads
RUN make clean && make

CMD ["/bin/bash"]
