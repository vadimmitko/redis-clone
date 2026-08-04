# syntax=docker/dockerfile:1

FROM ubuntu:latest
LABEL Description="Build environment"

ENV HOME=/root

SHELL ["/bin/bash", "-c"]

RUN apt-get update && apt-get -y --no-install-recommends install \
    clang \
    clangd \
    cmake \
    redis-tools \
    build-essential \
    git \
    ca-certificates \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*
