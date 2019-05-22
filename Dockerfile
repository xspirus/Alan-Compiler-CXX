FROM ubuntu:18.04

ENV LANG=C.UTF-8
ENV PATH=${PATH}:/root/.pyenv/bin:/root/.pyenv/shims
ENV PYENV_ROOT=/root/.pyenv

SHELL [ "/bin/bash", "-c" ]

RUN apt update
RUN apt install -y curl wget zip
RUN apt install -y git make flex bison clang cmake zlib1g-dev
RUN apt install -y python3 python3-pip python3-dev
RUN pip3 install pipenv
RUN curl https://pyenv.run | bash

WORKDIR /root
RUN git clone https://github.com/xspirus/Alan-Compiler-CXX.git
WORKDIR /root/Alan-Compiler-CXX
RUN git fetch --all
RUN git checkout arch/ubuntu-18.04

RUN make

COPY test.py .
